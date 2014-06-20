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

#include   "strseparate.h"
#include   "const.h"
#include    "recordLog.h"
#define HTTP_PORT          80

#include "sendRequest.inc"

int ftpFlag = 0;
int ftpDogFood = 0;
char serverDomain1[100] = "server1.cartvu-media.com";
char ip[100] = "203.186.75.165";

char mac[20] = "000011700009";
int cashier = 123456;

pthread_mutex_t  send_mutex;


int callBackFlag = 0;
int   isLogin = 0;



struct receive_info {
    char cashier[50];
    int not_confirmed;
    char info_text[INFO_TEXT_SIZE];
    char now_stamp[INFO_TEXT_SIZE];
    int lane;
    int flag;
};

struct receive_info rec_info;

#define PARA_SIZE   5






int sendInfoCallback(void *para, int n_column, char **column_value, char **column_name) {
    callBackFlag = 1;
    return  0;
}  // end

int update_sqlite(char *database, char *table, struct receive_info *info) {
    int ret;
    sqlite3 *conn  = NULL;
    int error_count;
    time_t timep;
    struct tm *p;
    char now_time[50] = {0};

    char sqlStr[BUF_RECEIVE_SIZE];

    ret = sqlite3_open(database, &conn);                             // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(update_sqlite) - %s\n", database);

        recordLog(1, 16, "Openning database fail(update_sqlite) fail!");
        return -1;
    }

    sprintf(sqlStr, "select * from %s where Cashier='%s' and Lane=%d", table, info->cashier, info->lane);             
    callBackFlag = 0;
    error_count = 0;
    while(1) {
        ret = sqlite3_exec(conn, sqlStr, sendInfoCallback, NULL, NULL);    // execute
        if (ret != SQLITE_OK) {
            error_count++;
            printf("fail : Execute fail(update_sqlite() - %s\n", sqlStr);
            if (error_count > TRY_COUNT) {
                recordLog(1, 34, strcat(sqlStr, " Execute (update_sqlite()) fail!!"));
                sqlite3_close(conn); 
                return -1;
            }

        } else {
            break;
        }
    }
    
    timep = time((time_t *)NULL);
    p = localtime(&timep);
    sprintf(now_time, "%d-%02d-%02d %02d:%02d:%02d",
                    1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
                           p->tm_hour, p->tm_min, p->tm_sec
                           );


    // 有此条记录时更新此条记录
    if (callBackFlag == 1) {
        
        
        sprintf(sqlStr, "update %s set NotConfirmed=%d,Flag=%d,TimeStamp='%s', Info='%s' where Cashier='%s' and Lane=%d", table, 
                info->not_confirmed, info->flag, 
                now_time,
                info->info_text,info->cashier, info->lane);  

        error_count = 0;
        while(1) {
            ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);    // execute
            if (ret != SQLITE_OK) {
                error_count++;
                
                if (error_count > TRY_COUNT) {
                    printf("fail : Execute fail(update_sqlite(in update) - %s\n", sqlStr);
                    recordLog(1, 34, strcat(sqlStr, " Execute (update_sqlite(in update)) fail!!"));
                    sqlite3_close(conn); 
                    return -1;
                }

            } else {
                break;
            }
        }

        
    } else {// 创建记录
        sprintf(sqlStr, "insert into %s values('%s',%d,%d,%d,'%s','%s','%s')",
                table, info->cashier, info->not_confirmed, info->lane, info->flag, 
                now_time, now_time,
                info->info_text);
        error_count = 0;
        while(1) {
            ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);    // execute
            if (ret != SQLITE_OK) {
                error_count++;
                
                if (error_count > TRY_COUNT) {
                    printf("fail : Execute fail(update_sqlite(in update) - %s\n", sqlStr);
                    recordLog(1, 34, strcat(sqlStr, " Execute (update_sqlite(in update)) fail!!"));
                    sqlite3_close(conn); 
                    return -1;
                }

            } else {
                break;
            }
        }
    }

    sqlite3_close(conn); 
    printf("%s here\n", sqlStr);

    return 0;
}





