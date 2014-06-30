/*******************************************************************************************
Function: int cartvuStatisCallback(void *, int, char**, char**)
Description: callback function that query statistics of cartvu from database
that did not send out

Entry: ......
Return: 0
 ********************************************************************************************/
int cartvuStatisCallback(void *para, int n_column, char **column_value, char **column_name) {
    char paras[1024], result[10], sqlStr[512];
    char *p, *p1;
    int len, ret;
    char temp_response[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];

    //avoid the watchdog error if it has many info to send
    dogFood = WATCHDOG_COUNT;
    //printf("set dogFood here\n");
    // fields:StatisDate Carts Alarm Confirmed
    sprintf(paras, "mac=%s&type=cartvu&date=%s&lane=%s&carts=%s&alarm=%s&confirm=%s",
            mac,
            column_value[0], column_value[1], column_value[2], column_value[3], column_value[4]);
    sprintf(request,
            "POST /statis.php HTTP/1.1\nHOST:%s\nConnection: close\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            serverDomain1, strlen(paras), paras);
    //printf("request=%s\n", request);
    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
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

    
    if (ret < 0) {
	printf("the network maybe error\n");
	recordLog(1, 2, "the network maybe error(cartvuStatisCallback())");
	isLogin = 0;
	return -1;
    }
    //printf("response=%s\n", response);
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
            strncpy(result, p, 2);       // ok   whatever 
            result[2] =0;
            strlower(result);
            trim(result);
            if (strcmp(result, "ok") == 0) {
                // update the field "flag" of database table CartvuStatis
                // it means local server send a statistics record to service server and service service gets it.
                sprintf(sqlStr, "update CartvuStatis set Flag=1 where StatisDate='%s' and Lane=%s\n",
                        column_value[0], column_value[1]);
                printf("sql = %s", sqlStr);
                ret = fputs(sqlStr, tmpFp);
                if (ret == EOF) {
                    printf("fail - Write fail to fial sql.txt.");
                }
            } else {
                // Login to remote server again if receiving some other message
                //isLogin = 0;
                printf("%s send and receive fail\n", paras);
                recordLog(1, 11, strcat(paras, " send and receive fail"));
            }
        }
    }
    return  0;
}  // end
/*******************************************************************************************
Function: int mediaStatisCallback(void *, int, char**, char**)
Description: callback function that query statistics of media from database
that did not send out

Entry: ......
Return: 0
 ********************************************************************************************/
int mediaStatisCallback(void *para, int n_column, char **column_value, char **column_name) {
    char paras[1024], result[10], sqlStr[512];
    char *p, *p1;
    int len, ret;
    char temp_response[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];

    
    dogFood = WATCHDOG_COUNT;
    //printf("set dogFood here\n");
    
    // fields:StatisDate Carts Alarm Confirmed
    sprintf(paras, "mac=%s&type=media&date=%s&adname=%s&count=%s",
    	    mac,
            column_value[0], column_value[1], column_value[2]);
    sprintf(request,
            "POST /statis.php HTTP/1.1\nHOST:%s\nConnection: close\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            serverDomain1, strlen(paras), paras);
    //printf("request=%s\n", request);
    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
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

    
    if (ret < 0) {
	printf("the network maybe error\n");
	recordLog(1, 2, "the network maybe error(mediaStatisCallback())");
	isLogin = 0;
	return -1;
    }
    //printf("response=%s\n", response);
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
            strncpy(result, p, 2);       // ok   whatever 
            result[2] = 0;
            strlower(result);
            trim(result);
            if (strcmp(result, "ok") == 0) {
                // update the field "flag" of database table
                sprintf(sqlStr, "update MediaStatis set flag=1 where StatisDate='%s' and AdName='%s'\n",
                        column_value[0], column_value[1]);
                printf("callback sql = %s", sqlStr);
                ret = fputs(sqlStr, tmpFp);
                if (ret == EOF) {
                    printf("fail - Write fail to fial sql.txt.");
                }
            } else {
                // Login to remote server again
                //isLogin = 0;
                printf("%s send and receive fail\n", paras);
                recordLog(1, 11, strcat(paras, " send and receive fail"));
            }
        }
    }
    return  0;
}  // end
/****************************************************************************
Function: int sendStatistics(int)
Description: send statistics information of device to service server

Entry: type       0 - cartvu, 1 - media
Return: 0       success
<0      fail
 *****************************************************************************/
