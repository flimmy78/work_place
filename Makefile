CC       = arm-linux-gcc -Wall
CFLAGS   = -lsqlite3 -lm -lpthread

INCPATH   = -I/shared/sqlite_target/include
LIBS      = -L/shared/sqlite_target/lib 
#INCPATH  = -I/opt/FriendlyARM/tiny210/linux/rootfs_qtopia_qt4/opt/sqlite3/include
#LIBS     = -L/opt/FriendlyARM/tiny210/linux/rootfs_qtopia_qt4/opt/sqlite3/lib

all:main
	#$(CC) SendInfo.c -o SendInfo  $(INCPATH) $(LIBS) $(CFLAGS) 
	#./make
	

main:main.c const.h public.h strseparate.h *.inc  recordLog.h  
	$(CC) main.c  recordLog.c   -o main $(INCPATH) $(LIBS) $(CFLAGS)

clean:
	rm -f *.c~
	rm -f *.h~
	rm -f *.inc~
	rm -f *.bak
	rm main

