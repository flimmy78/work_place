/*************************************************************************************
Function: int softwareCallback(void *, int, char**, char**)
Description: callback function that query version of local software from database

Entry: ......
Return: 0
 **************************************************************************************/
#define SOFTWARE_SIZE    20
struct ss{
    char key[40];
    char value[40];
    char path[100];
} software[SOFTWARE_SIZE];
int softId;

int softwareCallback(void *para, int n_column, char **column_value, char **column_name) {
    // fields:software location version minorver updatedate filename
    //printf("softId = %d\n", softId);
    //printf("c1 = %s\tc2 = %s\tc3 = %s\tc5 = %s\n", column_value[0], column_value[2], column_value[3], column_value[5]);
    strcpy(software[softId].key, column_value[0]);           // software name
    sprintf(software[softId].value, "%d.%02d", atoi(column_value[2]), atoi(column_value[3]));   // software version
    strcpy(software[softId].path, column_value[5]);          // software path
    softId++;
    return  0;
}  // end
/*********************************************************************************************
  function name：downloadSoftware()   download softwares of DEVICE from service server
description: Sending a http request to service server to get all the software version.
Then comparing the software version with local and then decide if download.

entry： int type    0 - cartvu, 1 - media, 2 - localnode
return：0  -- success
<0  -- fail(wrong type)
 **********************************************************************************************/