int query_info(struct receive_info  *rec_info) {
    char request[BUF_RECEIVE_SIZE];
    char response[BUF_RECEIVE_SIZE]; 
    char temp_response[BUF_RECEIVE_SIZE]; 
    char paras[256];
    int ret;
    char *p;
    char *p1;
    char result[BUF_RECEIVE_SIZE] = {0};
    int len;
    paraStruc paraS[PARA_SIZE];
    int i;
    int need_create_flag = 0;

    sprintf(paras, "mac=%s&", mac);
    printf("============================\n");


    // mac=000011700009&request=query

    sprintf(paras, "%s%s=%s", paras, "request", "query");
    sprintf(request,
            "POST %s HTTP/1.1\nHOST:%s\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            SEND_INFO_PHP, serverDomain1, strlen(paras), paras);
    printf("request     :%s\n\n", paras);



    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
    pthread_mutex_lock (&send_mutex);
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    pthread_mutex_unlock(&send_mutex);

    if (ret < 0) {
        printf("the network maybe error\n");
        recordLog(1, 2, strcat(response, "  the network maybe error(sendStatus)"));
        isLogin = 0;
        return -1;
    }



    //cashier=12345&noconfirmed=10&text=you should confirmed the button sended by manager
    p = strstr(response, "\r\n\r\n");
    if (p != NULL) {
        p += 4;
        //printf("p:%s", p);
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

        }
    }


    rec_info->lane = -1;
    if (result[0] != '\0') {
        //printf("info here\n");
        printf("result     :%s\n\n", result);
        stringSeparate(result, paraS, PARA_SIZE);
        for (i = 0; i < PARA_SIZE; i++) {
            if (paraS[i].key=='\0') break; 
            printf("%s = %s\n", paraS[i].key, paraS[i].value);
            if (strcmp(paraS[i].key, "cashier") == 0) {
                strcpy(rec_info->cashier, paraS[i].value);
                need_create_flag = 1;
                rec_info->flag = CREATE_STATUS;//创建的初始状态

            } else if (strcmp(paraS[i].key, "notconfirmed") == 0) {
                rec_info->not_confirmed = atoi(paraS[i].value);
            } else if (strcmp(paraS[i].key, "lane") == 0) {
                rec_info->lane= atoi(paraS[i].value);
            } else if (strcmp(paraS[i].key, "text") == 0) {
                strcpy(rec_info->info_text, paraS[i].value);
            } else if (strcmp(paraS[i].key, "none") == 0) {
                printf("no info the execute\n");
                break;
            } else {
                printf("Receive error  :%s\n", temp_response);
                return -1;
            }
        }

    }

    if (need_create_flag == 1) {
        printf("update sqlite here\n");
        ret = update_sqlite(SEND_INFO_DATABASE, SEND_INFO_TABLE, rec_info);
        if (ret == 0) {
            return 1;
        }
    }
    return 0;

}



int request_ok(struct receive_info  *rec_info) {
    char request[BUF_RECEIVE_SIZE];
    char response[BUF_RECEIVE_SIZE]; 
    char temp_response[BUF_RECEIVE_SIZE]; 
    char paras[256];
    int ret;
    char *p;
    char *p1;
    char result[BUF_RECEIVE_SIZE] = {0};
    int len;
    paraStruc paraS[PARA_SIZE];

    sprintf(paras, "mac=%s&", mac);

    printf("============================\n");

    //mac=000011700009&request=query
    //mac=xxxxxxx&request=received&cashier=12345

    sprintf(paras, "%s%s=%s&cashier=%s", paras, "request", "received", rec_info->cashier);
    sprintf(request,
            "POST %s HTTP/1.1\nHOST:%s\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            SEND_INFO_PHP, serverDomain1, strlen(paras), paras);
    printf("request     :%s\n\n", paras);



    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
    pthread_mutex_lock (&send_mutex);
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    pthread_mutex_unlock(&send_mutex);

    if (ret < 0) {
        printf("the network maybe error\n");
        recordLog(1, 2, strcat(response, "  the network maybe error(sendStatus)"));
        isLogin = 0;
        return -1;
    }


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
            //printf("result = %s --\n", result);

        }
    }

    if (result[0] != '\0') {
        printf("result     :%s\n\n", result);
        stringSeparate(result, paraS, PARA_SIZE);
        if (strcmp(result, "received ok") == 0) {
            printf("serviceNode has received modify ok here\n");
            //update sqlite here
            rec_info->flag = KNOWN_CREATE;
            ret = update_sqlite(SEND_INFO_DATABASE, SEND_INFO_TABLE, rec_info);
            if (ret == 0) {
                return 1;
            }
            
        } else {
            printf("serviceNode not receive\n");
        }
    }


    return 0;

}

