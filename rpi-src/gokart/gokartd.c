
/****************************************************************************
 ** gokartd.c ***************************************************************
 ***(c) Joshith RK***********************************************************
 ****************************************************************************
 * gokartd - watch the codes as lircd recognize them
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>
#include <signal.h>


static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"repeat", required_argument, NULL, 'r'},
        {"daemon", no_argument, NULL, 'd'},
        {0, 0, 0, 0}
};

#define PACKET_SIZE      256
#define MAX_KARTS        32
#define LIRCD                   "/var/run/lirc/lircd"
#define PISTILLPATH             "/usr/bin/raspistill"
#define PISTILLARGS             "-vf -o "
#define OUTDIR                  "/etc/gokart/data/"
#define PIDFILE                 "/etc/gokart/var/run/gokartd.pid"
#define LOCKFILE                "/etc/gokart/var/run/lock"
#define LOGFILE                 "/etc/gokart/var/gokartd.log"
#define DEBUG_NO

#ifndef DEBUG
#define log_print(args...) { \
            printf (args);   \
        };
#else
#define log_print(args...) { \
        fprintf(g_log_fd, args); \
        fflush (g_log_fd); \
};
#endif

typedef struct karts_ {
    char    kart_name[20];
    short   kart_id;
    int                 num_laps;
    struct timeval      lap_time;
    struct timeval      new_lap_time;
    FILE                *fp;
}karts_t;

karts_t all_karts[MAX_KARTS];
int g_lircd_fd = 0;
FILE *g_log_fd = NULL;
static char *lirc_buffer = NULL;


void
gokart_snapapic(const char *name, struct timeval *tv) {
    pid_t pid = 0;
    char img_path[128] = "";

    snprintf(img_path, 64, "%s%s_%ul.jpg", PISTILLARGS, name, tv->tv_sec);

    log_print ("raspistill arg path: %s\n", img_path);
    pid = fork ();

    if (pid == -1) {
        /* fork failed */
        return;
    }

    if (pid > 0) {
        /*this is parent */
        return;
    } else {
        /* this is child */
        if(-1 == execv(PISTILLPATH, (char * const *)img_path)) {
            log_print ("raspistill exec failed path:%s arg path: %s\n", PISTILLPATH, img_path);
        }
        exit (1);
    }

}

int gokart_nextcode(char **code)
{
    static int packet_size = PACKET_SIZE;
    static int end_len = 0;
    ssize_t len = 0;
    char *end, c;

    *code = NULL;
    if (lirc_buffer == NULL) {
        lirc_buffer = (char *)malloc(packet_size + 1);
        if (lirc_buffer == NULL) {
            log_print("%s: out of memory\n", __func__);
            return (-1);
        }
        lirc_buffer[0] = 0;
    }
    while ((end = strchr(lirc_buffer, '\n')) == NULL) {
        if (end_len >= packet_size) {
            char *new_buffer;

            packet_size += PACKET_SIZE;
            new_buffer = (char *)realloc(lirc_buffer, packet_size + 1);
            if (new_buffer == NULL) {
                return (-1);
            }
            lirc_buffer = new_buffer;
        }
        len = read(g_lircd_fd, lirc_buffer + end_len, packet_size - end_len);
        if (len <= 0) {
            if (len == -1 && errno == EAGAIN)
                return (0);
            else
                return (-1);
        }
        end_len += len;
        lirc_buffer[end_len] = 0;
        /* return if next code not yet available completely */
        if ((end = strchr(lirc_buffer, '\n')) == NULL) {
            return (0);
        }
    }
    /* copy first line to buffer (code) and move remaining chars to
       lirc_buffers start */
    end++;
    end_len = strlen(end);
    c = end[0];
    end[0] = 0;
    *code = strdup(lirc_buffer);
    end[0] = c;
    memmove(lirc_buffer, end, end_len + 1);
    if (*code == NULL)
        return (-1);
    return (0);
}

int check_fd_fine(FILE *fp) {
    struct stat _stat;
    int ret = -1;
    int fd = fileno(fp);

    if(fd > 1) {
        if(!fstat(fd, &_stat)) {
            if(_stat.st_nlink >= 1) {
                return 0;
            }
            log_print ("file removed fd: %d st_nlink:%d\n", fd, _stat.st_nlink);
        }
    }
    log_print ("failure fd: %d\n", fd);
    return ret;
}

