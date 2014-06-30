/*******************************************************************************************
Function: int imagesCallback(void *, int, char**, char**)
Description: callback function that query records that did not transfer image to server

Entry: ......
Return: 0
 ********************************************************************************************/
int imagesCallback(void *para, int n_column, char **column_value, char **column_name) {
    struct alert_image_more  *tmp;


    tmp = (struct alert_image_more  *)malloc(sizeof(struct alert_image_more));
    if (tmp == NULL) {
        perror("malloc tmp fail");
        return 0;
    }
    bzero(tmp, sizeof(struct alert_image_more));
    strcpy(tmp->alert_image.file_name, column_value[0]);
    strcpy(tmp->alert_image.alert_time, column_value[1]);
    tmp->alert_image.confirmed = atoi(column_value[2]);
    strcpy(tmp->alert_image.cashier, column_value[3]);
    strcpy(tmp->alert_image.barcode, column_value[4]);
    tmp->alert_image.flag = atoi(column_value[5]);
    tmp->alert_image.state = atoi(column_value[6]);

    tmp->next = alert_head->next;
    tmp->front = alert_head;
    alert_head->next->front = tmp;
    alert_head->next = tmp;

    return  0;
}  // end
/*******************************************************************************************
Function: int imagesInfoCallback(void *, int, char**, char**)
Description: callback function that query records that has transfer image to server but
has not send the infomation out

Entry: ......
Return: 0
 ********************************************************************************************/
int imagesInfoCallback(void *para, int n_column, char **column_value, char **column_name) {

    return  0;
}  // end
/****************************************************************************
Function: int sendAlarmImages()
Description: send alarm images and information to service server

Entry: None
Return: 0       success
<0      fail
 *****************************************************************************/
