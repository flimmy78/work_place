#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
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

#include   <arpa/inet.h>
#include   <sys/ioctl.h>


#include   <pthread.h>
#include   <linux/watchdog.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <linux/if.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    int sockfd = 0;
    int last_sockfd = -1;



    printf("start\n");
    while (1) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
        }
        if (last_sockfd == sockfd) {
            sleep(1);
        //    usleep(1000);
            printf("equal please wait here now sock_fd = %d  last fd = %d\n", sockfd, last_sockfd);
            if (close(sockfd) != 0) {
                perror("close sockfd error");
            }
        } else {
            sleep(1);
            printf("no equal now sock_fd = %d  last fd = %d\n", sockfd, last_sockfd);
        }
        last_sockfd = sockfd;
        close(sockfd);
    }




    return 0;
}