int sendStatistics(int type) {
    sqlite3 *conn  = NULL;          // sqlite db handler
    char    sqlStr[256];            // sql statement
    int     ret;
    //sqlite3_stmt* stmt = NULL;
   // int len;
    //char *p;
    int count = 0;
    char buf_echo[1024];

    // get the statistics that did not send out
    ret = sqlite3_open(DYNAMIC_DATABASE, &conn);                             // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(sendStatistics) - %s\n", DYNAMIC_DATABASE);
        recordLog(1, 16, "Openning database fail(sendStatistics) fail!");
        return -1;
    }
    tmpFp = fopen("/AdNode/proc/sql.txt", "w");
    if (tmpFp == NULL) {
        printf("fail - Openning temp file fail(sendStatistics).\n");
        recordLog(1, 17, "Openning temp file fail(sendStatistics)  fail!");
        return -2;
    }
    switch (type) {
        case 0:              // CARTVU
            strcpy(sqlStr, "select * from CartvuStatis where Flag=0");             // SQL语句
            ret = sqlite3_exec(conn, sqlStr, cartvuStatisCallback, NULL, NULL);    // 执行
            if (ret != SQLITE_OK) {
                printf("fail : Execute fail(sendStatistics) - %s\n", sqlStr);
                recordLog(1, 34, strcat(sqlStr, "fail in sendStatistics"));
                sqlite3_close(conn);
                return -3;
            }
            break;
        case 1:              // MEDIA
            strcpy(sqlStr, "select * from MediaStatis where Flag=0");              // SQL语句
            ret = sqlite3_exec(conn, sqlStr, mediaStatisCallback, NULL, NULL);     // 执行
            if (ret != SQLITE_OK) {
                printf("fail : Execute fail(sendStatistics) - %s\n", sqlStr);
                recordLog(1, 34, strcat(sqlStr, "fail in sendStatistics"));
                sqlite3_close(conn);
                return -3;
            }
            break;
    }

    ret = sqlite3_close(conn);                                                     // 关闭数据库
    if (ret != SQLITE_OK) {
        printf("fail : Closing database fail(sendStatistics) - %s\n", DYNAMIC_DATABASE);
        recordLog(1, 16, "Closing database fail(sendStatistics) fail!");
        return -1;
    }

    fclose(tmpFp);
    tmpFp = fopen("/AdNode/proc/sql.txt", "r");
    if (tmpFp != NULL) {
        while (fgets(sqlStr, 256, tmpFp)) {


            ret = sqlite3_open(DYNAMIC_DATABASE, &conn);                             // 打开数据库
            if (ret != SQLITE_OK) {
                printf("fail : Openning database fail(sendStatistics) - %s\n", DYNAMIC_DATABASE);
                recordLog(1, 16, "Openning database fail(sendStatistics) fail!");
                fclose(tmpFp);
                unlink("/AdNode/proc/sql.txt");
                return -1;
            }


            //printf("sql st = %s", sqlStr);

            sqlStr[strlen(sqlStr) - 1] = '\0';
#if 0
            

            len = strlen(sqlStr);
            if (sqlite3_prepare_v2(conn,sqlStr,len,&stmt,NULL) != SQLITE_OK) {
                perror("sqlite3_prepare_v2 database fail(sendStatistics) !");
                if (stmt) {
                    sqlite3_finalize(stmt);
                }
                fclose(tmpFp);
                unlink("/AdNode/proc/sql.txt");
                sqlite3_close(conn); 
                
                recordLog(1, 16, "sqlite3_prepare_v2 database fail(sendStatistics) !");
                return -1;
            }

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                perror("sqlite3_step database fail(sendStatistics)");
                sqlite3_finalize(stmt);
                fclose(tmpFp);
                unlink("/AdNode/proc/sql.txt");
                sqlite3_close(conn); 
                
                recordLog(1, 16, "sqlite3_step database fail(sendStatistics) !");
                return -1;

            }


            sqlite3_finalize(stmt);

#else
	   count = 0;
	   while (1) {
           	ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);
           	if (ret != SQLITE_OK) {
          	    count++;
          	    if (count >= TRY_COUNT) {
	          	    printf("fail : Execute fail(sendStatistics) - %s\n", sqlStr);
	          	    sprintf(buf_echo, "Execute %d times fail:%s", count, sqlStr);
	           	    recordLog(1, 34, buf_echo);
	           	    break;
           	    }
           	    
            	} else {
	            if (count >= 1) {
		            sprintf(buf_echo, "trying %d times ok:%s", count, sqlStr);
		            recordLog(1, 34, buf_echo);
		            printf("%s\n", buf_echo);
	            }
	            break;
               }
            	
            }
#endif

            ret = sqlite3_close(conn);                                                     // 关闭数据库
            if (ret != SQLITE_OK) {
                printf("fail : Closing database fail(sendStatistics) - %s\n", DYNAMIC_DATABASE);
                recordLog(1, 16, "Closing database fail(sendStatistics) fail!");
                return -1;
            }

        }
        fclose(tmpFp);
        unlink("/AdNode/proc/sql.txt");
    }  else {
        printf("the result of sql.txt is null\n");
    }

    return 0;
}