int compare_stamp(struct receive_info  *rec_info) {
    time_t timep;
    time_t before;
    struct tm tm;
    

    char *q;

    


#if 0
    char now_time[50] = {0};

    // 2014-03-31 04:19:35
    sprintf(now_time, "%d-%02d-%02d %02d:%02d:%02d",
                    1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
                           p->tm_hour, p->tm_min, p->tm_sec
                           );  
#endif
    
    q = rec_info->now_stamp;
    printf("timeStamp is %s\n", q);
    
    q[4] = '\0';
    tm.tm_year = atoi(q) - 1900;
    //printf("year:%d\n",tm.tm_year);
    
    q += 5;
    q[2] = '\0';
    tm.tm_mon = atoi(q) - 1;
    //printf("mon:%d\n",tm.tm_mon);

    q += 3;
    q[2] = '\0';
    tm.tm_mday = atoi(q);
    //printf("day:%d\n",tm.tm_mday);


    q += 3;
    q[2] = '\0';
    tm.tm_hour = atoi(q);
    //printf("shi:%d\n",tm.tm_hour);



    q += 3;
    q[2] = '\0';
    tm.tm_min = atoi(q);
    //printf("fen:%d\n",tm.tm_min);
    
    q += 3;
    q[2] = '\0';
    tm.tm_sec = atoi(q);
    //printf("miao:%d\n",tm.tm_sec);   

    before = mktime(&tm);

    timep = time((time_t *)NULL);
    //printf("before is %ld  now is %ld\n", before, timep);
    printf("difference :%ld\n", timep - before);
    if (timep - before > TIME_OUT_SEC) {
        return 1;
    } else {
        return 0;
    }
}

int confirmed_ok(struct receive_info  *rec_info) {
    char request[BUF_RECEIVE_SIZE];
    char response[BUF_RECEIVE_SIZE]; 
    char temp_response[BUF_RECEIVE_SIZE]; 
    char paras[256];
    int ret;
    char *p;
    char *p1;
    char result[BUF_RECEIVE_SIZE] = {0};
    int len;

    sprintf(paras, "mac=%s&", mac);

    printf("============================\n");
    //mac=000011700009&request=query
    //mac=xxxxxxx&request=received&cashier=12345
    //mac=xxxxxxx&request=confirmed&cashier=12345

    sprintf(paras, "%s%s=%s&cashier=%s", paras, "request", "confirmed", rec_info->cashier);
    sprintf(request,
            "POST %s HTTP/1.1\nHOST:%s\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            SEND_INFO_PHP, serverDomain1, strlen(paras), paras);
    printf("request     :%s\n\n", paras);



    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
    pthread_mutex_lock (&send_mutex);
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    pthread_mutex_unlock(&send_mutex);

    if (ret < 0) {
        printf("the network maybe error\n");
        recordLog(1, 2, strcat(response, "  the network maybe error(sendStatus)"));
        isLogin = 0;
        return -1;
    }


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
            //printf("result = %s --\n", result);

        }
    }

    if (result[0] != '\0') {
        printf("result     :%s\n\n", result);
        if (strcmp(result, "confirmed ok") == 0) {
            printf("serviceNode has receive confirmed ok here\n");
            rec_info->flag = SERVICENODE_RECEIVE;
            ret = update_sqlite(SEND_INFO_DATABASE, SEND_INFO_TABLE, rec_info);
            if (ret == 0) {
                return 1;
            }
        }
    }


    return 0;

}

