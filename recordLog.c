/*********************************************************************************************************************
  File name：recordLog.c
  Author：haozi
  Version：v1.0
  Date：2013-12－27
  
  useage:
  1. modiry the type:  LOCAL_SERVER_LOG, CARTVU_LOG, MEDIA_LOG
  2. use"   dealLog(0, 0, 0, 0);  "   to generate the global filename the clear tomorow File
  3. if network is ok please use"     dealLog(user, passwd, ip, 1);" or no use the function if network is false
  4. record the log use "recordLog(1, 1, "reboot time here");"
        

*********************************************************************************************************************/

#include "recordLog.h"

#include "const.h"
// 当前记录的文件名
char filename[FILE_NAME_SIZE] = {0};
extern int ftpFlag;
extern int dogFood;
extern int ftpDogFood;
extern char mac[];


/****************************************************************************
  Function name: void dealLog(char *user, char *passwd, char *ip, int network_ok)
Description: 

Entry:
user:
passwd:
ip:
network_ok: 1 meant the network is ok  0 . 0 meant meant the network is error
Return: 
 *****************************************************************************/


void dealLog(char *user, char *passwd, char *ip, int network_ok) {
    time_t timep;
    time_t tom;
    time_t yes;
    struct tm *p;
    int yesterday,today,tomorrow;
    FILE *fp;
    char *q;
    int lane;
    
    char buf_echo[BUF_ECHO_SIZE] = {0};
    char tomorrow_file[FILE_NAME_SIZE] = {0};
    char yesterday_file[FILE_NAME_SIZE] = {0};



    DIR *dir;
    struct dirent *dir_p;
    char *pos;


   if ((strcmp("Cartvu", NOW_TYPE) == 0) || (strcmp("Media", NOW_TYPE) == 0)) {
        fp = fopen(WHERE_ETH0_SETTING, "r");
        if (fp == NULL) {
            perror("fopen eth0-setting fail");
            return;
        }
        while (fgets(buf_echo, BUF_ECHO_SIZE, fp) != NULL) {
            if (strlen(buf_echo) == 0) break;
            if (strncmp(buf_echo, "LANE_NO", 7) == 0) {
                q = index(buf_echo, '=');
                if (q == NULL) {
                    fprintf(stderr, "the eth0-setting is error\n");
                } else {
                    *q = '\0';
                    q++;
                    strcpy(buf_echo, q);
                    lane = atoi(buf_echo);
                    printf("now the lane is %d\n", lane);
                }
                break;
            }
            

        }
    
   }




    time(&timep);
    p = localtime(&timep);
    today = p->tm_mday;
    

    yes = timep - 3600*24;
    p = localtime(&yes);
    yesterday = p->tm_mday;

    tom = timep + 3600*24;
    p = localtime(&tom);
    tomorrow = p->tm_mday;

    
   
   //printf("yes:%ld\ttoday:%ld\ttomorrow:%ld\n", yes, timep, tom);

    if (strcmp("LocalServer", NOW_TYPE) == 0) {
        sprintf(filename, "%sLocalServer-%02d.log", RUNNING_LOG_DIR, today);
        sprintf(tomorrow_file, "%sLocalServer-%02d.log", RUNNING_LOG_DIR, tomorrow);
        sprintf(yesterday_file, "%sLocalServer-%02d.log", RUNNING_LOG_DIR, yesterday);
     

    } else if (strcmp("Cartvu", NOW_TYPE) == 0) {
        sprintf(filename, "%sL%02d-C%02d.log", CARTVU_LOG_DIR, lane, today);
        sprintf(tomorrow_file, "%sL%02d-C%02d.log", CARTVU_LOG_DIR, lane, tomorrow);
        sprintf(yesterday_file, "%sL%02d-C%02d.log", CARTVU_LOG_DIR, lane, yesterday);


    } else if (strcmp("Media", NOW_TYPE) == 0) {
        sprintf(filename, "%sL%02d-M%02d.log", MEDIA_LOG_DIR, lane, today);
        sprintf(tomorrow_file, "%sL%02d-M%02d.log", MEDIA_LOG_DIR, lane, tomorrow);
        sprintf(yesterday_file, "%sL%02d-M%02d.log", MEDIA_LOG_DIR, lane, yesterday);


    }
    //printf("\nyesterday_file:%s\ntoday file:%s\ntomorrow_file:%s\n", yesterday_file, filename, tomorrow_file);



    // due to truncate the next day file
    //sprintf(buf_echo, "echo ''>%s", tomorrow_file);
    sprintf(buf_echo, "rm %s -rf", tomorrow_file);
    MY_SYSTEM(buf_echo);


    if (network_ok == 1) {
        printf("\n!!!!!!!!!!!!!!!!!!!start ftpput the log!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        // ftpput yesterday file
        //ftpputLog(user, passwd, ip, yesterday_file);


        if (strcmp("LocalServer", NOW_TYPE) == 0) {
            ///////
            if((dir = opendir(CARTVU_MEDIA_LOG)) == NULL)
            {
                fprintf(stderr, "open %s fail\n", CARTVU_MEDIA_LOG);
                return;
            }
            while ((dir_p = readdir(dir)) != NULL) {
                if ((strcmp(dir_p->d_name, ".") == 0) || (strcmp(dir_p->d_name, "..") == 0)) {
                    printf("the file name is %s\n", dir_p->d_name);
                    continue;
                }
                pos = index(dir_p->d_name, '.');
                if (pos != NULL) {
                    if (strncmp(pos, ".log", 4) == 0) {
                        sprintf(buf_echo, "%s%s", CARTVU_MEDIA_LOG, dir_p->d_name);
                        
                        ftpputLog(user, passwd, ip, buf_echo);
                    } else {
                        printf("no need to ftpput %s\n", dir_p->d_name);
                    }
                } else {
                    printf("no need to ftpput %s\n", dir_p->d_name);
                }
                
            }
            ///////
            if((dir = opendir(RUNNING_LOG_DIR)) == NULL)
            {
                fprintf(stderr, "open %s fail\n", RUNNING_LOG_DIR);
                return;
            }
            while ((dir_p = readdir(dir)) != NULL) {
                if ((strcmp(dir_p->d_name, ".") == 0) || (strcmp(dir_p->d_name, "..") == 0)) {
                    printf("the file name is %s\n", dir_p->d_name);
                    continue;
                }
                pos = index(dir_p->d_name, '.');
                if (pos != NULL) {
                    if (strncmp(pos, ".log", 4) == 0) {
                        sprintf(buf_echo, "%s%s", RUNNING_LOG_DIR, dir_p->d_name);
                        
                        ftpputLog(user, passwd, ip, buf_echo);
                    } else {
                        printf("no need to ftpput %s\n", dir_p->d_name);
                    }
                } else {
                    printf("no need to ftpput %s\n", dir_p->d_name);
                }
                
            }
        } else if (strcmp("Cartvu", NOW_TYPE) == 0) {
            ///////
            if((dir = opendir(CARTVU_LOG_DIR)) == NULL)
            {
                fprintf(stderr, "open %s fail\n", CARTVU_LOG_DIR);
                return;
            }
            while ((dir_p = readdir(dir)) != NULL) {
                if ((strcmp(dir_p->d_name, ".") == 0) || (strcmp(dir_p->d_name, "..") == 0)) {
                    printf("the file name is %s\n", dir_p->d_name);
                    continue;
                }
                pos = index(dir_p->d_name, '.');
                if (pos != NULL) {
                    if (strncmp(pos, ".log", 4) == 0) {
                        sprintf(buf_echo, "%s%s", CARTVU_LOG_DIR, dir_p->d_name);
                        
                        ftpputLog(user, passwd, ip, buf_echo);
                    } else {
                        printf("no need to ftpput %s\n", dir_p->d_name);
                    }
                } else {
                    printf("no need to ftpput %s\n", dir_p->d_name);
                }
                
            }
            ///////

        } else {

            ///////
            if((dir = opendir(MEDIA_LOG_DIR)) == NULL)
            {
                fprintf(stderr, "open %s fail\n", MEDIA_LOG_DIR);
                return;
            }
            while ((dir_p = readdir(dir)) != NULL) {
                if ((strcmp(dir_p->d_name, ".") == 0) || (strcmp(dir_p->d_name, "..") == 0)) {
                    printf("the file name is %s\n", dir_p->d_name);
                    continue;
                }
                pos = index(dir_p->d_name, '.');
                if (pos != NULL) {
                    if (strncmp(pos, ".log", 4) == 0) {
                        sprintf(buf_echo, "%s%s", MEDIA_LOG_DIR, dir_p->d_name);
                        
                        ftpputLog(user, passwd, ip, buf_echo);
                    } else {
                        printf("no need to ftpput %s\n", dir_p->d_name);
                    }
                } else {
                    printf("no need to ftpput %s\n", dir_p->d_name);
                }
                
            }
            ///////


        }
        printf("\n!!!!!!!!!!!!!!!!!!!end ftpput the log!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }



}

//localserver:                                              //localserver_path                        //remote_path                                       
//ftpput -u LocalNodeFtp -p hsq1998 203.186.75.165 /AdNode/cartvu/images/I-11-20131209115246.jpg   /Cartvu/Images/000011/I-11-20131209115246.jpg 



//cartvu:                                               //local_path                                       //remote_path
//ftpput -u xxxx -p xxxx ip /cartvu/log/L08-C04.log /AdNode/log/CartvuMedia/L08-C04.log


//media:                                               //local_path                                       //remote_path
//ftpput -u xxx -p xxx ip /advertising/log/L08-M04.log /AdNode/log/CartvuMedia/L08-M04.log


/****************************************************************************
  Function name: void ftpputLog(char *user, char *passwd, char *ip, char *ftpput_file)
Description: 

Entry: 
user:
passwd:
ip:
ftpput_file:the absolute path of the file which need to ftpput

Return: 
 *****************************************************************************/


void ftpputLog(char *user, char *passwd, char *ip, char *ftpput_file) {
    char buf_echo[BUF_ECHO_SIZE] = {0};
    char original_name[FILE_NAME_SIZE] = {0};
    char buf_show[BUF_ECHO_SIZE] = {0};
    char mac_dir[20];

    char *p;
    FILE *fp;
    int error_flag = 0;
    int count = 5;

    if (strcmp(ftpput_file, filename) == 0) {
        printf("no need to update today file log:%s\n", ftpput_file);
        return;
    }
    strcpy(original_name, ftpput_file);
    while ((p = index(original_name, '/')) != NULL) {
        strcpy(original_name, p + 1);

    }
    printf("ftpput file name:%s\n", original_name);

    if (strcmp("LocalServer", NOW_TYPE) == 0) {
        strncpy(mac_dir, mac, 6);
        mac_dir[6] = '\0';
        sprintf(buf_echo, "ftpput -u %s -p %s %s %s %s%s/%s  2>%s", user, passwd, ip, ftpput_file, SERVICE_NODE_LOG_PATH, mac_dir, original_name, FTPPUT_ERROR_FILE);
    } else if (strcmp("Cartvu", NOW_TYPE) == 0) {
        sprintf(buf_echo, "ftpput -u %s -p %s %s %s %s%s 2>%s", user, passwd, ip, ftpput_file, CARTVU_MEDIA_LOG, original_name, FTPPUT_ERROR_FILE);
    } else if (strcmp("Media", NOW_TYPE) == 0) {
        sprintf(buf_echo, "ftpput -u %s -p %s %s %s %s%s 2>%s", user, passwd, ip, ftpput_file, CARTVU_MEDIA_LOG, original_name, FTPPUT_ERROR_FILE);
    }

    



    while (count--) { 
        ftpFlag = 1;
        ftpDogFood = WATCHDOG_COUNT;
        printf(buf_echo);
        printf("\n");
        MY_SYSTEM(buf_echo);
        ftpFlag = 0;

       

        fp = fopen(FTPPUT_ERROR_FILE, "r");
        if (fp == NULL) {
            fprintf(stderr, "open %s fail\n", FTPPUT_ERROR_FILE);
            return;
        }

        error_flag = 0;
        while (fgets(buf_show, BUF_ECHO_SIZE, fp)) {
            if(strlen(buf_show) == 0) {
                break;
            }
            printf("error:%s\n", buf_show);
            recordLog(1, 33, buf_show);
            
            error_flag = 1;
        }
        fclose(fp);
        if (error_flag == 0) {
            printf("ftpput the %s is ok\n", ftpput_file);
            //if (strncmp(original_name, "LocalServer", 11) != 0) {
                sprintf(buf_show, "rm %s", ftpput_file);
                recordLog(1, 33, buf_show);
                MY_SYSTEM(buf_show);
            //}
            sprintf(buf_show, "ftpput %s is ok", ftpput_file);
            recordLog(1, 33, buf_show);
            break;
        } else {
            fprintf(stderr, "!!!!!!!!!!!!!!ftpput  %s is fail!!!!!!!!!!!!!!!!!!!\n", ftpput_file);
            sprintf(buf_show, "ftpput %s is fail", ftpput_file);
            recordLog(1, 33, buf_show);
            //sprintf(buf_show, "rm %s", FTPPUT_ERROR_FILE);
            //MY_SYSTEM(buf_show);
            sleep(2);

            
            //recordLog(1, 33, ftpput_file);
        }
        

    }


}



/****************************************************************************
  Function name: void recordLog(int type, int code, char *info)
Description: 

Entry: 
type:which mode ,such as   camera thread   sensor thread  time synchronize
code:the code of error number
info:you shall transmit the info if you want to record
Return: 
 *****************************************************************************/

void recordLog(int type, int code, char *info) {
    time_t timep;
    struct tm *p;
    char buf_echo[1024];
    char *c;

    
    c = info;
    while ((c = index(c, '\'')) != NULL) {
            *c = '~';        
    }
/*
    c = info;
    while ((c = index(c, '\n')) != NULL) {
            *c = '!';        
    }
*/
    c = info;
    while ((c = index(c, '\"')) != NULL) {
            *c = '@';        
    }

    
    time(&timep);
    p = localtime(&timep);

    sprintf(buf_echo, "echo '%04d-%02d-%02d %02d:%02d:%02d %s:[%02d%02d] %s' >>%s", 
            p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, NOW_TYPE, type, code, info, filename);
    
    //printf("%s\n", buf_echo);
    MY_SYSTEM(buf_echo);
}
