This is a README for the gokartd:

HOWTO: 
INSTALL:

1. place gokartd startup script in /etc/init.d/gokart
2. chkconfig –add gokart | update-rc.d gokart enable 4
3. chkconfig –level 2345 gokart on
4. Place the following config files as below
CONFIG_FILE=/etc/gokart/gokartd.conf PID_FILE=/etc/gokart/var/run/gokartd.pid CMD_FILE=/etc/gokart/sbin/gokartd

* data in:
/etc/gokart/data/
START:
service gokartd start

STOP: 
service gokartd stop

 mkdir /etc/gokart
 mkdir /etc/gokart/sbin
 mkdir /etc/gokart/var
 mkdir /etc/gokart/var/run
 mkdir /etc/gokart/data
 cp a.out /etc/gokart/sbin/gokartd

touch /etc/gokartd/var/run/gokartd.pid
cp gokartd.conf /etc/gokartd/gokartd.conf
cp gokartd /etc/gokartd/sbin/gokartd

 vi gokartd.conf
 cp gokartd.conf /etc/gokart/.
 touch /etc/gokart/var/run/gokartd.pid
 vi gokart
 chmod 777 gokart
 cp gokart /etc/init.d/.