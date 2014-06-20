/*******************************************************************************************
Function: int cartvuStatusCallback(void *, int, char**, char**)
Description: 

Entry: ......
Return: 0
 ********************************************************************************************/

#define LANE_SIZE    50
int deviceStatus[LANE_SIZE];

struct msg {
    int kind;
    int lane;
    int status;
    unsigned long timestamp;
};

struct send_msg {
    struct msg send[LANE_SIZE];	
};

struct send_msg cartvu_msg;
struct send_msg media_msg;

int restart_device(char *type);


int cartvuStatusCallback(void *para, int n_column, char **column_value, char **column_name) {
    int i;

    i = atoi(column_value[2]) - 1;
    if (i >= 0 && i < LANE_SIZE) {
        deviceStatus[i] = atoi(column_value[5]);
    }

    return  0;
}  // end
/*******************************************************************************************
Function: int mediaStatusCallback(void *, int, char**, char**)
Description: 

Entry: ......
Return: 0
 ********************************************************************************************/
int mediaStatusCallback(void *para, int n_column, char **column_value, char **column_name) {
    int i;

    i = atoi(column_value[2]) - 1;
    if (i >= 0 && i < LANE_SIZE) {
        deviceStatus[atoi(column_value[2]) - 1] = atoi(column_value[5]);
    }

    return  0;
}  // end
/****************************************************************************
Function: int sendStatus(int)
Description: send device status to service server

Entry: type       0 - cartvu, 1 - media, 2 - localnode
Return: 0       success
<0      fail
 *****************************************************************************/
int sendStatus(int type) {
    int      i;
    char    paras[512] = {0};

    char *p, *p1, result[10];;
    int len;
    int ret = 0;
    char buf_echo[2048];
    char temp_response[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];
    static int error_count = 0;



    sprintf(paras, "mac=%s&selfIp=%s&", mac, send_info);
    switch (type) {
        case 0:
            strcat(paras, "cc=");
            for (i = 0; i < global_cartvu_lane_size; i++) {
                if (i > 0) strcat(paras, ":");
                sprintf(paras, "%s%d", paras, cartvu_msg.send[i].status);        // Lane no. oct not hex
            }

            break;


        case 1:
            strcat(paras, "mc=");
            for (i = 0; i < global_media_lane_size; i++) {
                if (i > 0) strcat(paras, ":");
                sprintf(paras, "%s%d", paras, media_msg.send[i].status);
            }
            break;

        case 2:                 // LocalNode status
            strcat(paras, "ln=0");// status = 0;
            break;
    }

    sprintf(request,
            "POST /status.php HTTP/1.1\nHOST:%s\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            serverDomain1, strlen(paras), paras);
    //printf("request=%s\n", request);
    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
    if (pthread_mutex_lock (&send_mutex) != 0) {
        sprintf(temp_response, "mutex lock fail %s", __func__);
        printf("%s\n", temp_response);
        recordLog(1, 2, temp_response);
    }
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    if (pthread_mutex_unlock(&send_mutex) != 0) {
        sprintf(temp_response, "mutex unlock fail %s", __func__);
        printf("%s\n", temp_response);
        recordLog(1, 2, temp_response);
    }
    if (ret < 0) {
        printf("the network maybe error\n");

        error_count++;
        if (error_count > 10) {
            recordLog(1, 2, strcat(response, "  the network maybe error(sendStatus)"));
            isLogin = 0;
            error_count = 0;
        }

        return -1;
    }

    //  printf("response is %s\n", response);
    strcpy(buf_echo, response);
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

            if (strncmp(result, "ok", 2) == 0) {
                return 0;
            } else if (strncmp(result, "restart", 7) == 0) {
                printf("you receive restart here\n");
                recordLog(1, 20, "you receive restart here and restart the machine");
                reset();

            } else if (strncmp(result, "localserverrestart", 18) == 0) {
                printf("you receive restart here\n");
                recordLog(1, 20, "you receive restart here and restart the machine");
                reset();
            } else if (strncmp(result, "mediarestart", 12) == 0) {
                restart_device("media");
            } else if (strncmp(result, "cartvurestart", 13) == 0) {
                restart_device("cartvu");
            } else {  
                isLogin = 0;
                printf("the service node send is error(sendStatus)?\n");
                //recordLog(1, 20, "the result is error(sendStatus)");
                recordLog(1, 20, strcat(buf_echo, "  sendStatus error"));
                return -1;
            } 

        }
    }

    return 0;
}

#define RESTART_STATUS   1

#define ENABLE_TELNETD_STATUS    3

#define RESET_DEVICE_STATUS      2

int restart_device(char *type) {
    sqlite3 *conn = NULL;
    char sqlStr[256];
    int ret;
    char now_type[20];
    char buf_echo[1024];

    int count;


    strcpy(now_type, type);
    strlower(now_type);


    ret = sqlite3_open(STATIC_DATABASE, &conn);
    if(ret != 0){
        printf("database connect fail in restart device\n");

        return -1;
    }

    printf("~~~~~~~~restart %s here~~~~~~~~~~~~\n", type);
    // update Mac set Operation=1 where  Kind in ('cartvu')
    if (strncmp(now_type, "cartvu", 6) == 0) {
        sprintf(sqlStr, "update Mac set Operation=%d where  Kind in ('%s')", RESTART_STATUS, now_type);
    } else if (strncmp(now_type, "media", 5) == 0) {
        sprintf(sqlStr, "update Mac set Operation=%d where  Kind in ('%s')", RESTART_STATUS, now_type);
    } else {
        printf("input wrong string here\n");
    }


    count = 0;                     
    while (1) {
        count++;
        ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);       
        if (ret != SQLITE_OK) {
            if (count > 100) {
                sprintf(buf_echo, "fail %d times in  %s", count, sqlStr);
                ret = -1;
                break;
            }
        } else {
            sprintf(buf_echo, "trying %d times ok in %s", count, sqlStr);

            ret = 0;
            break;
        }
    }
    printf("%s\n", buf_echo);
    recordLog(1, 20, buf_echo);
    sqlite3_close(conn);   

    return ret;


}
