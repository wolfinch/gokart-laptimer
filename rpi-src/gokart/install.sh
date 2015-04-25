#! /bin/sh
#Install script for gokartd

echo "Cleaning up existing installation\n"
rm -rf /etc/gokart
echo "Installing gokartd..\n"

mkdir /etc/gokart
mkdir /etc/gokart/sbin
mkdir /etc/gokart/var
mkdir /etc/gokart/var/run
mkdir /etc/gokart/data
cp -f lircd.conf /etc/lirc/lircd.conf
service lirc stop
service lirc start
cp -f gokartd /etc/gokart/sbin/gokartd
cp -f gokart-server.py /etc/gokart/sbin/gokart-server.py
cp gokartd.conf /etc/gokart/gokartd.conf
cp -f gokart_init /etc/init.d/gokart
cp -f gokart-server_init /etc/init.d/gokart-server
update-rc.d gokart-server start 95 2 . stop 95 2
#touch /etc/gokart/var/run/gokartd.pid
#touch /etc/gokart/var/run/lock

echo "Installation complete!\n"

