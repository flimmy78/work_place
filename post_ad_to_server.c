
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <errno.h>
#include   <netdb.h>
#include   <fcntl.h>
#include   <unistd.h>
#include   <time.h>
#include   <math.h>
#include   <sys/types.h>
#include   <netinet/in.h>
//#include   <sys/stat.h>
#include   <arpa/inet.h>
#include   <sys/ioctl.h>

//#include   <net/if.h>
#include   <pthread.h>
#include   <linux/watchdog.h>

#include   <sqlite3.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
char serverDomain1[100] = "server1.cartvu-media.com";

char mac[20] = "000011700009";

#define HTTP_PORT          80


char ip[100] = "203.186.75.165";

#define MAXLEN_RESPONSE    1024
pthread_mutex_t  send_mutex;


#include "const.h"
#include "strseparate.h"

#include "sendRequest.inc"

#include "post_ad_ok.c"







int main(void) {
    int ret;
    ret = pthread_mutex_init(&send_mutex, NULL); //init mutex ≥ı ºªØª•≥‚À¯


    post_ad_ok("4", 1);
    sleep(2);
    post_ad_ok("4", 2);

    return 0;

}
