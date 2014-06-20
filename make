#!/bin/sh


CC=arm-linux-gcc 

INCPATH=-I/shared/sqlite_target/include
LIBS=-L/shared/sqlite_target/lib 
#INCPATH  = -I/opt/FriendlyARM/tiny210/linux/rootfs_qtopia_qt4/opt/sqlite3/include
#LIBS     = -L/opt/FriendlyARM/tiny210/linux/rootfs_qtopia_qt4/opt/sqlite3/lib

source=SendInfo.c
target=SendInfo
flag=-Wall
ip=192.168.1.59
root=root
passwd=1

$CC $source recordLog.c -o $target $INCPATH $LIBS -lsqlite3 -lm -lpthread -Wall 
#arm-linux-gcc $source -o $target $flag


echo "open $ip 21
      user $root $passwd
      binary
      put $target
      chmod 777 $target
      close
      bye" |ftp -in 
_