int sendAlarmImages() {
    sqlite3 *conn  = NULL;          // sqlite db handler
    char    sqlStr[256];            // sql statement
    int     ret;
    struct alert_image_more  *now_tmp;
    struct alert_image_more  *free_tmp;
    char lane[4];
    char result[10];
    char sysCmd[1024];
    FILE *fp;
    int error_flag = 0;
    char request[1024];
    char buf_echo[2048];
    char temp_response[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];


    char paras[1024];
    int len;
    char *p;
    char *p1;
    static int error_count = 0;

//    int retValue = 0;

    ret = sqlite3_open(DYNAMIC_DATABASE, &conn);                             // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(sendAlarmImages) - %s", STATIC_DATABASE);
        
        recordLog(1, 16, "Openning database fail(sendAlarmImages) fail!");
        return -1;
    }
    // get records that did not transfer image to server. Transfer the image and send out the infomation.
    // update the database set flag=2



    alert_head->next = alert_head->front = alert_head;

    strcpy(sqlStr, "select * from Images where Flag < 2");             // SQL statement,search records that did not transfer image to server
    ret = sqlite3_exec(conn, sqlStr, imagesCallback, NULL, NULL);    // execute
    if (ret != SQLITE_OK) {
	error_count++;
        printf("fail : Execute fail(sendAlarmImages) - %s\n", sqlStr);
        if (error_count > 100) {
        	error_count = 0;
        	recordLog(1, 34, strcat(sqlStr, " Execute (sendAlarmImages()) fail!!"));
        }
        sqlite3_close(conn); 
        return -1;
    }

    sqlite3_close(conn); 

    now_tmp = alert_head->next;
    while (now_tmp != alert_head) {
/*
        printf("%s\t%s\t%d\t%s\t%s\t%d\t%d\n", now_tmp->alert_image.file_name, now_tmp->alert_image.alert_time, 
                now_tmp->alert_image.confirmed, now_tmp->alert_image.cashier, now_tmp->alert_image.barcode,
                now_tmp->alert_image.flag, now_tmp->alert_image.state);
*/
	if (isLogin == 1) {

	        if (now_tmp->alert_image.flag == 0) {
	            sprintf(sysCmd, "chmod 777 %s%s", CARTVU_IMAGE_PATH, now_tmp->alert_image.file_name); 
	            printf("%s\n", sysCmd);
	            system(sysCmd); 
	            /* 
	            sprintf(sysCmd, "ftpput -u %s -p %s %s %sImages/%s/%s %s%s 2>%s",               
	                    ftpUser, ftpPassword, ip, REMOTE_FTP_CARTVU_PATH, m1m2m3, now_tmp->alert_image.file_name,              
	                    CARTVU_IMAGE_PATH, now_tmp->alert_image.file_name, TMP_ERROR); 
	                    */
	            sprintf(sysCmd, "ftpput -u %s -p %s %s  %s%s %sImages/%s/%s 2>%s",               
	                    ftpUser, ftpPassword, ip, CARTVU_IMAGE_PATH, now_tmp->alert_image.file_name,
 	                    REMOTE_FTP_CARTVU_PATH, m1m2m3, now_tmp->alert_image.file_name, TMP_ERROR); 
 	                   
	            printf("sysCmd = %s\n", sysCmd); 
	            ftpFlag = 1;
	            ftpDogFood = WATCHDOG_COUNT/2;// because the size of image is very small
	            system(sysCmd);
	            ftpFlag = 0;

	            

	            fp = fopen(TMP_ERROR, "r");
	            if (fp != NULL) {
	                error_flag = 0;
	                while (fgets(sqlStr, 256, fp)) {
	                    if(strlen(sqlStr) == 0) {
	                        break;
	                    }
	                    printf("error:%s\n", sqlStr);
	                    recordLog(1, 12, sqlStr);
	                    error_flag = 1;
	                }
	                fclose(fp);
	            }
	            if (error_flag == 0) {
	                now_tmp->alert_image.flag = 1;
	                ret = write_into_sqlite(now_tmp->alert_image);
	                if (ret < 0) {
	                    printf("write to sqlite flag = 1 fail\n");

	                }
	            } else {
			 recordLog(1, 33,strcat(sysCmd, " fail!!!(sendAlarmImages())"));
	            }

	        }


	        if (now_tmp->alert_image.flag == 1) {
	            strncpy(lane, &(now_tmp->alert_image.file_name[2]), 2);         // Lane no.
	            lane[2] = '\0';
	            sprintf(paras, "mac=%s&lane=%d&imagefile=%s&alarmtime=%s&confirm=%d&cashier=%s&barcode=%s",
	            	    mac,
	                    atoi(lane), 
	                    now_tmp->alert_image.file_name,  now_tmp->alert_image.alert_time, 
	                    now_tmp->alert_image.confirmed, now_tmp->alert_image.cashier, now_tmp->alert_image.barcode);
	            bzero(request, sizeof(request));
	            sprintf(request,
	                    "POST /alarm.php HTTP/1.1\nHOST:%s\nConnection: close\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
	                    serverDomain1, strlen(paras), paras);
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
		      recordLog(1, 2, "the network maybe error(sendAlarmImages())");
		      isLogin = 0;
		    } else {
			    strcpy(buf_echo, response);
		            p = strstr(response, "\r\n\r\n");
		            if (p == NULL) {
		                printf("p == NULL in sendAlarmImages(1) - %s\n", response);
		                recordLog(1, 12,strcat(buf_echo, " p == NULL in sendAlarmImages(1)"));
		            } else {
			            p += 4;

			            p1 = p;                            // length of response
			            p = strstr(p, "\r\n");
			            if (p == NULL) {
			                printf("p == NULL in sendAlarmImages(2) - %s\n", response);
			                recordLog(1, 12,strcat(buf_echo, " p == NULL in sendAlarmImages(2)"));
			                //return 0;
			            } else {
				            *p = 0;
				            p += 2;

				            len = (int)strtol(p1, NULL, 16);;
				            p[len] = 0;
				            printf("%s in flag = 2 and response=%s\n", now_tmp->alert_image.file_name, p);
				            
				            strncpy(result, p, 2);       // ok   whatever 
				            result[2] = 0;
				            strlower(result);
				            trim(result);
				            if (strcmp(result, "ok") == 0) {
				                now_tmp->alert_image.flag = 2;
				                ret = write_into_sqlite(now_tmp->alert_image);
				                if (ret < 0) {
				                    printf("!!!!!!!!!!!!write to sqlite flag = 2 fail!!!!!!!!!!!!!!!!!!\n");
				                } else {
				                	sprintf(buf_echo, "ftpput %s and write to sqlite flag =2 is ok!", now_tmp->alert_image.file_name);
							recordLog(1, 12,buf_echo);
				                }
				            } else {
						 recordLog(1, 12,"the result is error!!!(sendAlarmImages())");
						 sprintf(temp_response, "%s the result is error!!!(sendAlarmImages()) ~%s~", buf_echo, paras);
						 printf("%s\n", temp_response);
						 recordLog(1, 12,temp_response);

						// in order to avoid chang hong kong server error
						now_tmp->alert_image.flag = 0;
				                ret = write_into_sqlite(now_tmp->alert_image);
				                if (ret < 0) {
				                    printf("write to sqlite flag = 0 fail\n");

				                }
				            }
			            }
		            }
	            }

	        }


	}//end of if(isLogin)


        free_tmp = now_tmp;
        now_tmp = now_tmp->next;
        free(free_tmp);

    }

    return 0;
}

int write_into_sqlite(struct alert_image tmp)
{

    sqlite3 *conn  = NULL;          // sqlite db handler
    char    sqlStr[256];            // sql statement
    int     ret;


    ret = sqlite3_open(DYNAMIC_DATABASE, &conn);                             // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(sendAlarmImages) - %s", DYNAMIC_DATABASE);
        recordLog(1, 16, "Openning database fail(sendAlarmImages) fail!");
        return -1;
    }

    sprintf(sqlStr, "update Images set Flag=%d where FileName='%s'\n", tmp.flag, tmp.file_name);

    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(sendAlarmImages) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute (sendAlarmImages()) fail!!"));
        return -1;
    }

    return 0;

}