int downloadSoftware(int type) {
    //FILE *fp;
    char result[2048], para[50], srcFile[200], tarFile[200];
    char *p, *p1, *p2;
    int len, i, j;
    double ver, mver, ver1;
    char sysCmd[256];               // Just for shell command
    int  resetFlag = 0;
    int  main_reset = 0;

    paraStruc paraS[50];            //

    sqlite3 *conn  = NULL;          // sqlite db handler
    char    sqlStr[500];            // sql statement
    int     ret;

    struct tm *ptm;
    time_t timep;

    int count;
    char buf_echo[MAX_SIZE] = {0};
    char temp_response[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];


    if (type < 0 || type > 2) return -1;
    // 1. query the version of device
    sprintf(para,"mac=%s&", mac);
    switch (type) {
        case 0:
            strcat(para, "t=cartvu");
            break;
        case 1:
            strcat(para, "t=media");
            break;
        case 2:
            strcat(para, "t=localnode");
            break;
    }
    sprintf(request, 
            "POST /software.php HTTP/1.1\nHOST:%s\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            serverDomain1, strlen(para), para);
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
	recordLog(1, 2, "the network maybe error(sendAlarmImages())");
	isLogin = 0;
	return -1;
    }
    //printf("response=%s\n", response);
    if (ret < 0) {
        printf("the network maybe error\n");
        recordLog(1, 2, "the network maybe error(downloadSoftware)");
        isLogin = 0;
        return -1;
    }
    p = strstr(response, "\r\n\r\n");
    if (p == NULL) {
        printf("fail : Wrong response - 1\n");
        recordLog(1, 2, "Wrong response -1(downloadSoftware)");
        return -1;
    }
    p += 4;

    p1 = p;
    p = strstr(p, "\r\n");
    if (p == NULL) {
        printf("fail : Wrong response - 2\n");
        recordLog(1, 2, "Wrong response -2(downloadSoftware)");
        return -2;
    }
    *p = 0;
    p += 2;

    len = (int)strtol(p1, &p2, 16);                     // length of the return message
    p[len] = 0;
    strcpy(result, p);
    result[len] = 0;
    // 获取键值对
    stringSeparate(result, paraS, SOFTWARE_SIZE);
    // initial the structure software[]
    softId = 0;
    for (i = 0; i < SOFTWARE_SIZE; i++) {
        software[i].key[0] = 0;
    }  // end of for

    // get all the software of local node from database
    ret = sqlite3_open(VERSION_DATABASE, &conn);                           // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(downloadSoftware) - %s\n", VERSION_DATABASE);
        recordLog(1, 16, "Openning VERSION_DATABASE database fail(downloadSoftware) fail!");
        return -1;
    }


    sprintf(sqlStr, "select * from Version where location=%d", type);      // SQL语句
    count = 0;
    while (1) {
        count++;
        ret = sqlite3_exec(conn, sqlStr, softwareCallback, NULL, NULL);              // 执行
        if (ret != SQLITE_OK) {
            if (count > TRY_COUNT) {
                printf("fail :try %d times in %s\n", count, sqlStr);
                printf("fail : Execute fail(downloadSoftware) - %s\n", sqlStr);
                sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                system(buf_echo);
                recordLog(1, 34, buf_echo);
                break;
            }
        } else {
            //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);

            //system(buf_echo);
            break;
        }
    }

    sqlite3_close(conn); 

    // traverse all the softwares
    for (i = 0; i < SOFTWARE_SIZE; i++) {
    	
        if (paraS[i].key == NULL) {
        	if (i == 0) {
			printf("information is wrong here\n");
			recordLog(1, 34, strcat(temp_response, " download Software error"));
			
        	}
        	break;                                     // end of traversing
        }
        //strlower[paraS[i].key);
        for (j = 0; j < SOFTWARE_SIZE; j++) {
            //printf("software[%d] = %s\tparaS[%d] = %s\n", j, software[j].key, i, paraS[i].key);
            if (software[j].key[0] == 0) break;
            resetFlag = 0;

            if (strcmp(software[j].key, paraS[i].key) == 0) {
                // Found the same software
                //printf("software version = %s\n", paraS[i].value);
                if (strcmp(paraS[i].value, software[j].value) > 0) {
                    ver1 = atof(paraS[i].value);
                    mver = modf(ver1, &ver) * 100;
                    //printf("software version = %s\tver1=%f\tver=%f\tmver=%f\n", paraS[i].value, ver1, ver, mver);
                    // The version of software in server is newer than local
                    // download this software and update the database
                    switch (type) {
                        case 0:
                            sprintf(srcFile, "%s%s/%s", REMOTE_FTP_CARTVU_PATH, REMOTE_FTP_SOFTWARE_PATH, software[j].key);
                            break;
                        case 1:
                            sprintf(srcFile, "%s%s/%s", REMOTE_FTP_MEDIA_PATH, REMOTE_FTP_SOFTWARE_PATH, software[j].key);
                            break;
                        case 2:
                            sprintf(srcFile, "%s%s/%s", REMOTE_FTP_LOCALNODE_PATH, REMOTE_FTP_SOFTWARE_PATH, software[j].key);
                            break;
                    }
                    sprintf(tarFile, "%s%s", TEMP_PATH, software[j].key);               // file name that download from service server
                    sprintf(sysCmd, "ftpget -u %s -p %s %s %s %s 2>%s", ftpUser, ftpPassword, ip, tarFile, srcFile, RECORD_ERROR);
                    printf("sysCmd = %s\n", sysCmd);
                    printf("Download file: %s\n", software[j].key);

                    
                    ftpFlag = 1;
                    ftpDogFood = WATCHDOG_COUNT;
                    system(sysCmd);
                    sprintf(buf_echo, "download %s", tarFile);
                    if (check_shell_error(RECORD_ERROR) < 0) {
                        recordLog(1, 14, strcat(buf_echo, "   is fail!!!"));
                    } else {
                        recordLog(1, 14, strcat(buf_echo, "   is ok!!!"));
                    }
                    ftpFlag = 0;


                    

                    // check if downloading file is exist, update version in database
                    if (access(tarFile, F_OK) == 0) {                                   // file exist
                        if (checkSum(tarFile) == 0) {                                     // checksum == 0
                            //printf("software version = %s\tver1=%f\tver=%f\tmver=%f\n", paraS[i].value, ver1, ver, mver);
                            
			    sprintf(sysCmd, "chmod 777 %s", tarFile);
			    system(sysCmd);
                            if (type != 2) {     // not Local Server
                                sprintf(sysCmd, "cp -f %s %s", tarFile, software[j].path);        // copy file to right place
                                //printf("sh command=%s\n", sysCmd);
                                system(sysCmd);

				sprintf(sysCmd, "chmod 777 %s%s", software[j].path, software[j].key);
				recordLog(1, 14, sysCmd);
				printf("%s\n", sysCmd);
				system(sysCmd);
                                
                            } else {             // software of local server
                                if (strcmp(software[j].key, "www.zip") == 0) {
                                    // unzip into /AdNode/
                                    sprintf(sysCmd, "unzip -o -q %s -d /AdNode/www/", tarFile);        // copy file to right place
                                    printf("sh command=%s\n", sysCmd);
                                    system(sysCmd);
                                    system("chmod 777 /AdNode/www/cgi-bin/* -R");
                                    system("chmod 777 /AdNode/www/alarm/*.cgi -R");
                                } else if (strcmp(software[j].key, "cgi.zip") == 0) {
                                    // unzip into /AdNode/www/
                                    sprintf(sysCmd, "unzip -o -q %s -d /AdNode/www/cgi-bin/", tarFile);        // copy file to right place
                                    printf("sh command=%s\n", sysCmd);
                                    system(sysCmd);
                                    system("chmod 777 /AdNode/www/cgi-bin/*");
                                } else if (strcmp(software[j].key, "localroot.zip") == 0) {
                                    sprintf(sysCmd, "mkdir /tmp/root/ -p");
                                    printf(sysCmd);
                                    printf("\n");
                                    system(sysCmd);
                                    
                                    
                                
                                    // unzip into /tmp/root/
                                    sprintf(sysCmd, "unzip -o  %s -d /tmp/root/", tarFile);        // copy file to right place
                                    printf("sh command=%s\n", sysCmd);
                                    system(sysCmd);
                                    
                                    sprintf(sysCmd, "chmod 777 /tmp/root/ -R");
                                    printf("%s\n", sysCmd);
                                    system(sysCmd);
                                    
                                    sprintf(sysCmd, "cp /tmp/root/*   / -rf");
                                    printf("%s\n", sysCmd);
                                    system(sysCmd);

                                    sprintf(sysCmd, "rm /tmp/root/* -rf");
                                    printf("%s\n", sysCmd);
                                    system(sysCmd);
                                    
                                } else if (strcmp(software[i].key, "main") == 0 || strcmp(software[i].key, "sntpServer") == 0) {
                                    resetFlag = 1;                
                                } else {
                                    sprintf(sysCmd, "cp -f %s %s", tarFile, software[j].path);        // copy file to right place
                                    //printf("sh command=%s\n", sysCmd);
                                    system(sysCmd);
                                    
				    sprintf(sysCmd, "chmod 777 %s%s", software[j].path, software[j].key);
				    recordLog(1, 14, sysCmd);
				    printf("%s\n", sysCmd);
				    system(sysCmd);
                                    
                                }
                            }
                            recordLog(1, 32, strcat(sysCmd, " is ok!!!"));


                            timep = time((time_t *)NULL);
                            ptm = localtime(&timep);
			    // get all the software of local node from database
			    ret = sqlite3_open(VERSION_DATABASE, &conn);                           // 打开数据库
			    if (ret != SQLITE_OK) {
			        printf("fail : Openning database fail(downloadSoftware) - %s\n", VERSION_DATABASE);
			        recordLog(1, 16, "Openning VERSION_DATABASE database fail(downloadSoftware) fail!");
			        return -1;
			    }


                            sprintf(sqlStr, "update Version set Version=%d, Minorver=%d, Updatedate='%4d%02d%02d' where location=%d and software='%s'",
                                    (int)ver, (int)(mver + 0.1),
                                    1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday,
                                    type, software[j].key);                          // SQL语句
                            //printf("sql = %s\n", sqlStr);
                            ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);              // update database
                            if (ret != SQLITE_OK) {
                                printf("fail : Execute fail(downloadSoftware) - %s", sqlStr);
                                recordLog(1, 34, strcat(sqlStr, " fail : Execute fail(downloadSoftware)"));
                            }
                            sqlite3_close(conn); 
                            
                        } else {
                            printf("checksum of %s is fail", tarFile);
                            sprintf(buf_echo, "checksum of %s is fail", tarFile);
                            recordLog(1, 19, buf_echo);

                        }// end if (checkSum())
                        if (resetFlag == 0) {
                            unlink(tarFile);
                        } else {
                            
                            main_reset = 1;
                        }
                    } else {
                        printf("~~~~~~~~~~~~~~~~~~Can not download file: %s~~~~~~~~~~~~~~~~~~~~~~\n", software[j].key);
                        
                    }
                    
                }  // end of if
                //printf("you should break here\n");
                break;
            }  // end of if
        }  // end of for
    }  // end of for



    if (main_reset == 1) {
	printf("Reset here\n");
	reset();
    }
    return 0;  
}  // end of downlodeSoftware