int test_send(int cashier) {
    char request[BUF_RECEIVE_SIZE];
    char response[BUF_RECEIVE_SIZE]; 
    char temp_response[BUF_RECEIVE_SIZE]; 
    char paras[256];
    int ret;
    //char result[BUF_RECEIVE_SIZE] = {0};
    //int len;
    //paraStruc paraS[PARA_SIZE];

    sprintf(paras, "mac=%s&", mac);

    printf("============================\n");
    //mac=000011700009&request=query
    //mac=xxxxxxx&request=received&cashier=12345
    //mac=xxxxxxx&request=confirmed&cashier=12345

    sprintf(paras, "%s%s", paras, "lane=09&imagefile=I-11-20140320163815.jpg&alarmtime=20140317183255&confirm=0&cashier=1111&barcode=aaa");
    sprintf(request,
            "POST %s HTTP/1.1\nHOST:%s\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            "/alarm.php", serverDomain1, strlen(paras), paras);
    printf("request     :%s\n\n", paras);



    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
    pthread_mutex_lock (&send_mutex);
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    pthread_mutex_unlock(&send_mutex);

    if (ret < 0) {
        printf("the network maybe error\n");
        recordLog(1, 2, strcat(response, "  the network maybe error(test_send)"));
        isLogin = 0;
        return -1;
    }



        printf("result     :%s\n\n", response);


    return 0;

}

int getInfoCallback(void *para, int n_column, char **column_value, char **column_name) {

    
    strcpy(rec_info.cashier, column_value[0]);
    rec_info.not_confirmed = atoi(column_value[1]);
    rec_info.lane = atoi(column_value[2]);
    rec_info.flag = atoi(column_value[3]);
    strcpy(rec_info.info_text, column_value[6]);
    strcpy(rec_info.now_stamp, column_value[5]);
    return  0;
}  // end


int query_sqlite(struct receive_info  *info, char *database, char *table, int flag) {
    int ret;
    sqlite3 *conn  = NULL;
    int error_count;

    char sqlStr[BUF_RECEIVE_SIZE];

    ret = sqlite3_open(database, &conn);                             // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(query_sqlite) - %s\n", database);

        recordLog(1, 16, "Openning database fail(query_sqlite) fail!");
        return -1;
    }

    sprintf(sqlStr, "select * from %s where Flag=%d", table, flag);             
    error_count = 0;
    while(1) {
        ret = sqlite3_exec(conn, sqlStr, getInfoCallback, NULL, NULL);    // execute
        if (ret != SQLITE_OK) {
            error_count++;
            printf("fail : Execute fail(query_sqlite() - %s\n", sqlStr);
            if (error_count > TRY_COUNT) {
                recordLog(1, 34, strcat(sqlStr, " Execute (query_sqlite()) fail!!"));
                sqlite3_close(conn); 
                return -1;
            }

        } else {
            break;
        }
    }
    sqlite3_close(conn); 

    return 0;
}




int main(void) {
    int ret;
    int set_flag = 0;

    strcpy(filename, "/now_record");
    printf("~~~~~~~start here~~~~~~~\n");

    ret = pthread_mutex_init(&send_mutex, NULL); //init mutex 初始化互斥锁
    if (ret != 0)
    {
        perror("send_mutex  initialization failed");
        return -1;
    } else {
        ;//printf("send_mutex initialization ok here\n");
    }


#if 1
while(1) {
    //发起查询请求
    ret = query_info(&rec_info);
    if (ret == 1) {
        // flag = 1;//创建记录ok
        printf("set the flag = 1\n");
        request_ok(&rec_info);
    }
    // 查询数据库的当前标志 付给flag
    set_flag = CASHIER_CONFIRMED;
    bzero(&rec_info, sizeof(rec_info));
    query_sqlite(&rec_info, SEND_INFO_DATABASE, SEND_INFO_TABLE, set_flag);
    if (rec_info.flag == set_flag) {
        printf("set the flag = 5\n");
        confirmed_ok(&rec_info);
        
    } 


    // 当标志为4或者2时重新更新为 1
#if 1
    set_flag = CASHIER_TIMEOUT;
    bzero(&rec_info, sizeof(rec_info));
    query_sqlite(&rec_info, SEND_INFO_DATABASE, SEND_INFO_TABLE, set_flag);
    if (rec_info.flag == set_flag) {

            rec_info.flag = 1;
            ret = update_sqlite(SEND_INFO_DATABASE, SEND_INFO_TABLE, &rec_info);
            if (ret == 0) {
                printf("update the flag from 4 to 1 is ok\n");
            }
        
    } 

    set_flag = SCREEN_SHOW;
    bzero(&rec_info, sizeof(rec_info));
    query_sqlite(&rec_info, SEND_INFO_DATABASE, SEND_INFO_TABLE, set_flag);
    if (rec_info.flag == set_flag) {
            if (compare_stamp(&rec_info) == 1) {
                printf("need to update the flag from 2 to 1\n");
                rec_info.flag = 1;
                ret = update_sqlite(SEND_INFO_DATABASE, SEND_INFO_TABLE, &rec_info);
                if (ret == 0) {
                    printf("update the flag from 4 to 1 is ok\n");
                }
            }
    } 
#else
    // 直接更新标识为4或2的变为1
    // update SendInfo set flag=1 where flag=4 or flag=2;
    
#endif
    sleep(30);
}
#endif
    //test_send(123);

    return 0;
}