static int gokart_digest_code(char *code)
{
    int rep;
    char *backup;
    char *remote, *button;
    char *s = NULL;
    struct lirc_config_entry *scan;
    int exec_level;
    karts_t        *this_kart = NULL;
    struct timeval tv;
    FILE           *fp = NULL;

    if (sscanf(code, "%*x %x %*s %*s\n", &rep) == 1) {
        backup = strdup(code);
        if (backup == NULL)
            return (-1);

        rep = atoi(strtok(backup, " "));
        strtok(NULL, " ");
        button = strtok(NULL, " ");
        remote = strtok(NULL, "\n");

        if (button == NULL || remote == NULL) {
            free(backup);
            return (0);
        }

        log_print ("LOG: button_code: %x, but_id: %s, remote: %s\n", rep, button, remote);

        if (rep <= 0 || rep >= MAX_KARTS) { // Error
            free(backup);
            return -1;
        }

        /* Now get the kart struct */
        this_kart = &(all_karts[rep]);

        /* slap time stamp */
        gettimeofday(&tv, 0);

        /* Check if this signal redundant,
         * do not accept signal <10sec interval. drop, and update last seen time
         *  */
        if (this_kart->new_lap_time.tv_sec > (tv.tv_sec - 10)) {
            log_print ("Repetitive signal \n");
            this_kart->new_lap_time = tv;

            free(backup);
            return 1;
        }

        /* New whole set session or
         * individual kart session (only file for this kart was deleted) */
        if (this_kart->kart_id == 0 || check_fd_fine (this_kart->fp)) {
            /* We are seeing this kart for the first time */
            strncpy (this_kart->kart_name, button, 20);
            this_kart->kart_id = rep;
            this_kart->num_laps = 0;
            this_kart->lap_time = this_kart->new_lap_time = tv;

            /* open a file for data */
            if ((fp = fopen (button, "w+")) == NULL) {
                log_print ("file open with name : %s failed\n", button);
                free (backup);
                return -1;
            }

            this_kart->fp = fp;
            fprintf (fp, "KART:%s:START_TIME:%d.%d:END_TIME:%d.%d:\n",
                this_kart->kart_name,0,0, tv.tv_sec, tv.tv_usec);
            fflush (this_kart->fp);
            /* take a snap */
            gokart_snapapic (this_kart->kart_name, &(this_kart->lap_time));

            /* done */
            free(backup);
            return 0;
        }

        /* subsequent lap */
        if (this_kart->kart_id != rep) {
            /* Error */
            log_print ("inconsistent data. kart_id db: %x, kart_id signal: %x",
                    this_kart->kart_id, rep);
            free(backup);
            return -1;
        }

        fprintf (this_kart->fp, "KART:%s:START_TIME:%d.%d:END_TIME:%d.%d:\n",
            this_kart->kart_name, this_kart->lap_time.tv_sec,
            this_kart->lap_time.tv_usec, tv.tv_sec, tv.tv_usec);
        this_kart->lap_time = this_kart->new_lap_time = tv;
        fflush (this_kart->fp);

        /* take a snap */
        gokart_snapapic (this_kart->kart_name, &(this_kart->lap_time));

        free(backup);
    }

    return (0);
}

int main(int argc, char *argv[])
{
    int  i;
    char buf[128];
    struct sockaddr_un addr;
    int c;
    int delay = 10;
    char *progname;
    char *code;
    int  daemonize = 0;
    char cwd[100] = "";
    pid_t pid = 0;

    progname = "gokartd";

    /* Init all karts */
    memset(all_karts, 0, sizeof(all_karts));

    addr.sun_family = AF_UNIX;

    while ((c = getopt_long(argc, argv, "hvdr", long_options, NULL))
            != EOF) {
        switch (c) {
        case 'h':
            printf("Usage: %s [socket]\n", argv[0]);
            printf("\t -h --help \t\tdisplay usage summary\n");
            printf("\t -v --version \t\tdisplay version\n");
            printf("\t -r --repeat \t\t Repeat delay interval\n");
            printf("\t -d --daemon \t\t Run as a daemon\n");
            return (EXIT_SUCCESS);
        case 'v':
            printf("%s\n", progname);
            return (EXIT_SUCCESS);
        case 'd':
            daemonize = 1;
            break;
        case 'r':
            delay = atoi(argv[optind]);
            break;
        case '?':
            fprintf(stderr, "unrecognized option: -%c\n", optopt);
            fprintf(stderr, "Try `%s --help' for more information.\n", progname);
            return (EXIT_FAILURE);
        }
    }
    //setup log
#ifdef DEBUG
    g_log_fd = fopen (LOGFILE, "w+");
    //  if(!g_log_fd) g_log_fd = 1;
#endif

    /* set SIGCHLD to SIG_IGN, don't care about childs */
    signal(SIGCHLD, SIG_IGN);

    strcpy(addr.sun_path, LIRCD);

#if 0
    if (argc == optind) {
        /* no arguments */
    } else if (argc == optind + 1) {
        /* one argument */
        printf ("one argument\n");
        strncpy(addr.sun_path, argv[optind], sizeof(addr.sun_path) -1);
    } else {
        fprintf(stderr, "%s: incorrect number of arguments.\n", progname);
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
        return (EXIT_FAILURE);
    }
#endif

    //Check if we are aleardy running
    if(access(LOCKFILE, F_OK)) {
        //We are not running, create lock file
        sprintf(buf, "touch %s\n", LOCKFILE);
        system (buf);
    } else {
        log_print ("gokartd is already running \n");
        exit (1);
    }

    g_lircd_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_lircd_fd == -1) {
        perror("socket");
        exit(errno);
    };
    if (connect(g_lircd_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(errno);
    };

    if (daemonize) {
        if (daemon(0, 0) == -1) {
            fprintf(stderr, "%s: can't daemonize\n", progname);
            exit(-1);
        }
    }

    //Change CDW to OUTDIR
    if(chdir(OUTDIR)) {
        log_print("Couldn't Change CWD to %s errno: %d\n", OUTDIR, errno);
        exit (-1);
    }
    log_print("CWD: %s\n", getcwd(cwd, 100));

    // write pidfile
    pid = getpid();
    sprintf(buf, "echo %d > %s\n", (int)pid, PIDFILE);
    system (buf);

    while (gokart_nextcode(&code) == 0) {
        if (code == NULL)
            continue;

        gokart_digest_code (code);

    }
}

