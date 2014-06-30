



#define POST_AD_OK_START      1

#define POST_AD_OK_END      2




int sendRequest(char *ip,                       // server's ip address  '
        char *request,                  // request you want to send
        char *response,                 // response you will get from the server
        unsigned int len);


int analyse_string(char *response) {
    char *p;
    char *p1;
    int len;

    char result[1024];



    p = strstr(response, "\r\n\r\n");
    if (p != NULL) {
        p += 4;

        p1 = p;                            // length of response
        p = strstr(p, "\r\n");
        if (p != NULL) {
            *p = 0;
            p += 2;

            len = (int)strtol(p1, NULL, 16);;
            p[len] = 0;
            //strncpy(result, p, 2);       // ok   whatever 
            strcpy(result, p);  
            //result[2] = 0;
            strlower(result);
            trim(result);
            printf("result = %s --\n", result);
            if (strstr(result, "ok") != NULL) {
                return 0;
            }
            /*
            if (strncmp(result, "ok", 2) == 0) {
                return 0;
            } 
            */

        }
    }

    return -1;

}


int post_ad_num(int num, int flag) {
    char paras[200];

    int ret;

    char request[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];
    char temp_response[MAXLEN_RESPONSE];

    sprintf(paras, "mac=%s&ad=%d&flag=%d", mac, num, flag);
    //printf("paras is !%s!\n", paras);



    sprintf(request,
            "POST %s HTTP/1.1\r\nHOST:%s\r\nConnection: close\nContent-Type:application/x-www-form-urlencoded\r\nContent-Length:%d\r\n\r\n%s\r\n\r\n",
            POST_AD_PHP, serverDomain1, strlen(paras), paras);
    //printf("request     :%s\n\n", request);



    bzero(response, sizeof(response));
    if (pthread_mutex_lock (&send_mutex) != 0) {
        sprintf(temp_response, "mutex lock fail %s", __func__);
        printf("%s\n", temp_response);
        recordLog(1, 2, temp_response);
    } else {
        printf("lock in %s \n", __func__);
    }
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    if (pthread_mutex_unlock(&send_mutex) != 0) {
        sprintf(temp_response, "mutex unlock fail %s", __func__);
        printf("%s\n", temp_response);
        recordLog(1, 2, temp_response);
    } else {
        printf("unlock in %s\n", __func__);
    }


    printf("start:\n%sstop\n", response);

    return analyse_string(response);

}

void post_ad_ok(char *num, int flag) {

    FILE *fp;

    char file_path[50];
    char file_path2[50];
    paraStruc post_info;


    char tmp[TMP_SIZE];
    int ret;

    char request[MAXLEN_RESPONSE];

    int l_num;


    sprintf(file_path2, "%s%s", MEDIA_COMMAND_PATH, POST_AD_FAIL2);


    sprintf(request, "rm %s -f", file_path2);
    system(request);


    sprintf(file_path, "%s%s", MEDIA_COMMAND_PATH, POST_AD_FAIL);
    fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("open file fail or no post fail here no need to post\n");
    } else {


        // postFail=1
        // postFail=2

        while (fgets(tmp, TMP_SIZE, fp)) {
            //printf("%s\n", tmp);
            stringSeparate(tmp, &post_info, 1);
            //printf("%s is %s\n", post_info.key, post_info.value);
            l_num = atoi(post_info.value);
            ret = post_ad_num(l_num, POST_AD_OK_END);
            if (ret == 0) {
                printf("post ad=%d flag=%d is ok\n", l_num, POST_AD_OK_END);
            } else {
                printf("post ad=%d flag=%d is fail\n", l_num, POST_AD_OK_END);
                sprintf(request, "echo 'postFail=%d' >> %s", l_num, file_path2);
                system(request);
            }
            sleep(1);

        }
        fclose(fp);



    }



    l_num = atoi(num);

    if (l_num == -1) {
        printf("equal to -1 not update\n");
        if (access(file_path2, F_OK) == 0) { 
            sprintf(request, "mv %s  %s", file_path2, file_path);
            system(request);
        } else {
            sprintf(request, "rm %s -f", file_path);
            printf("%s\n", request);
            system(request);
        
            printf("no %s here\n", file_path2);
        }

        return ;
    }

    ret = post_ad_num(l_num, flag);
    if (ret == 0) {
        printf("post ad=%d flag=%d is ok\n", l_num, flag);
    } else {
        printf("post ad=%d flag=%d is fail\n", l_num, flag);
        if (flag == POST_AD_OK_END) {
            sprintf(request, "echo 'postFail=%d' >> %s", l_num, file_path2);
            system(request);
        }
    }

    if (access(file_path2, F_OK) == 0) { 
        sprintf(request, "mv %s  %s", file_path2, file_path);
        system(request);
    } else {
        sprintf(request, "rm %s -f", file_path);
        printf("%s\n", request);
        system(request);
        
        printf("no %s here\n", file_path2);
    }

}

