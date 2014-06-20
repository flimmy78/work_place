#ifndef RECORD_LOG_H
#define RECORD_LOG_H

#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
 
 
#define LOCAL_SERVER_LOG
//#define CARTVU_LOG
//#define MEDIA_LOG

#define SERVICE_NODE_LOG_PATH       "/LocalNode/Log/"

// cartvu和media上传日志文件处
#define CARTVU_MEDIA_LOG    "/AdNode/log/CartvuMedia/"

// localserver的记录日志的位置
#define RUNNING_LOG_DIR         "/AdNode/log/LocalServer/"

//cartvu 记录文件位置
#define CARTVU_LOG_DIR          "/cartvu/log/"
// media 记录文件位置
#define MEDIA_LOG_DIR           "/advertising/log/"

#define WHERE_ETH0_SETTING       "/etc/eth0-setting"


// 当前要记录的类型
#if defined(LOCAL_SERVER_LOG)
#define NOW_TYPE         "LocalServer"
#define FTPPUT_ERROR_FILE   "/AdNode/log/ftpput_error_file"
#define MY_SYSTEM      system

#else
#if defined(CARTVU_LOG)
#define NOW_TYPE         "Cartvu"  
#define FTPPUT_ERROR_FILE   "/cartvu/log/ftpput_error_file"
#else
#if defined(MEDIA_LOG)
#define NOW_TYPE         "Media"  
#define FTPPUT_ERROR_FILE   "/advertising/log/ftpput_error_file"
#endif
#endif
#define MY_SYSTEM      my_system
extern int my_system(const char*);
#endif




#define BUF_ECHO_SIZE    1024
#define FILE_NAME_SIZE   100


extern char filename[FILE_NAME_SIZE];



void ftpputLog(char *user, char *passwd, char *ip, char *ftpput_file);

void recordLog(int type, int code, char *info);
void dealLog(char *user, char *passwd, char *ip, int network_ok);



#endif