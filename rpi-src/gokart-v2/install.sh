#! /bin/sh
#Install script for gokartd

echo "Cleaning up existing installation\n"
rm -rf /etc/gokart
echo "Installing gokartd..\n"

mkdir /etc/gokart
mkdir /etc/gokart/sbin
mkdir /etc/gokart/var
#mkdir /var/run/gokart
mkdir /etc/gokart/data
cp -f gokartd /etc/gokart/sbin/gokartd
cp -f gokart-server.py /etc/gokart/sbin/gokart-server.py
cp gokartd.conf /etc/gokart/gokartd.conf
cp -f gokart_init /etc/init.d/gokart
cp -f gokart-server_init /etc/init.d/gokart-server
update-rc.d gokart-server start 95 2 . stop 95 2
#touch /var/run/gokartd.pid
#touch /var/run/gokartd.lock

echo "Installation complete!\n"

