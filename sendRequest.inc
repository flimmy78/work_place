/*************************************************************************
  Function: sendRequest(char *, char *, char *, unsigned int)
  Description: send a http request to server and get a response

  Entry: ip        ip address of server
         request   request string
         response  response buffer
         len       the length of response
  Return:0      success
         <0     fail
**************************************************************************/

#define USE_MUX   0

int sendRequest(char *ip,                       // server's ip address  '
        char *request,                  // request you want to send
        char *response,                 // response you will get from the server
        unsigned int len) {             // length of response
    //ssize_t ret;
    int     sockfd = -1;
    static int last_sockfd = 0;
    struct  sockaddr_in  serverAddress;
    int flag;
    int flags;
    time_t  now_time;
    time_t  start_time;
    time_t  end_time;

    struct  timeval tv;
    fd_set  readfds, writefds;

    serverAddress.sin_family      =  AF_INET;
    serverAddress.sin_addr.s_addr =  inet_addr(ip);
    serverAddress.sin_port        =  htons(HTTP_PORT);
#if USE_MUX
    //printf("post_request lock here %p\n", response);
    pthread_mutex_lock (&post_request);
#endif
    time(&now_time);
    start_time = now_time;


    printf("\n\n!!!!!!!!!!!!!!!!last_sockfd is %d~~~~~~~~~~~~~~~~~~~~~\n\n", last_sockfd);
    // connect to socket


    while(1){
            // create a socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            strcpy(response, "socket create error");
#if USE_MUX
            pthread_mutex_unlock (&post_request);
#endif
            return -1;
        }  // end of if
        
        if (last_sockfd == sockfd) {
            //sleep(1);
            //usleep(1000);
            printf("equal please wait here now sock_fd = %d  last fd = %d\n", sockfd, last_sockfd);
            // not close now fd
        } else {
            printf("no equal now sock_fd = %d  last fd = %d\n", sockfd, last_sockfd);
            if (close(last_sockfd) != 0) {
                printf("close sockfd error %d\n", last_sockfd);
            } else {
            
                printf("close last sockfd ok here %d\n", last_sockfd);
            }
            break;
        }

           
    }
    
     last_sockfd = sockfd;


    // non block connect here
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL,flags|O_NONBLOCK);

    if ((connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0) {
        /*
        close(sockfd);
        perror("connect fail (sendRequest())");
        time(&now_time);
        end_time= now_time;
        printf("sec is %d\n", end_time - start_time);
        return -2;
        */
    }  // end of if


    tv.tv_sec  = CONNECT_TIMEOUT   ;
    tv.tv_usec = 0;

    FD_ZERO(&writefds);
    FD_SET(sockfd, &writefds);

    flag = select(sockfd + 1, NULL, &writefds, NULL, &tv);
    time(&now_time);
    end_time= now_time;
    
    
    if (flag == 0) {
        printf("connect time out here\n");
        //perror("connect time out");
        close(sockfd);
        printf("timeout in (senRequest) is  %ld sec\n", end_time - start_time);
        strcpy(response, "connection timeout here");
#if USE_MUX
        pthread_mutex_unlock (&post_request);
#endif
        return -1;
    } else if (flag < 0) {
        printf("select error here\n");
        perror("select error here");
        strcpy(response, "select error here");
        close(sockfd);
#if USE_MUX
        pthread_mutex_unlock (&post_request);
#endif
        return -2;
    }

    if (flag == 1) {
        if (FD_ISSET(sockfd, &writefds)) {
            // send some message 
            if (send(sockfd, request, strlen(request), 0) < 0) {
                close(sockfd);
                perror("send error in (sendRequest)");
                strcpy(response, "send error in (sendRequest)");
                printf("timeout in (senRequest) is  %ld sec\n", end_time - start_time);
#if USE_MUX
        	pthread_mutex_unlock (&post_request);
#endif
                return -3;
            }  // end of if
            // get response from the server

            tv.tv_sec  = CONNECT_TIMEOUT    ;
            tv.tv_usec = 0;

            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);

            flag = select(sockfd + 1, &readfds, NULL, NULL, &tv);
            if (flag == 0 || flag < 0) {
                printf("time out here\n"); 
                strcpy(response, "timeout here!");
                close(sockfd);
#if USE_MUX
        	pthread_mutex_unlock (&post_request);
#endif
                return -1;
            } else {
                if (FD_ISSET(sockfd, &readfds)) {
                    if (recv(sockfd, response, len, 0) == -1) {
                        close(sockfd);
                        perror("recv");
                        strcpy(response, "recv error here");
#if USE_MUX
        		pthread_mutex_unlock (&post_request);
#endif
                        return -4;
                    }  // end of if
                } else {
                    strcpy(response, "other fset here\n");
                }
                close(sockfd);
            }


        } else {
            printf("other writefds here\n");
        }
    }

#if USE_MUX
            //printf("post_request unlock here port:%d\n", serverAddress.sin_port);
            pthread_mutex_unlock (&post_request);
#endif
    
    return 0;
}

