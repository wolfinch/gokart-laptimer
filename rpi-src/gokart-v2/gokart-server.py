#!/usr/bin/python
################################################################################
#  gokart-server - The light-weight server for servicing control/status cmds   #
#  Copyright (C) 2015 Joshith (joe.cet@gmail.com)                              #
################################################################################
import SocketServer
import os

#RUN_DATA    ='/Users/joshith/Documents/Work/non-work/gokartd'#'/etc/gokartd/data/'
RUN_DATA    ='/etc/gokart/data/'
REMOTEHOST  ='localhost'
START_CNT   = 0


def CleanupRundata (self, devName):
    if (devName == "ALL"):
        if (0 != os.system("rm "+RUN_DATA+"*") ):
            self.send('\nfailed to cleaned up dev:'+devName)
            print('\nfailed to cleaned up dev:'+devName)
        else:
            #            self.send('stopped gokartd\n')
            print('\ncleaned up dev:'+devName)
    else:
        if (0 != os.system("rm "+RUN_DATA+devName+"_*") ):
            self.send('\nfailed to cleaned up dev:'+devName)
            print('\nfailed to cleaned up dev:'+devName)
        else:
            #            self.send('stopped gokartd\n')
            print('\ncleaned up dev:'+devName)

def StartOrStopCmd (self, start, dev):
    global START_CNT
    self.send('OK\r\n')
    
    if(start == True):
        CleanupRundata(self, dev)
        print 'Starting gokartd!!\n'
#        self.send('Starting gokartd\n')
        if (START_CNT <= 0):
            START_CNT = 0
            if (0 != os.system("service gokart start") ):
#               self.send('Failed to start gokartd\n')
                print('Failed to start gokartd\n')
            else:
#               self.send('started gokartd\n')
                print('started gokartd\n')
        START_CNT+=1
    else:
        print 'Stopping gokartd!!\n'
        self.send('Stopping gokartd\n')
        CleanupRundata(self, dev)
        START_CNT-=1
        if (START_CNT <= 0):
            START_CNT = 0
            if (0 != os.system("service gokart stop") ):
                self.send('Failed to stop gokartd\n')
                print('Failed to stop gokartd\n')
            else:
#               self.send('stopped gokartd\n')
                print('stopped gokartd\n')
    self.request.close()
    return

def ProcessStatusQuery(self, serverAddr):
    print "STATUS"
    self.send('OK\r\n')
    self.request.close()
#Send files in dir to server
    for file in os.listdir(RUN_DATA):
        curlcmd = 'curl -X POST -F file=@'+RUN_DATA+file+' http://'+serverAddr+':80/fileupload.php'
#        print curlcmd
        if (False == os.system(curlcmd)):
            #SUCCESSfully transferred file, now delete file if jpg
            if file.count(".jpg"):
                os.remove (file)
        else:
            print "File upload failed: "+file

class MyTCPHandler(SocketServer.BaseRequestHandler):
    def handle(self):
            self.data = self.request.recv(1024).strip()
#            print "{} wrote:".format(self.client_address[0])
#            print self.data
#            self.send('webserver ONLINE\n')
            #reqstring = ""
            self.data.format()
#            print self.data
            cmd = self.data.split(":");
#            print cmd[1]
            if (self.data.count("START") == 1):
                StartOrStopCmd(self, True, cmd[1])
            elif (self.data.count("STOP") == 1):
                StartOrStopCmd(self, False, cmd[1])
            elif (self.data.count("STATUS") == 1):
                ProcessStatusQuery(self, self.client_address[0])
            else:
                print "unidentified option\n"
#                self.send('unidentified option!!\n')
            return
    def send(self, content):
        data = 'HTTP/1.1 200 OK\r\n{}'.format(content)
#        print data
        self.request.sendall(data)

if __name__ == "__main__":
	HOST, PORT = "0.0.0.0", 9997

	server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
	server.serve_forever()
