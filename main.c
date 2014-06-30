/*********************************************************************************************************************
  File name：main.c
  Description：Main program for Local Server

  Compile：arm-linux-gcc main.c -o main -I../../opt/sqlite3/include -L../../opt/sqlite3/lib -lsqlite3 -lm -lpthread
note: -I         include path for sqlite
-L         library path of sqlite
-lsqlite3  need sqlite3 library
-lm        need math library

Author：Songqing Hua
Version：v1.0
Date：2013-10－28

Change log：
1.  2013-10-26    v1.00          Main functions and watchdog
2.  2013-10-27    v1.01          check reset time function
3.  2013-10-28    v1.02          Add download localnode software function
4.  2013-10-29    v1.03          Update cartvu and media software
5.  2013-10-31    v1.05          Download softwares every 100 seconds. Relogin every 1 hour. Download media command file
6.  2013-11-01    v1.07          
7.  2013-11-02    v1.08          Correct media command process
8.  2013-11-03    v1.09          Function: updateStatus(). update status in table mac.
9.  2013-11-09    v1.11          Figure out a lot of problems
10. 2013-11-13    v1.12          Watchdog problem when downloading software
11. 2013-11-13    v1.13          Do not restart when can not download the local server config file
12. 2013-11-14    v1.14          use ftpget replace shell in downloading LN CC & MC config file
13. 2013-11-14    v1.15          try to figure out the segment fault problem in sendStatus()
14. 2013-11-14    v1.16          open a wrong database in sendStatus()
15. 2013-11-14    v1.17          improve sendAlarmImages()
16. 2013-11-14    v1.18          fill 0 at the end of var result in sendStatus.inc
17. 2013-11-14    v1.19          just figure out many small mistakes
18. 2013-11-14    v1.20          Figure out p==NULL 
19. 2013-11-14    v1.21          Do not reset when can not get internet
20. 2013-11-14    V1.22          Do not reset when downloading images or movies
21. 2013-11-14    v1.23          ....
22. 2013-11-14    v1.24          ....
23. 2013-11-15    v1.25          Use ftpget instead of shell when download advertising images
24. 2013-11-15    v1.26          Just switch off login to server after sending statistics
25. 2013-11-15    v1.27          Just set statustime of table mac to now() instead of string "0000-00-00 00:00:00"
26. 2013-11-15    v1.28          Just correct a mistake of v1.27
27. 2013-11-15    v1.29          Just correct a mistake of v1.28
28. 2013-11-15    v1.30          Just correct a mistake of v1.29
29. 2013-11-15    v1.31          clear status every 40 secs
30. 2013-11-18    v1.32          Transition version for testing
31. 2013-11-19    v1.33          figure out the problem about device's status
32. 2013-11-19    v1.35          figure out a segment fault in loginserver()
33. 2013-11-20    v1.36          Just a new version for download.
34. 2013-11-20    v1.37          figure out a mistake for processing media command
35. 2013-11-20    v1.38          figure out a mistake of downloading media image
36. 2013-11-21    v1.39          just for test
37. 2013-11-22    v1.40          figure out the problem about processing command file
38. 2013-11-22    v1.41          remove some note message
39. 2013-11-23    v1.43          correct mistake about lane no when send status to service server
40. 2013-11-23    v1.44          correct mistake that interruption of sending status
41. 2013-11-25    v1.45          Correct mistakes when sending status to remote server
42. 2013-11-26    v1.46          Correct mistakes that is in cleanDatabase
43. 2013-11-26    v1.47          Just for media statistics debug
44. 2013-11-27    v1.48          Change some places for transferring alarm images
45. 2013-11-27    v1.49          Correct mistake of updating the www.zip
46. 2013-11-28    v1.50          Change date in config to yyyymmddhhmmss

47. 2013-12-13    v1.51          sprintf(sysCmd, "unzip -o -q %s -d /AdNode/www/", tarFile);  sprintf(sysCmd, "unzip -o -q %s -d /AdNode/www/cgi-bin/", tarFile); 
48. 2013-12-16    v1.51          change the reset function to  watdog thread,   change the function of sendAlarmImages.inc
change the query times of sqlite,  add bzero(response, sizeof(response));
add delete useronline every 120sec
49. 2013-12-19    v1.52          modify the checksum to /AdNode/proc/checksum
change sql.txt to /AdNode/proc/sql.txt  initialize the iii=0 and break if try time is more than 10 times
do the command.cfg need to checksum in server node?


50. 2013-12-24    v1.53          change the method of send status to udp method 

51. 2013-12-25    v1.54          update the max lane error and the network  and feed the dog in login

52. 2013-12-25    v1.55           add setting the ip if the network is error
53. 2013-12-26    v1.56           modify the ifconfig error, modify the update LocalNodeVersion.db  first cp and then update the sqlite
54. 2013-12-26    v1.57           modify the ftpput error add ftpFlag = 1; 
54. 2013-12-30    v1.58          modify the ftpput yesterfile position after send status
55. 2013-12-31    v1.58             add download command.cfg every one hour                     
56. 2014-01-03    v1.59           modify the ftpput times until 5
57. 2014-01-06    v1.60           modify the record.c 
58. 2014-01-07    v1.61            change the ftpput log dir relative to mac 
59. 2014-01-08    v1.62           change permission of download file to 755
not use the new command.cfg if the cfg file is not right
60. 2014-01-09    v1.63           not ftpput today log file  and ftpput all the log in localserver
61. 2014-01-24    v1.64           modify the clear database time, add the 'df' command to know the localserver space
send alarm images to service node   before clean database
62. 2014-01-25    v1.65           modify the ftpput and ftpget
63. 2014-02-13    v1.66           modify the times of ftpget file of command.cfg to 5;
64. 2014-03-03    v1.67           modify the post of method add mac=xxxxxxxx;   alarm.php   checklane.php  software.php  statis.php  status.php
65. 2014-03-04    v1.68           move send status to another siglethread
66. 2014-03-05    v1.69           modify the segment error where receive is error   and add mutex in sendReauest.inc to avoid multiple post request
67. 2014-03-07    v1.70           remove the mutex in send request and p == NULL in send alarm image error
68. 2014-03-07    v1.71           add ftpput today's log file to service node
69. 2014-03-07    v1.72           get software  command every half an hour  modify the command.cfg error if fail to get  and move the ftpget command to thread
modify the watchdog count to 15

70. 2014-03-11    v1.73           modify the ftpget command.cfg and record fail when download fail
71. 2014-03-14    v1.74           for test 
72. 2014-03-14    v1.75           restart machine when receive "restart"
73. 2014-03-14    v1.76           will killall ftpput and ftpget when ftpput or ftpget add ftpDogFood in ftpget and ftpput
and move the download software to thread

74. 2014-03-17    v1.77           modify the sendRequest.inc and locked 
75. 2014-03-18    v1.78           add mux in sendRequest.inc 
76. 2014-03-21    v1.79           modify the sendRequest, will save the buf before unlocked sendRequest
77. 2014-03-21    v1.80           add record when network is error 


78. 2014-03-21    v1.81           
79. 2014-03-27    v1.82           modify the lane size to 50
80. 2014-04-01    v1.83           add db add sendinfo.cgi    add dialogure
81. 2014-04-09    v1.84           modify the error send status of log 
82  2014-04-10    v1.85           modify the sequence of generate ftp passwd and login
83  2014-04-10    v1.86           modify the sequence of getip
84  2014-04-21    v1.87           add thread about who is localserver
85  2014-05-15    v1.88           modify the permission  check if the download is ok
86  2014-05-15    v1.89           modify the checklane when is network is bad,add restart the media or cartvu
87  2014-05-15    v1.90           modify the permission of AdNode  recursive to 777 
88  2014-05-23    v1.91           modify the sendStatus record log
89  2014-05-23    v1.92           modify the record log when send status and download software is at the same time
90  2014-05-23    v1.93           add ad download complete  
91  2014-05-23    v1.94           modify the connect timeout to 12sec 
92  2014-05-23    v1.95           modify the send cartvumedia static herer
93  2014-06-03    v1.96           add reset the cradle point  
94  2014-06-05    v1.97           add show media adstatus and create the info here     
95  2014-06-05    v1.98           add re login when first download the command.sys  
96  2014-06-19    v1.99           add get userdef.zip from different path
                                  modify when hongkong server change has no alalm file
97  2014-06-19    v1.99           modify cross socket receive error
98  2014-06-30    v1.99           add Connection: close to post
                                  modify checkLane when the the num is bigger than service node



**********************************************************************************************************************/
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

#include <sys/ioctl.h>
#include <linux/if.h>
#include <unistd.h>
#include <fcntl.h>


#include   "strseparate.h"
#include   "const.h"
#include    "recordLog.h"





pthread_mutex_t  post_request;

pthread_mutex_t  work_mutex;


pthread_mutex_t  send_mutex;



#define RECORD_ERROR  "/AdNode/proc/record_error"

#define SET_DISABLE_TIME      30
#define TMP_ERROR       "/AdNode/proc/tmp_error"

#define TRY_COUNT   100

#define MAX_SIZE           1024              // length of buffer
#define MAX_VALUE          20                // the Max number of key/value pairs
#define MAXLEN_RESPONSE    1024              // length of reponse buffer
#define HTTP_PORT          80

// ============== Global varibles ================
char  ip[20];                                // server's ip address
char  mac[24];                               // mac address
char  ftpUser[20] = "LocalNodeFtp";                           // ftp username to service node
char  m1m2m3[10];                            // M1M2M3 in MAC address
char  ftpPassword[20] = "hsq";                       // ftp password tp service node

char  configDate[16];                        // Date of config file
char  serverDomain1[30];                     // Domain name of server
char  serverDomain2[30];                     // Somain name of backup server

char  openningTime[6];                       // Opening time in config file
char  closeTime[6];                          // closing time in config file
char  resetTime[6];                          // Reset time in config file
int   timeZone;                              // timezone in config file. it is no use

int   isLogin = 0;                           // Login to the server

char  request[MAXLEN_RESPONSE];              // Request string that i will send out
char  response[2 * MAXLEN_RESPONSE];         // response from server

FILE *tmpFp;                                 // handler of a temp file

int dogFood = WATCHDOG_COUNT;                // watchdog timeout count.
int ftpDogFood = WATCHDOG_COUNT; 

int ftp_thread_food = FTP_THREAD_COUNT;

int watchFd;                                 // handler of watchdog device

int ftpFlag = 0;                             // Set 1 if using ftpget, ftpput or ftp shell .etc

int global_cartvu_lane_size = 0;
int global_media_lane_size = 0;
int max_lane = 0;


#define FILE_SIZE    50




#include "post_ad_ok.c"


#include   "SendInfoFun.c"

char send_info[50];





struct alert_image {
    char file_name[FILE_SIZE];
    char alert_time[FILE_SIZE];
    char cashier[FILE_SIZE];
    char barcode[FILE_SIZE];
    char confirmed;
    char flag;
    char state;
};


//
struct alert_image_more {
    struct alert_image alert_image;
    struct alert_image_more *next;
    struct alert_image_more *front;
};


struct alert_image_more  *alert_head;


int write_into_sqlite(struct alert_image tmp);

int check_shell_error(char *record_file);

void updateCommand();

void download_all_software();


//===========================================================================

/****************************************************************************
  function name: void reset()
description: system restart 

entry: NONE
return: none
 *****************************************************************************/
void reset() {
    int timeout;

    dogFood = 0;
    timeout = 1;
    ioctl(watchFd, WDIOC_SETTIMEOUT, &timeout);
    printf("reboot the machine in main\n");
    sleep(WATCHDOG_TIME_OUT + 10);
}
/****************************************************************************
Function: int checksum(char *)
Description: Calculate the CHECKSUM of a file

Entry: char *filename      file Name
Return: checksum of the file
-1        fail
 *****************************************************************************/
int checkSum(char *filename) {
    FILE *m;
    char sum, oneChar;
    long unsigned int len = 0;

    m = fopen(filename, "rb");
    if (m == NULL) {
        printf("\n File %s Not Found!", filename);
        return -1;
    }
    sum = 0;

    while (fread(&oneChar, 1, 1, m) > 0) {
        len++;
        sum += oneChar;
    }
    fclose(m);

    if (len > 0) return (int)sum;
    else return -1;
}

/****************************************************************************
  function name: int checkReset()
description: Check reset time and stop the watchdog 

entry: NONE
return: 0  -- always
 *****************************************************************************/
int checkReset() {
    int reset_hour, reset_min;
    time_t timep;
    struct tm *p;

    time(&timep);
    p = localtime(&timep);

    reset_hour = (resetTime[0] - '0') * 10 + (resetTime[1] - '0');
    reset_min  = (resetTime[3] - '0') * 10 + (resetTime[4] - '0');

    //reset_min -= 30;
    if (p->tm_hour == reset_hour && reset_min == p->tm_min) {
        if (p->tm_sec < 10) {
            printf("due to the restime here will reboot the machine\n");
            reset();
        }
    }
    return 0;
}
/**************************************************************************
  File Name: getIpAndMac()
  It includes 2 functions.
 ***************************************************************************/
/*****************************************************
Function: int getMac(char *)
Description: Get MAC address

Entry: char *mac    buffer for mac address
return: 0  -- success
-1  -- fail
 ******************************************************/
/**********************************************************************
Function: int getIpViaDomain(char *, char *)
Description: Get IP address via DNS

Entry: char *domain      Domain name
char *ip          buffer for ip address
Return: 0  -- Success, put ip address in var ip
－1 -- Fail
 ***********************************************************************/
#include "getIpAndMac.inc"
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
#include "sendRequest.inc"
/*****************************************************************
function: genFtpPassword
Description: Generate a new FTP password for cartvu and media

Entry: NONE
Return: 0     success
<0    fail
 ******************************************************************/
int genFtpPasswd() {
    char password[10], myMac[200];
    char s[200];
    int i;
    int fd;
    //FILE *fp;

    // 1. generate a new password. password's length is 8. a letter and a digit crossed.
    srand(time(0));
    for (i = 0; i < 8; i++) {
        if (i % 2 == 0)
            password[i] = 'a' + rand() % 26;
        else
            password[i] = '0' + rand() % 10;
    }  // end of for
    password[i] = 0;
    strlower(password);

    // 2. get MAC address
    strcpy(myMac, mac);
    //printf("password=%s\tmac=%s\n", password, myMac);
    // 3. write the password to a asigned file
    fd = open(FTPPASSWD_FILE, O_WRONLY | O_CREAT);
    if (fd == 0) {
        return -1;
    }  // end of if
    if (write(fd, password, FTPPASSWD_LEN) != 8) {
        close(fd);
        return -2;
    }  // end of if
    close(fd);
    // 4. change the password of FTP user
    myMac[6] = 0;
    sprintf(s, "/AdNode/proc/setpwd.sh F%s %s 2>%s", myMac, password, RECORD_ERROR);
    //printf("s=%s\n", s);
    system(s);
    if (check_shell_error(RECORD_ERROR) < 0) {
        recordLog(1, 42, "execute /AdNode/proc/setpwd.sh fail");
    }
    return  0;

}  // end
/************************************************************************
Function: int readConfig()
Description: Read from config file

Entry:  None
Return: 0 -- Success
-1 -- Fail
 *************************************************************************/
#include "readConfig.inc"
/************************************************************************
Function: int writeConfig()
Description: Write to config file

Entry: None
Return: 0    Success
-1   fail
 *************************************************************************/
int writeConfig() {
    return 0;

}  // end writeConfig
/************************************************************
Function: int loginServer()
Description: LocalNod Login to ServiceNode

Entry: NONE
Return: 0     Success
<0    Fail
 *************************************************************/
int loginServer() {
    char result[2048];
    char *p, *p1, *p2;
    int len;
    int ret = 0;
    static int count = 0;
    static int first_flag = 1;
    char buf_echo[1024];
    char temp_response[MAXLEN_RESPONSE];
    char response[MAXLEN_RESPONSE];


    sprintf(request,
            "POST /login.php HTTP/1.1\nHOST:%s\nConnection: close\nContent-Type:application/x-www-form-urlencoded\nContent-Length:16\n\nmac=%s\n\n",
            serverDomain1, mac);
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
        count++;
        if (count >= 450) {//8*450=1hour 
            recordLog(1, 2, "the network maybe error(loginServer)");
        }
        if (first_flag == 1) {
            first_flag = 0;
            recordLog(1, 2, "the network maybe error(loginServer)");
        }
        isLogin = 0;
        return -1;
    }

    //printf("response=%s\n", response);
    strcpy(buf_echo, response);
    if (response == NULL) {
        printf("No response for Login\n");
        recordLog(1, 2, "no response for login");
        return -1;
    }
    p = strstr(response, "\r\n\r\n");
    if (p == NULL) {
        printf("Can't find string double CRLF int response.(loginServer())\n");
        sprintf(result, "Can't find string double CRLF int response.(loginServer()) :%s", buf_echo);
        recordLog(1, 2, result);
        return -2;
    }

    p += 4;

    p1 = p;
    p = strstr(p, "\r\n");
    if (p == NULL) {
        printf("Can't find string CR & LF int response.(loginServer())\n");
        sprintf(result, "2222Can't find string double CRLF int response.(loginServer()) :%s", buf_echo);
        recordLog(1, 2, result);
        return -2;
    }
    *p = 0;
    p += 2;

    len = (int)strtol(p1, &p2, 16);;
    p[len] = 0;
    //printf("p1=%s   p=%s   len=%d", p1, p, len);

    strncpy(result, p, 9);       // login ok! whatever 
    result[9] = 0;
    strlower(result);
    printf("1 result=%s\n", result);
    if (strcmp(result, "login ok!") != 0) {
        // Login fail.
        printf("Login returns error:%s.\n", buf_echo);
        sprintf(result, "the service node send error info(loginServer) :%s", buf_echo);
        recordLog(1, 2, result);
        return -3;
    }  // end of if

    strcpy(result, &p[9]);
    //printf("2 result=%s\n", result);
    p = index(result, '=');
    *p = 0;
    p++;
    strlower(result);
    if (strcmp(result, "password") != 0) {
        printf("service node send error information\n");
        sprintf(result, "service node send error information :%s", buf_echo);
        recordLog(1, 2, result);
        return -1;
    }

    strcpy(ftpPassword, p);
    return 0;
}  // end of loginServer
/************************************************************
Function: int updateConfig()
Description: Update config file of localnode

Entry: NONE
Return: 0     success
<0    fail
 *************************************************************/
#include "updateConfig.inc"
/************************************************************
Function: int updateMediaCmd()
Description: Update command file of MEDIA

Entry: NONE
Return: 0     success
<0    fail
 *************************************************************/
#include "updateMediaCmd.inc"
/************************************************************
Function: int updateMediaConfig()
Description: Update config file of MEDIA

Entry: NONE
Return: 0     success
<0    fail
 *************************************************************/
#include "updateMediaConfig.inc"
/************************************************************
Function: int updateCartvuConfig()
Description: Update config file of CARTVU

Entry: NONE
Return: 0     success
<0    fail
 *************************************************************/
#include "updateCartvuConfig.inc"
/*************************************************************************************
  File Name: downloadSoftware.inc
  it includes 2 functions.
 **************************************************************************************/
/*************************************************************************************
Function: int softwareCallback(void *, int, char**, char**)
Description: callback function that query version of local software from database

Entry: ......
Return: 0
 **************************************************************************************/
/*********************************************************************************************
  function name：downloadSoftware()   download softwares of DEVICE from service server
description: Sending a http request to service server to get all the software version.
Then comparing the software version with local and then decide if download.

entry： int type    0 - cartvu, 1 - media, 2 - localnode
return：0  -- success
<0  -- fail(wrong type)
 **********************************************************************************************/
#include "downloadSoftware.inc"
/*******************************************************************************************
FileName: CartvuMediaStatis.inc
It includes 3 functions.
 ********************************************************************************************/
/*******************************************************************************************
Function: int cartvuStatisCallback(void *, int, char**, char**)
Description: callback function that query statistics of cartvu from database
that did not send out

Entry: ......
Return: 0
 ********************************************************************************************/
/*******************************************************************************************
Function: int mediaStatisCallback(void *, int, char**, char**)
Description: callback function that query statistics of media from database
that did not send out

Entry: ......
Return: 0
 ********************************************************************************************/
/****************************************************************************
Function: int sendStatistics(int)
Description: send statistics information of device to service server

Entry: type       0 - cartvu, 1 - media
Return: 0       success
<0      fail
 *****************************************************************************/
#include "CartvuMediaStatis.inc"
/*******************************************************************************************
FileName: sendStatus.inc
It includes 3 functions.
 ********************************************************************************************/
/****************************************************************************
Function: int sendStatus(int)
Description: send device status to service server

Entry: type       0 - cartvu, 1 - media, 2 - localnode
Return: 0       success
<0      fail
 *****************************************************************************/
/*******************************************************************************************
Function: int cartvuStatusCallback(void *, int, char**, char**)
Description: 

Entry: ......
Return: 0
 ********************************************************************************************/
/*******************************************************************************************
Function: int mediaStatusCallback(void *, int, char**, char**)
Description: 

Entry: ......
Return: 0
 ********************************************************************************************/
#include "sendStatus.inc"
/******************************************************************************
FileName: sendAlarmImages.inc
It includes 3 functions.
 *******************************************************************************/
/****************************************************************************
Function: int sendAlarmImages()
Description: send alarm images and information to service server

Entry: None
Return: 0       success
<0      fail
 *****************************************************************************/
/*******************************************************************************************
Function: int imagesInfoCallback(void *, int, char**, char**)
Description: callback function that query records that has transfer image to server but
has not send the infomation out

Entry: ......
Return: 0
 ********************************************************************************************/
/*******************************************************************************************
Function: int imagesCallback(void *, int, char**, char**)
Description: callback function that query records that did not transfer image to server

Entry: ......
Return: 0
 ********************************************************************************************/
#include "sendAlarmImages.inc"

/****************************************************************************
function: int processMediaommand()
Description: Read the media command file that download from the server
and excute command according to command file

Entry: NONE
Return: 0    success
<0   fail
 *****************************************************************************/
#include "processMediaCommand.inc"
/****************************************************************************
FileName: checkLane.inc
It includes 2 functions.
 *****************************************************************************/
/****************************************************************************
function: int checkLaneCallback(.....)
Description: CallBack function from database query of checkLane function

Entry: NONE
Return: 0     success
<0    fail
 *****************************************************************************/
/****************************************************************************
function: int checkLane()
Description: Sending query to service node and getting reply about 
lane information

Entry: NONE
Return: 0     success
<0    fail
 *****************************************************************************/
#include "checkLane.inc"
/****************************************************************************
FileName: cleanDatabase.inc
It includes
 *****************************************************************************/


#define CLEAR_DAYS     30



int imageDelete(void *para, int n_column, char **column_value, char **column_name) {
    char buf_echo[1024];


    sprintf(buf_echo, "/bin/rm %s%s", CARTVU_IMAGE_PATH, column_value[0]);
    if (column_value[0] != NULL) {
        system(buf_echo);
        recordLog(1, 34, strcat(buf_echo, " ok"));
    }

    printf("%s\n", buf_echo);


    return 0;
}



int cleanDatabase() {
    sqlite3 *conn  = NULL;          // sqlite db handler
    char sqlStr[256], sysCmd[256];
    int ret;

    time_t timep;
    struct tm *p;
    struct tm long_time;


    ret = sqlite3_open(DYNAMIC_DATABASE, &conn);              // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(cleanDatabase()) - %s\n", DYNAMIC_DATABASE);
        recordLog(1, 16, "Openning database fail(cleanDatabase) fail!");
        return 0;
    }
    time(&timep);
    timep -= CLEAR_DAYS * 24 * 3600;                                   // 
    p = localtime(&timep);
    long_time.tm_year = p->tm_year;
    long_time.tm_mon = p->tm_mon;
    long_time.tm_mday = p->tm_mday;
    long_time.tm_hour = p->tm_hour;
    long_time.tm_min = p->tm_min;
    long_time.tm_sec = p->tm_sec;

    p = &long_time;



    // delete all the statistics records that is 30 days before from the CartvuStatis table
    sprintf(sqlStr, "delete from CartvuStatis where StatisDate<'%4d%02d%02d' and flag=1",
            1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
    //printf("sql = %s\n", sqlStr);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
    } else {
        recordLog(1, 34, strcat(sqlStr, " ok"));
    }

    // delete all the statistics records that is 30 days before from the MediaStatis table
    sprintf(sqlStr, "delete from MediaStatis where StatisDate<'%4d-%02d-%02d' and flag=1",
            1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
    //printf("sql = %s\n", sqlStr);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
    } else {
        recordLog(1, 34, strcat(sqlStr, " ok"));
    }


    // delete images file in localserver
    sprintf(sqlStr, "select * from Images where AlertTime<'%4d%02d%02d%02d%02d%02d' and flag=2",
            1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
            p->tm_hour, p->tm_min, p->tm_sec);
    ret = sqlite3_exec(conn, sqlStr, imageDelete, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
    } else {
        recordLog(1, 34, strcat(sqlStr, " ok"));
    }
    // if select sql if fail meant no need to clear the images
    if (ret != SQLITE_OK) {
        printf("no need to clear the database\n");
        recordLog(1, 34, "no need to clear the Images table");
    } else {
        // delete all the alarm records that is 30 days before from the images table
        sprintf(sqlStr, "delete from Images where AlertTime<'%4d%02d%02d%02d%02d%02d' and flag=2",
                1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
                p->tm_hour, p->tm_min, p->tm_sec);
        //printf("sql = %s\n", sqlStr);
        ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
        if (ret != SQLITE_OK) {
            printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
            recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
        } else {
            recordLog(1, 34, strcat(sqlStr, " ok"));
        }

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // clear 2 time of 30 days if it is not send to servers node
    time(&timep);
    timep -= CLEAR_DAYS* 2 * 24 * 3600;                                   // 
    p = localtime(&timep);
    long_time.tm_year = p->tm_year;
    long_time.tm_mon = p->tm_mon;
    long_time.tm_mday = p->tm_mday;
    long_time.tm_hour = p->tm_hour;
    long_time.tm_min = p->tm_min;
    long_time.tm_sec = p->tm_sec;

    p = &long_time;



    // delete all the statistics records that is 7 days before from the CartvuStatis table
    sprintf(sqlStr, "delete from CartvuStatis where StatisDate<'%4d%02d%02d'",
            1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
    //printf("sql = %s\n", sqlStr);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
    } else {
        recordLog(1, 34, strcat(sqlStr, " ok"));
    }

    // delete all the statistics records that is 7 days before from the MediaStatis table
    sprintf(sqlStr, "delete from MediaStatis where StatisDate<'%4d-%02d-%02d'",
            1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
    //printf("sql = %s\n", sqlStr);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
    } else {
        recordLog(1, 34, strcat(sqlStr, " ok"));
    }

    // delete all the alarm records that is 7 days before from the images table
    sprintf(sqlStr, "delete from Images where AlertTime<'%4d%02d%02d%02d%02d%02d'",
            1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
            p->tm_hour, p->tm_min, p->tm_sec);
    //printf("sql = %s\n", sqlStr);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(cleanDatabase) - %s\n", sqlStr);
        recordLog(1, 34, strcat(sqlStr, "Execute fail(cleanDatabase)!!"));
    } else {
        recordLog(1, 34, strcat(sqlStr, " ok"));
    }


    // delete the alarm images that is 7 days before from the localserver
    // find /AdNode/cartvu/images/*  -mtime +60 -type f -name '*.jpg' -exec rm -f {} \\;
    sprintf(sysCmd, "find %s* -mtime +%d -type f -name '*.jpg' -exec rm -f {} \\;", CARTVU_IMAGE_PATH, CLEAR_DAYS*2);
    //printf("sysCmd = %s\n", sysCmd);
    system(sysCmd);
    ////////////////////////////////////////////////

    sqlite3_close(conn);

    return 0;
}



#define SELF_IP   "127.0.0.1"
#define SERV_PORT     6767


void deal_kind(struct msg send) {
    int now_lane = send.lane - 1;
    //char *p;
    if (send.kind == 1) {//cartvu
        //p = (char*)&(cartvu_msg.send[now_lane]);
        //strncpy(p, (char*)&send, sizeof(send));
        cartvu_msg.send[now_lane].status = send.status;
        cartvu_msg.send[now_lane].timestamp = send.timestamp;

    } else if (send.kind == 2) {//media
        media_msg.send[now_lane].status = send.status;
        media_msg.send[now_lane].timestamp = send.timestamp;

    } else {
        fprintf(stderr, "receive error from status function send kind is %d\n", send.kind);
    }


}

void update_msg_status() {
    int i = 0;
    unsigned long int now;

    now = (unsigned long int)time((time_t *)NULL);
    for (i = 0; i < max_lane; i++) {
        if (now - cartvu_msg.send[i].timestamp > SET_DISABLE_TIME) {
            cartvu_msg.send[i].status = 255;
        }

        if (now - media_msg.send[i].timestamp > SET_DISABLE_TIME) {
            media_msg.send[i].status = 255;
        }
        printf("cartvu[%02d]:%d\tmeida[%02d]:%d\n", i+1, cartvu_msg.send[i].status, i+1, media_msg.send[i].status);
    }

}


int cartvuStatusTotal(void *para, int n_column, char **column_value, char **column_name) {
    int i;

    i = atoi(column_value[2]) - 1;
    if (i >= 0 && i < LANE_SIZE) {
        //deviceStatus[i] = atoi(column_value[5]);
        //global_cartvu_lane_size++;
        if ((i+1)>global_cartvu_lane_size)
            global_cartvu_lane_size = i+1;
    }

    //printf("i is %d global_cartvu_lane_size is %d\n", i, global_cartvu_lane_size);
    return  0;
}  // end


int mediaStatusTotal(void *para, int n_column, char **column_value, char **column_name) {
    int i;

    i = atoi(column_value[2]) - 1;
    if (i >= 0 && i < LANE_SIZE) {
        //deviceStatus[i] = atoi(column_value[5]);
        //global_media_lane_size++;
        if ((i+1)>global_media_lane_size)
            global_media_lane_size = i+1;
    }


    return  0;
}  // end



void totoal_size_analyze(void) {
    sqlite3 *conn  = NULL;          // sqlite db handler
    char    sqlStr[512];            // sql statement
    int     ret;
    int count_flag = 0;
    int error_count = 0;
    char buf_echo[1024];


    // get the statistics that did not send out
    ret = sqlite3_open(STATIC_DATABASE, &conn);                             // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(sendStatus) - %s", STATIC_DATABASE);
        recordLog(1, 16, "Openning database fail(sendStatus) fail!");
        reset();
        return ;
    }

    count_flag = 0;
    error_count = 0;
    strcpy(sqlStr, "select * from Mac where kind='cartvu'"); // SQL语句
    while (1) {
        ret = sqlite3_exec(conn, sqlStr, cartvuStatusTotal, NULL, NULL);    // 执行
        if (ret != SQLITE_OK) {
            printf("fail : Execute fail(totoal_size_analyze) - %s\n", sqlStr);
            perror("execute mac cartvu sqlite fail");
            if (count_flag == 0) {
                count_flag = 1;
                sprintf(buf_echo, "%s Execute fail(totoal_size_analyze)", sqlStr);
                recordLog(1, 34, buf_echo);
            }
            error_count++;
            if (error_count > 100) {
                reset();
            }
            usleep(200000);
            continue;
        } else {
            sprintf(sqlStr, "trying:%d times total analyze the cartvu lane is ok:lane:%d", error_count, global_cartvu_lane_size);
            recordLog(1, 34, sqlStr);
            break;
        }
    }


    count_flag = 0;
    error_count = 0;
    strcpy(sqlStr, "select * from Mac where kind='media'"); // SQL语句
    while (1) {
        ret = sqlite3_exec(conn, sqlStr, mediaStatusTotal, NULL, NULL);    // 执行
        if (ret != SQLITE_OK) {

            printf("fail : Execute fail(totoal_size_analyze) - %s\n", sqlStr);
            perror("execute mac cartvu sqlite fail");
            if (count_flag == 0) {
                count_flag = 1;
                sprintf(buf_echo, "%s Execute fail(totoal_size_analyze)", sqlStr);
                recordLog(1, 34, buf_echo);
            }
            error_count++;
            if (error_count > 100) {
                reset();
            }
            usleep(200000);
            continue;
        } else {
            sprintf(sqlStr, "trying:%d times total analyze the media lane is ok:lane:%d", error_count, global_media_lane_size);
            recordLog(1, 34, sqlStr);
            break;
        }
    }

    sqlite3_close(conn);  



    if (global_cartvu_lane_size > global_media_lane_size) {
        max_lane = global_cartvu_lane_size;

    } else {
        max_lane = global_media_lane_size;

    }

}



int get_ip(char * ipaddr)
{

    int sock_get_ip;
    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;
    char buf[20] = {0};
    int i = 0;


    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket create fai...GetLocalIp!/n");
        strncpy(ipaddr, "socket create fail...GetLocalIp!", IPADDR_SIZE);
        return -1;
    }

    printf("\n\n~~~~~~~~~~%s~~sock fd = %d~~~~~~~~~~~\n\n", __func__, sock_get_ip);

    memset(&ifr_ip, 0, sizeof(ifr_ip));

    for(i = 0; i < ETH_SIZE; i++) {
        sprintf(ifr_ip.ifr_name, "eth%d", i);
        if(ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
        {
            perror("ioctl fail");
        } else {
            break;
        }
    }
    if (i == ETH_SIZE) {
        strncpy(ipaddr, "ioctl fail to get ip", IPADDR_SIZE);
        close(sock_get_ip);
        return -1;
    }


    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));

    printf("ip:%s \n",ipaddr);
    sprintf(buf, "ip:%s", ipaddr);
    strcpy(ipaddr, buf);
    close(sock_get_ip);



    return 0;
}




void receive_fun(void) {
    int socket_fd;
    struct sockaddr_in sen;
    struct sockaddr_in rec;
    socklen_t sen_len;
    socklen_t rec_len;
    ssize_t recv_rc;
    //"who is localserver"
    char receive_info[50];

    char ip[50] = "ip=192.168.1.200";
    int opt = 1;

    get_ip(ip);
    strcpy(send_info, ip);


    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("create socket error");
        return;
    }

    printf("\n\n~~~~~~~~~~%s~~sock fd = %d~~~~~~~~~~~\n\n", __func__, socket_fd);

    // for receiver broadcast
    rec.sin_family = AF_INET;
    rec.sin_addr.s_addr = htonl(INADDR_ANY); //self addr
    rec.sin_port = htons(SERV_RE_PORT);
    rec_len = sizeof(rec);

    if (bind(socket_fd, (struct sockaddr *)&rec, rec_len)) {
        perror("bind socket error");
        close(socket_fd);
        return;
    }

    // for send broadcast
    sen.sin_family = AF_INET;
    sen.sin_port = htons(SERV_RE_PORT);
    sen.sin_addr.s_addr = inet_addr(UDP_ADDR); //udp addr
    sen_len = sizeof(sen);

    if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) == -1) {
        perror("set socket opt fail");
        close(socket_fd);
        return;
    }



    while(1) {
        recv_rc = recvfrom(socket_fd, receive_info, sizeof(receive_info), 0, (struct sockaddr *)&rec, &rec_len);
        if (recv_rc == -1) {
            perror("receiver from client error");
        } else {
            if (strcmp(receive_info, ASK_SERVER) == 0) {
                printf("someone ask:%s\n", receive_info);
                get_ip(ip);
                strcpy(send_info, ip);
                sendto(socket_fd, send_info, sizeof(send_info), 0, (struct sockaddr *)&sen, sen_len);
            } else {
                printf("myself receive:%s\n", receive_info);
            }
        }
    }
    close(socket_fd);
}


void send_cartvu_media_status(void) {
    int socket_fd;
    int count = 0;
    int i = 0;

    struct msg send;

    ssize_t recv_rc;
    struct sockaddr_in sin;

    socklen_t sin_len;


    /*1      udp */  

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fd < 0) {
        printf("creat socket error");
        return ;  
    }
    printf("\n\n~~~~~~~~~~%s~~sock fd = %d~~~~~~~~~~~\n\n", __func__, socket_fd);

    //int b_on = 1;
    //setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &b_on, sizeof(int));
    /*2*/

    //bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);//自身 ip
    sin.sin_port = htons(SERV_PORT);

    sin_len = sizeof(sin);

    //cin.sin_addr.s_addr
    /*3绑定*/
    if(bind(socket_fd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("bind socket error");
        close(socket_fd);
        return ;
    }






    totoal_size_analyze();


    //initial the status and the timestamp
    for (i = 0; i < LANE_SIZE; i++) {
        cartvu_msg.send[i].status = 255;
        cartvu_msg.send[i].timestamp = 0;
        media_msg.send[i].status = 255;
        media_msg.send[i].timestamp = 0;
    }





    while(1) {
        recv_rc = recvfrom(socket_fd, &send, sizeof(send), 0, (struct sockaddr *)&sin, &sin_len);
        if(recv_rc == -1)
        {
            perror("receive from status(send) failed");
            count++;
            if (count > 1000) {
                fprintf(stderr, "recvfrom is error more than 1000 times\n");
                break;
            }
        } else {
            count = 0;
        }
        //printf("kind:%d, lane:%02d, status:%d, timestamp:%ld\n", send.kind, send.lane, send.status, send.timestamp);
        deal_kind(send);
    }

    close(socket_fd);
        

}



#define TRY_LOGIN_TIME   5
void ftpput_log_alarm_thread(void) {

    int sleepCount = 0;
    int ret = 0;
    char buf[100];
    int try_count = TRY_LOGIN_TIME;



    ftp_thread_food = FTP_THREAD_COUNT;
    printf("make ftpput log alarm thread here\n");
    recordLog(1, 44, "make ftpput log alarm thread here");
    // send alarm images to service node
    if (isLogin) {
        //ftpput the today's file  no matter where it record
        sprintf(buf, "cp  %s %sLocalServer-NW.log", filename, RUNNING_LOG_DIR);
        MY_SYSTEM(buf);
        ////////////////////////// ftpput the yesterday file 
        ftpFlag = 1;
        dealLog(ftpUser, ftpPassword, ip, 1);
        ftpFlag = 0;
    }



    /****************************************************************************
      12. Clean up database - LocalNode.db
      CartvuStatis MediaStatis and Images.
      Delete records that were generated before one week.
     ****************************************************************************/
    pthread_mutex_lock (&work_mutex);
    while(try_count--) {
        ret = loginServer();
        if (ret == 0) {
            isLogin = 1;
            break;
        } else {
            isLogin = 0;
        }

    }

    sprintf(buf, "trying login  in ftpput  thread is %d times", TRY_LOGIN_TIME - try_count);
    printf("%s\n", buf);
    recordLog(1, 47, buf);
    pthread_mutex_unlock (&work_mutex);


    if (isLogin) {
        recordLog(1, 12, "Sending sendAlarmImages here");
        if (sendAlarmImages() < 0) {
            printf("sending alarmimages  fail\n");
            //  recordLog(1, 12, "Sending sendAlarmImages is failed!");
        } else {
            printf("sending alarmimages  ok\n");
        }


        if (cleanDatabase() == 0) {
            printf("Clean up database is ok!\n");
            recordLog(1, 44, "Clean up database is ok!");
        } else {
            printf("Clean up database failed!\n");
            recordLog(1, 44, "Clean up database failed!");
        }
    }


    try_count = TRY_LOGIN_TIME*2;
    ret = loginServer();
    while(try_count--) {
        ret = loginServer();
        if (ret == 0) {
            isLogin = 1;
            break;
        } else {
            isLogin = 0;
        }

    }


    // ftpget command
    if (isLogin) {
        printf("first get command.cfg here\n");
        updateCommand();
        use_main();
    }
    sleepCount=0;




    while(1) {

        ftp_thread_food = FTP_THREAD_COUNT;
        if (isLogin) {
            if ((sleepCount % 2) == 0) {
                if (sendAlarmImages() < 0) {
                    printf("sending alarmimages  fail\n");
                    //  recordLog(1, 12, "Sending sendAlarmImages is failed!");
                } else {
                    printf("sending alarmimages  ok\n");
                }

            }
        }
        sleepCount++;
        // every half an hour
        if (sleepCount == 360) {
            if (isLogin) {
                download_all_software();
                updateCommand();

                use_main();

            }
            sleepCount=0;
        }
        sleep(5);



    }

}


/********************************************************************
Function: void watchDog()
Description: Watchdog thread
 *********************************************************************/
void watchDog(void) {
    int ret;
    int time_out;
    int count = 0;

    watchFd = open("/dev/watchdog", O_RDWR);
    if (watchFd < 0) {
        perror("Openning watchdog failed.");
        exit(1);
    }

    ret = ioctl(watchFd, WDIOC_GETTIMEOUT, &time_out);
    if (ret < 0) {
        perror("Reading watchdog time out failed.");
        exit(1);
    }
    printf("default time out is %d   second\n", time_out); 

    time_out = WATCHDOG_TIME_OUT;
    ret = ioctl(watchFd, WDIOC_SETTIMEOUT, &time_out);
    if (ret < 0) {
        printf("Time out = %d\n", time_out);
        perror("Setting watchdog time out failed");
        exit(1);
    }

    printf("\n\nNow watchdog time out is %d seconds. \n", time_out); 

    while ((dogFood > 0) && (ftp_thread_food > 0)) {
        //sleep(WATCHDOG_TIME_OUT * 0.9);
        sleep(1);
        count++;
        if (count >= WATCHDOG_TIME_OUT * 0.9) {
            count = 0;
            dogFood--;


            ftp_thread_food--;

            // deal ftpput and ftpget when the thread is dead
            ftpDogFood--;
            if (ftpDogFood < 2 && ftpFlag == 1) {
                system("killall ftpput");
                system("killall ftpget");
                printf("killall ftpput and ftpget\n");
                recordLog(1,1,"killall ftpput and ftpget here");
            }
            if (ftpDogFood == 0) {
                ftpDogFood = WATCHDOG_COUNT;
            }


            ret = ioctl(watchFd, WDIOC_GETTIMEOUT, &time_out);
            if (ret < 0) {
                perror("read watchdog time out error");
                exit(1);
            }
            printf("timeout = %d sec\tdogFood = %d  ftpDogFood = %d\n", time_out, dogFood, ftpDogFood); 

            ret = ioctl(watchFd, WDIOC_KEEPALIVE, NULL);
            if (ret < 0) {
                perror("Feeding dog failed.");
            } 


        }
        //  check reset time
        if (count%2)
            checkReset();


    }

    printf("Watchdog thread exit. dogFood = %d.\n", dogFood);
    recordLog(3, 29,"Watchdog thread exit");

    return;
}
/****************************************************************************
  Function name: int updateStatus()
Description: update the status in table Mac that records did not update 
over 30 seconds

Entry: None
Return: Allaways 0
 *****************************************************************************/
int updateStatus() {
    sqlite3 *conn  = NULL;          // sqlite db handler
    char sqlStr[256];
    int ret;

    ret = sqlite3_open(STATIC_DATABASE, &conn);          // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(updateStatus()) - %s", STATIC_DATABASE);
        return 0;
    }

    sprintf(sqlStr, "update Mac set status=255 where (strftime('%s','now')-strftime('%s',StatusTime))>%d", "%s", "%s", SET_DISABLE_TIME);
    //printf("sql = %s\n", sqlStr);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(updateStatus()) - %s\n", sqlStr);
        printf("*************************************************************\n");
        sleep(3);
    }

    sqlite3_close(conn);

    return 0;

}




/****************************************************************************
  Function name: int updateUserOnline()
Description: delete the machine ip when is not online  in table UserOnline 
every 60 seconds

Entry: None
Return: Allaways 0
 *****************************************************************************/
int updateUserOnline() {
    sqlite3 *conn  = NULL;          // sqlite db handler
    char sqlStr[256];
    int ret;

    ret = sqlite3_open(DYNAMIC_DATABASE, &conn);          // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(updateUserOnline()) - %s", DYNAMIC_DATABASE);
        return 0;
    }

    sprintf(sqlStr, "delete from useronline where (%ld - TimeStamp) > %u", 
            (unsigned long int)time((time_t *)NULL), 60);
    ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  // 执行
    if (ret != SQLITE_OK) {
        printf("fail : Execute fail(updateUserOnline()) - %s\n", sqlStr);
        printf("*************************************************************\n");
        sleep(3);
    }

    sqlite3_close(conn);

    return 0;
}





int check_shell_error(char *record_file) {
    FILE *fp;
    int error_flag;
    char buf_echo[BUF_ECHO_SIZE] = {0};


    fp = fopen(record_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "open %s fail\n", record_file);
        return -1;
    }

    error_flag = 0;
    while (fgets(buf_echo, BUF_ECHO_SIZE, fp)) {
        if(strlen(buf_echo) == 0) {
            break;
        }
        printf("error:%s\n", buf_echo);
        recordLog(1, 42, buf_echo);
        error_flag = 1;
    }
    fclose(fp);
    if (error_flag == 0) {
        printf("execute  shell is ok\n");
        return 0;
    } else {
        fprintf(stderr, "execute  shell is fail\n");
        return -1;
    }

}

int showCallback(void *para, int n_column, char **column_value, char **column_name) {
    strcpy(software[softId].key, column_value[0]);           // software name
    sprintf(software[softId].value, "%d.%02d", atoi(column_value[2]), atoi(column_value[3]));   // software version
    //strcpy(software[softId].path, column_value[5]);          // software path
    softId++;
    return  0;

}


int executeShow(char *sqlStr, int type) {
    sqlite3 *conn  = NULL;          // sqlite db handler
    int     ret;
    int count;
    char buf_echo[1024] = {0};
    int i;

    softId = 0;
    for (i = 0; i < SOFTWARE_SIZE; i++) {
        software[i].key[0] = 0;
    }  // end of for

    // get all the software of local node from database
    ret = sqlite3_open(VERSION_DATABASE, &conn);                           // 打开数据库
    if (ret != SQLITE_OK) {
        printf("fail : Openning database fail(downloadSoftware) - %s\n", VERSION_DATABASE);
        recordLog(1, 16, "Openning VERSION_DATABASE database fail(executeShow)");
        return -1;
    }



    count = 0;
    while (1) {
        count++;
        ret = sqlite3_exec(conn, sqlStr, showCallback, NULL, NULL);              // 执行
        if (ret != SQLITE_OK) {
            if (count > TRY_COUNT) {
                printf("fail :try %d times in %s\n", count, sqlStr);
                printf("fail : Execute fail(executeShow) - %s\n", sqlStr);
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

    switch (type) {
        case 0:
            sprintf(buf_echo, "\nCartvu Version:\n");
            break;
        case 1:
            sprintf(buf_echo, "\nMedia Version:\n");
            break;
        case 2:
            sprintf(buf_echo, "\nLocalServer Version:\n");
            break;
        default:
            sprintf(buf_echo, "\nError:\n");
            break;  
    }
    for (i = 0; i < SOFTWARE_SIZE; i++) {
        if (software[i].key[0] == 0) break;

        //printf("software[%02d]:%s\t%s\n", i, software[i].key, software[i].value);
        sprintf(buf_echo, "%ssoftware[%02d]:%25s:\t%s\n",buf_echo, i, software[i].key, software[i].value);      
    }
    printf("%25s\n", buf_echo);
    recordLog(1, 7, buf_echo);
    return 0;
}


void showVersion() {
    int i = 0;
    char sqlStr[1024];

    for (i = 0; i < 3; i++) {
        sprintf(sqlStr, "select * from Version where location=%d", i);      // SQL语句
        executeShow(sqlStr, i);
    }

}

#define ETH0_SETTING   "/etc/eth0-setting"
void reset_network() {
    FILE *fp;
    char buf_echo[BUF_ECHO_SIZE];
    paraStruc network_info;
    char buf[1024];

    char localserver_ip[20] = {0};
    char gateway[20] = {0};
    char mask[20] = {0};



    fp = fopen(ETH0_SETTING, "r");
    if (fp == NULL) {
        perror("open eth0-setting fail");
        return;
    }

    while (fgets(buf_echo, BUF_ECHO_SIZE, fp) != NULL) {
        if (strlen(buf_echo) == 0) break;
        trim(buf_echo);
        strlower(buf_echo);
        //printf("~~~~~~~~%s~~~\n", buf_echo);
        stringSeparate(buf_echo, &network_info, 1);
        //printf("key:%s   value:%s\n", network_info.key, network_info.value);
        if (strncmp(network_info.key, "ip", 2) == 0) {
            strcpy(localserver_ip, network_info.value);
        } else if (strncmp(network_info.key, "gateway", 7) == 0) {
            strcpy(gateway, network_info.value);
        } else if (strncmp(network_info.key, "mask", 4) == 0) {
            strcpy(mask, network_info.value);
        }

    }

    // ifconfig eth0 down
    // ifconfig eth0 hw ether 00:00:11:70:00:09
    // ifconfig eth0 192.168.1.223 netmask 255.255.255.0 up
    // route add default gw 192.168.1.3
    //  echo nameserver =8.8.8.8 > /etc/resolv.conf
    if ((localserver_ip[0] != 0) && (gateway[0] != 0) && (mask[0] != 0)) {
        system("ifconfig eth0 down");
        sprintf(buf, "ifconfig eth0 %s netmask %s up", localserver_ip, mask);
        system(buf);
        printf("%s\n", buf);
        sprintf(buf, "route add default gw %s", gateway);
        system(buf);
        printf("%s\n", buf);
    }

    fclose(fp);

    recordLog(1,2,"network error here and reset the network");

}


void first_read_network() {
    FILE *fp;
    char buf_echo[BUF_ECHO_SIZE];
    paraStruc network_info;



    fp = fopen(ETH0_SETTING, "r");
    if (fp == NULL) {
        perror("open eth0-setting fail");
        return;
    }

    while (fgets(buf_echo, BUF_ECHO_SIZE, fp) != NULL) {
        if (strlen(buf_echo) == 0) break;
        trim(buf_echo);
        strlower(buf_echo);
        //printf("~~~~~~~~%s~~~\n", buf_echo);
        recordLog(1, 2, buf_echo);
        stringSeparate(buf_echo, &network_info, 1);
        printf("key:%s   value:%s\n", network_info.key, network_info.value);

    }
    fclose(fp);

}

void update_statis() {
    if (isLogin) {
        if (sendStatistics(1) == 0) {                 // media statistics
            printf("Sending MEDIA statistics is ok!\n");
            recordLog(1, 11, "Sending MEDIA statistics is ok!");
        } else {
            printf("Sending MEDIA statistics failed!\n");
            recordLog(1, 11, "Sending MEDIA statistics is ok!");
        }
        if (sendStatistics(0) == 0) {                 // cartvu statistics
            printf("Sending CARTVU statistics is ok!\n");
            recordLog(1, 11, "Sending CARTVU statistics is ok!");
        } else {
            printf("Sending CARTVU statistics failed!\n");
            recordLog(1, 11, "Sending CARTVU statistics is failed!");
        }
    }

}





void get_stamp(char *file, char *rec_time) {

    FILE *fp;
    char buf[256];
    char *p;
    char year[10];
    char month[10];
    char day[10];
    char hour[10];
    char min[10];
    char sec[10];

    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("open %s error\n", file);
        return ;
    }


    if (fgets(buf, 256, fp) != NULL) {
        strlower(buf);
        trim(buf);

        p = buf;
        bzero(year, sizeof(year));
        strncpy(year, p, 4);

        p += 4;
        bzero(month, sizeof(month));
        strncpy(month, p, 2);

        p += 2;
        bzero(day, sizeof(day));
        strncpy(day, p, 2);

        p += 2;
        bzero(hour, sizeof(hour));
        strncpy(hour, p, 2);

        p += 2;
        bzero(min, sizeof(min));
        strncpy(min, p, 2);

        p += 2;
        bzero(sec, sizeof(sec));
        strncpy(sec, p, 2);
        sprintf(rec_time, "%4s-%2s-%2s %2s:%2s:%2s", year, month, day, hour, min, sec);
        printf("%s\n", rec_time);

    }
    fclose(fp);

}

void update_adStatus(char *before, char *now) {





    char lastTime[50];
    char nowTime[50];
    char updateTime[50];

    time_t timeStamp;


    sqlite3 *conn = NULL;
    char sqlStr[256];
    int ret;

    int count = 0;

    int ok_flag = 0;

    struct tm *p;



    bzero(lastTime, sizeof(lastTime));
    bzero(nowTime, sizeof(nowTime));

    get_stamp(before, lastTime);

    get_stamp(now, nowTime);




    if ((lastTime[0] != '\0') && (nowTime[0] != '\0')) {

        ret = sqlite3_open(SEND_INFO_DATABASE, &conn);
        if (ret != SQLITE_OK) {
            printf("open %s fail", SEND_INFO_DATABASE);
            return;
        }


        sprintf(sqlStr, "delete from commandInfo");

        count = 0;
        while (1) {
            count++;
            ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);                // 执行sql语句
            if (ret != SQLITE_OK) {
                if (count > TRY_COUNT) {
                    printf("fail :try %d times in %s\n", count, sqlStr);
                    //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                    //system(buf_echo);
                    ok_flag = 0;
                    recordLog(11, 2,strcat(sqlStr, "  try 101 times fail update_adStatus()"));
                    
                    break;
                }
            } else {
                printf("ok :try %d times in %s\n", count, sqlStr);
                ok_flag = 1;

                recordLog(11, 2,strcat(sqlStr, "  ok update_adStatus()"));
                //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                //system(buf_echo);
                break;
            }
        }


        if (ok_flag == 1) {


            time(&timeStamp);
            p = localtime(&timeStamp);


            sprintf(updateTime, "%04d-%02d-%02d %02d:%02d:%02d", 
                    p->tm_year + 1900, p->tm_mon + 1,
                    p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

            sprintf(sqlStr, "insert into commandInfo values(-1, '%s', '%s', '%s', '%s')",
                    "command.sys", lastTime, nowTime, updateTime);

            count = 0;
            while (1) {
                count++;
                ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);                // 执行sql语句
                if (ret != SQLITE_OK) {
                    if (count > TRY_COUNT) {
                        printf("fail :try %d times in %s\n", count, sqlStr);
                        recordLog(11, 2,strcat(sqlStr, "  try 101 times fail2 update_adStatus()"));
                        //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                        //system(buf_echo);
                        break;
                    }
                } else {
                    printf("ok :try %d times in %s\n", count, sqlStr);
                    recordLog(11, 2,strcat(sqlStr, "  ok 2 update_adStatus()"));
                    //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                    //system(buf_echo);
                    break;
                }
            }



        }


        sprintf(sqlStr, "delete from adStatus");
        count = 0;
        while (1) {
            count++;
            ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);                // 执行sql语句
            if (ret != SQLITE_OK) {
                if (count > TRY_COUNT) {
                    printf("fail :try %d times in %s\n", count, sqlStr);
                    recordLog(11, 2,strcat(sqlStr, "  try 101 times fail3 update_adStatus()"));
                    //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                    //system(buf_echo);
                    break;
                }
            } else {
                printf("ok :try %d times in %s\n", count, sqlStr);
                recordLog(11, 2,strcat(sqlStr, "  ok  3 update_adStatu()"));
                //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                //system(buf_echo);
                break;
            }
        }






        sqlite3_close(conn);







    }

}


/*
Licence=1

DownloadMovie=01
DownloadMovie=02
DownloadMovie=03
DownloadMovie=04
DownloadImage=05
DownloadMovie=06
DownloadImage=07

*/


void get_num_command(char *file, int *num) {

    FILE *fp;
    char buf[256];

    int i = 0;

    

    char *p;

    paraStruc info;

    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("open %s error\n", file);
        return ;
    }

    i = 0;
    while (fgets(buf, 256, fp) != NULL) {
        
        strlower(buf);
        trim(buf);
        p = index(buf, '=');
        if (p != NULL) {
              stringSeparate(buf, &info, 1);
              if ((strcmp(info.key, "downloadmovie") == 0 ) || (strcmp(info.key, "downloadimage") == 0) ){
                    num[i++] = atoi(info.value);
              } 
              
        }
        
    }

    fclose(fp);


}

void update_db(int num, char *name) {
    sqlite3 *conn = NULL;
    char sqlStr[256];
    int ret;

    char updateTime[50];

    struct tm *p;

    time_t timeStamp;
    int count;


    time(&timeStamp);
    p = localtime(&timeStamp);
    
    
    sprintf(updateTime, "%04d-%02d-%02d %02d:%02d:%02d", 
            p->tm_year + 1900, p->tm_mon + 1,
            p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);


    ret = sqlite3_open(SEND_INFO_DATABASE, &conn);
    if (ret != SQLITE_OK) {
        printf("open %s fail", SEND_INFO_DATABASE);
        return;
    }


    sprintf(sqlStr, "insert into commandInfo values(%d, '%s', '%s', '%s', '%s')", num, name, updateTime, updateTime, updateTime);

    count = 0;
    while (1) {
        count++;
        ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);                // 执行sql语句
        if (ret != SQLITE_OK) {
            if (count > TRY_COUNT) {
                printf("fail :try %d times in %s\n", count, sqlStr);
                recordLog(11, 2,strcat(sqlStr, "  fail 101 times"));
                //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
                //system(buf_echo);
                break;
            }
        } else {
            printf("ok :try %d times in %s\n", count, sqlStr);
            recordLog(11, 2,strcat(sqlStr, "  ok"));
            //sprintf(buf_echo, "echo 'try %d times in %s' >/dev/ttySAC0", count, sqlStr);
            //system(buf_echo);
            break;
        }
    }
    
    sqlite3_close(conn);

}

void update_sqlite_adStatus(int num) {


    char cfg_file[50];

    FILE *fp;
    char buf[256];
    char *p;
    paraStruc info;


    sprintf(cfg_file, "%s%02d%s", MEDIA_CONFIG_PATH, num, ".cfg");

    fp = fopen(cfg_file, "r");
    if (fp == NULL) {
        printf("open %s error\n", cfg_file);
        return ;
    }
    while (fgets(buf, 256, fp) != NULL) {
        strlower(buf);
        trim(buf);
        p = index(buf, '=');
        if (p != NULL) {
              stringSeparate(buf, &info, 1);
              if (strcmp(info.key, "id") == 0){
                    update_db(num, info.value);
                    break;
              } 
              
        }

    }

    fclose(fp);


}

void compare_two_file_cfg(char *ok_file, char *fail_file) {


    int i = 0;
    int j = 0;
    int ok_num[50];

    int fail_num[50];

    bzero(ok_num, sizeof(ok_num));
    bzero(fail_num, sizeof(fail_num));
    get_num_command(ok_file, ok_num);
    get_num_command(fail_file, fail_num);

    i = 0;
    while(ok_num[i] != 0) {
        j = 0;
        while(fail_num[j] != 0) {
            if (ok_num[i] == fail_num[j]) {
                ok_num[i] = -1;
                break;
            }
            j++;
        }

        i++;
    }

    printf("ok num is below\n");
    i = 0;
    while(ok_num[i] != 0) {
        printf("%d\n", ok_num[i]);


        if (ok_num[i] != -1) {
            update_sqlite_adStatus(ok_num[i]);
        }

        i++;
    }



    

    

}


void updateCommand() {
    char sysCmd[1024] = {0};
    int ret = 0;
    int try_count = 5;
    char down_command[256];
    char fail_command[256];
    char another_command_fail[256];
    char now_command[256];

    sprintf(down_command, "%s%s", TEMP_PATH, MEDIA_COMMAND_FILE);
    sprintf(fail_command, "%s%s", MEDIA_COMMAND_PATH,COMMAMD_FAIL);

    sprintf(now_command, "%s%s", MEDIA_COMMAND_PATH, MEDIA_COMMAND_FILE);

    // ****MC command file
    //sprintf(sysCmd, "./getfile.sh %s %s %s %sCommand/%s/ command.cfg",                        // MEDIA的命令文件
    //                ip, ftpUser, ftpPassword, REMOTE_FTP_MEDIA_PATH, m1m2m3);
    sprintf(sysCmd, "ftpget -u %s -p %s %s %scommand.cfg %sCommand/%s/command.cfg 2>%s",
            ftpUser, ftpPassword, ip, TEMP_PATH, REMOTE_FTP_MEDIA_PATH, m1m2m3, RECORD_ERROR);
    printf("sysCmd = %s\n", sysCmd);
    try_count = 5;
    while (try_count--) {
        ftpFlag = 1;
        ftpDogFood = WATCHDOG_COUNT;
        system(sysCmd);
        if (check_shell_error(RECORD_ERROR) < 0) {
            recordLog(1, 14, "download command.cfg fail!");
        } else {
            recordLog(1, 14, "download command.cfg is ok!");
            try_count = 0;
        }
        ftpFlag = 0;
    }


    ret = updateMediaCmd();
    if (ret < -1) {
        recordLog(1, 46, "the downloaded command format is not right");
    } else if (ret == 100) {//need to update the command file
        recordLog(1, 14, "need to update the command.cfg");
        processMediaCommand(down_command, fail_command);

        //  以前的command.sys   现在的command.sys
        update_adStatus(now_command, down_command);

        sprintf(sysCmd, "mv %s %s%s", 
                down_command,
                MEDIA_COMMAND_PATH, MEDIA_COMMAND_FILE);
        system(sysCmd);

        compare_two_file_cfg(now_command, fail_command);


        sprintf(sysCmd, "chmod 777 %s%s", MEDIA_COMMAND_PATH, MEDIA_COMMAND_FILE);
        printf("%s\n", sysCmd);
        recordLog(1, 14, sysCmd);
        system(sysCmd);

    } else if (ret == 200) {// no need to update the command file

        sprintf(another_command_fail, "%s%s", MEDIA_COMMAND_PATH,COMMAMD_FAIL2);

        if (access(fail_command, F_OK) == 0) { 
            recordLog(1, 14, "need to execute command.fail here");
            processFailCommand(fail_command, another_command_fail);

            compare_two_file_cfg(now_command, fail_command);

        } else {
            recordLog(1, 14, "no command.fail here");

        }

        post_ad_ok("-1", POST_AD_OK_START);

    }


}








unsigned  long int read_self_size(char *filename) {
    FILE *fp;
    unsigned long int sum;
    char oneChar;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("open self main fail");
        return -1;
    }


    sum = 0;

    while (fread(&oneChar, 1, 1, fp) > 0) {
        sum += 1;
    }
    fclose(fp);

    return sum;

}



void download_all_software() {
    int ret = 0;


    ret = downloadSoftware(2);
    if (ret == 0) {
        printf("Downloading LocalNode Software is ok.\n");
        recordLog(1, 4, "Downloading LocalNode Software is ok");
    } else {
        printf("Downloading LocalNode Software failed.\n");
        recordLog(1, 4, "Downloading LocalNode Software is ok");
    } // end of if


    // ***cartvu software
    ret = downloadSoftware(0);
    if (ret == 0) {
        printf("Downloading Cartvu Software is ok.\n");
        recordLog(1, 4, "Downloading Cartvu Software is ok");
    } else {
        printf("Downloading Cartvu Software failed.\n");
        recordLog(1, 4, "Downloading Cartvu Software failed");
    } // end of if


    // ***media software
    ret = downloadSoftware(1);
    if (ret == 0) {
        printf("Downloading Media Software is ok.\n");
        recordLog(1, 4, "Downloading Media Software is ok");
    } else {
        printf("Downloading Media Software failed.\n");
        recordLog(1, 4, "Downloading Media Software failed");
    } // end of if


}


void func(int signal) {
    printf("signal is %d\n", signal);

    printf("signale error here and exit here\n");
    exit(1);

}

/****************************************************************************
  function name: int main()
description: main program

entry: NONE
return: 0  -- normal exit
<0 -- exception exit
 ***************************************************************************/
int main(int argc, char **argv) {
    //  FILE *fp;
    pthread_t watchdogId;
    pthread_t cartvu_media_status;
    pthread_t log_alarm_put;

    pthread_t localserver_ip;


    char sysCmd[1024];
    int ret, iii;
    char buf_echo[1024];
    time_t timep;
    struct tm *p;

    time(&timep);
    p = localtime(&timep);
    int login_count = 0;
    int reset_count = 0;


    signal(SIGHUP,func);
    signal(SIGINT,func);
    signal(SIGQUIT,func);
    signal(SIGILL,func);
    signal(SIGABRT,func);
    signal(SIGFPE,func);
    signal(SIGKILL,func);
    signal(SIGSEGV,func);
    signal(SIGPIPE,func);
    //signal(SIGALRM,func);
    signal(SIGTERM,func);

    //signal(SIGCHLD,func);
    signal(SIGCONT,func);
    signal(SIGSTOP,func);
    signal(SIGTSTP,func);
    signal(SIGTTIN,func);
    signal(SIGTTOU,func);

    ret = pthread_mutex_init(&work_mutex, NULL); //init mutex 初始化互斥锁
    if (ret != 0)
    {
        perror("work_mutex  initialization failed");
        return -1;
    } else {
        printf("work_mutex initialization ok here\n");
    }

    ret = pthread_mutex_init(&post_request, NULL); //init mutex 初始化互斥锁
    if (ret != 0)
    {
        perror("post_request  initialization failed");
        return -1;
    } else {
        printf("post_request initialization ok here\n");
    }

    ret = pthread_mutex_init(&send_mutex, NULL); //init mutex 初始化互斥锁
    if (ret != 0)
    {
        perror("send_mutex  initialization failed");
        return -1;
    } else {
        printf("send_mutex initialization ok here\n");
    }



    alert_head = (struct alert_image_more  *)malloc(sizeof(struct alert_image_more));
    if (alert_head == NULL) {
        perror("malloc fail");
        return -1;
    }
    alert_head->next = alert_head->front = alert_head;



    printf("\n************Main Program of Local Node %s*********\n\n", VERSION);


    sprintf(buf_echo, "chmod 777 %s -R", ROOT_DIR);
    printf("%s\n", buf_echo);
    system(buf_echo);

    sprintf(buf_echo, "mkdir %s %s -p -m 777", CARTVU_MEDIA_LOG, RUNNING_LOG_DIR);
    system(buf_echo);
    dealLog(0, 0, 0, 0);

    sprintf(buf_echo, "echo '\n************Main Program of Local Node %s*********\n\n'>>%s", VERSION, filename);
    system(buf_echo);

    sprintf(buf_echo, "df >>%s", filename);
    system(buf_echo);


    recordLog(1, 1, "reboot time here");


    sprintf(buf_echo, "%s size is %ld", argv[0], read_self_size(argv[0]));
    printf("%s\n", buf_echo);
    recordLog(1, 1, buf_echo);

    //system("date -R >>/etc/rebootTime");
    sprintf(buf_echo, "echo '%04d-%02d-%02d %02d:%02d:%02d VERSION:%s' >>/etc/rebootTime", 
            p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, VERSION);
    system(buf_echo);
    if (p->tm_mday == 1) {
        system("rm /etc/rebootTime");
    }



    first_read_network();
    //reset_network();
    sleep(2);


    /***********************************************************
      Start watchdog thread
     ************************************************************/
    ret = pthread_create(&watchdogId, NULL, (void *)watchDog, NULL);
    if (ret != 0) {
        printf("Create WATCHDOG thread failed.\n");
    }

    ret = pthread_create(&cartvu_media_status, NULL, (void *)send_cartvu_media_status, NULL);
    if (ret != 0) {
        printf("Create send_cartvu_media_status thread failed.\n");
    }


    ret = pthread_create(&localserver_ip, NULL, (void *)receive_fun, NULL);
    if (ret != 0) {
        printf("Create receive_fun thread failed.\n");
    }



    while(max_lane == 0); 
    printf("\n~~~~~~~calculate the lane size is ok!~~max_lane:%d~~~~~~\n", max_lane);
    /***********************************************************
      1. Open and read the config file
     ************************************************************/
    if (readConfig() == 0) {
        printf("Reading config file is ok!\n");
        recordLog(1, 39, "read config is ok!");
    } else {
        printf("Reading config file fail!\n");
        strcpy(configDate, "00000000000000");
        strcpy(serverDomain1, "server1.cartvu-media.com");    
        strcpy(serverDomain2, "server1.cartvu-media.com");
        strcpy(resetTime, "03:00");
        recordLog(1, 39, "read config is fail!");
        //return -1;
    }  // end of if
    //printf("configDate=%s\nserverDomain1=%s\nserverDomain2=%s\n", configDate, serverDomain1, serverDomain2);
    //printf("openningTime=%s\ncloseTime=%s\nresetTime=%s\ntimeZone=%d\n", openningTime, closeTime, resetTime, timeZone);

    /***********************************************************
      3. get MAC address
     ************************************************************/
    dogFood = WATCHDOG_COUNT;
    if (getMac(mac) == 0) {
        printf("Getting MAC ok!\n");
        mac[12]=0;
        recordLog(1, 40, "Getting MAC ok!");
    } else {
        printf("Getting MAC fail!\n");
        recordLog(1, 40, "Getting MAC fail!");
        return -3;
    } // end of if



    /****************************************************************************
      4. Generate a new ftp password for all the cartvus and medias
     *****************************************************************************/
    dogFood = WATCHDOG_COUNT;
    ret = genFtpPasswd();
    if (ret != 0) {                      // password store in /AdNode/config/.ftppasswd
        printf("Fail to generate a new FTP password! -- %d\n", ret);
        recordLog(1, 43, "Fail to generate a new FTP password!");
    }



    /***********************************************************
      2. get ip addressof service server via domain
     ************************************************************/
    dogFood = WATCHDOG_COUNT;
    ret = 1;
    iii = 0;
    while (ret != 0) {
        iii++;
        ret = getIpViaDomain(serverDomain1, ip);
        if (ret == 0) {
            printf("Getting IP ok! -- %s\n", ip);
            recordLog(1, 40, "Getting serverDomain1 IP ok!");
        } else {
            printf("Getting IP fail!\t%d times.\n", iii);
            sleep(1);
            //if (iii == 10) break;

            if (iii >= 5) {
                strcpy(ip, "10.0.0.1");
                iii = 0;
                recordLog(1, 40, "Getting serverDomain1 IP fail!");
                break;
            }

        }
        dogFood = WATCHDOG_COUNT;
    }  // end of if






    /**********************************************************
      5. login the service node and get the ftp password
     ***********************************************************/
    if (strlen(ip) >= 7) {
        ret = 1; iii = 0;
        while (ret != 0) {
            iii++;
            ret = loginServer();
            if (ret == 0) {
                //printf("Login server ok! Password=%s\n", ftpPassword);
                printf("Login server ok! Password=**********\n");
                recordLog(1, 1, "Login server ok! Password=**********");
                isLogin = 1;
                break;
            } else {
                printf("Login server fail! -- %d\n", iii);
                if (iii == 1) {
                    recordLog(1, 1, "Login server fail!");
                }
                //if (iii == 30) break;
                if (iii == 5) {
                    recordLog(1, 1, "Login server fail! 10 times");
                    iii = 0;
                    break;
                }
            }
            sleep(5);
            dogFood = WATCHDOG_COUNT;
        } // end of if
    } else {
        printf("Can not login server. No internet.\n");
        recordLog(1, 1, "Can not login server. No internet");
    }
    /**************************************************************************************************
      6. get the new config file from the service node via ftp and decide if update the config file
     ***************************************************************************************************/
    dogFood = WATCHDOG_COUNT;
    if (isLogin) {

        //sprintf(sysCmd, "./getfile.sh %s %s %s %sConfig/%s/ config.sys",
        //                ip, ftpUser, ftpPassword, REMOTE_FTP_LOCALNODE_PATH, m1m2m3);
        sprintf(sysCmd, "ftpget -u %s -p %s %s %s %sConfig/%s/config.sys 2>%s",
                ftpUser, ftpPassword, ip, TEMP_CONFIG_FILE, REMOTE_FTP_LOCALNODE_PATH, m1m2m3, RECORD_ERROR);
        //              ip  ftpuser  ftppassword  serviceNodePath                   filename
        printf("sysCmd = %s\n", sysCmd);

        ftpDogFood = WATCHDOG_COUNT/3;
        ftpFlag = 1;


        system(sysCmd);
        if (check_shell_error(RECORD_ERROR) < 0) {
            sprintf(buf_echo, "%s execute fail", sysCmd); 
            recordLog(1, 47, buf_echo);
        } else {
            sprintf(buf_echo, "%s execute ok!", sysCmd); 
            recordLog(1, 47, buf_echo);

            dogFood = WATCHDOG_COUNT;
            ret = updateConfig();
            if (ret == 0) {
                printf("Getting config file of localnode is ok.\n");
                recordLog(1, 4, "Getting config file of localnode is ok");
            } else {
                printf("Getting config file of localnode Fail!\n");
                recordLog(1, 4, "Getting config file of localnode is fail");
            } // end of if
        }


        ftpFlag = 0;



    }










    /******************************************************************
      7. update MC config file and CC config file
     *******************************************************************/
    dogFood = WATCHDOG_COUNT;
    if (isLogin) {
        // ****MC config file
        //sprintf(sysCmd, "./getfile.sh %s %s %s %sConfig/%s/ config.sys",                          // MEDIA的配置文件
        //                ip, ftpUser, ftpPassword, REMOTE_FTP_MEDIA_PATH, m1m2m3);
        sprintf(sysCmd, "ftpget -u %s -p %s %s %s %sConfig/%s/config.sys 2>%s",
                ftpUser, ftpPassword, ip, TEMP_CONFIG_FILE, REMOTE_FTP_MEDIA_PATH, m1m2m3, RECORD_ERROR);
        printf("sysCmd = %s\n", sysCmd);

        ftpDogFood = WATCHDOG_COUNT/3;
        ftpFlag = 1;
        system(sysCmd);



        if (check_shell_error(RECORD_ERROR) < 0) {
            sprintf(buf_echo, "%s execute fail", sysCmd); 
            recordLog(1, 47, buf_echo);

        } else {
            sprintf(buf_echo, "%s execute ok!", sysCmd); 
            recordLog(1, 47, buf_echo);

            dogFood = WATCHDOG_COUNT;
            ret = updateMediaConfig();
            if (ret == 0) {
                printf("Getting config file of Media is ok.\n");
                recordLog(1, 4, "Getting config file of Media is ok");
            } else {
                printf("Getting config file of Media Fail!\n");
                recordLog(1, 4, "Getting config file of Media Fail!");
            } // end of if

        }





        ftpFlag = 0;


        // ****CC config file
        //sprintf(sysCmd, "./getfile.sh %s %s %s %sConfig/%s/ config.sys",                          // CARTVU的配置文件
        //                ip, ftpUser, ftpPassword, REMOTE_FTP_CARTVU_PATH, m1m2m3);
        sprintf(sysCmd, "ftpget -u %s -p %s %s %s %sConfig/%s/config.sys",
                ftpUser, ftpPassword, ip, TEMP_CONFIG_FILE, REMOTE_FTP_CARTVU_PATH, m1m2m3);

        printf("sysCmd = %s\n", sysCmd);


        ftpDogFood = WATCHDOG_COUNT/3;
        ftpFlag = 1;
        system(sysCmd);

        if (check_shell_error(RECORD_ERROR) < 0) {
            sprintf(buf_echo, "%s execute fail", sysCmd); 
            recordLog(1, 47, buf_echo);

        } else {
            sprintf(buf_echo, "%s execute ok!", sysCmd); 
            recordLog(1, 47, buf_echo);

            dogFood = WATCHDOG_COUNT;
            ret = updateCartvuConfig();
            if (ret == 0) {
                printf("Getting config file of Cartvu is ok.\n");
                recordLog(1, 4, "Getting config file of Cartvu is ok");
            } else {
                printf("Getting config file of Cartvu Failed!\n");
                recordLog(1, 4, "Getting config file of Cartvu Failed!");
            } // end of if


        }



        ftpFlag = 0;



    }
    /*********************************************************************
      8. update all the softwares for local node, cartvu and media
     **********************************************************************/
    dogFood = WATCHDOG_COUNT;
    if (isLogin) {
        // download all software
        download_all_software();


        showVersion();
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }
    /****************************************************************************
      9. download MC command file images and image config files for media
     *****************************************************************************/
#if 0
    dogFood = WATCHDOG_COUNT;
    if (isLogin) {



        updateCommand();
    }
#endif





    /************************************************/
    printf("\n!!!!!!!!!!!!another login server!!!!!!!!!!!!!!!!!!!!!!\n\n");
    if (strlen(ip) >= 7) {
        ret = 1; iii = 0;
        while (ret != 0) {
            iii++;
            ret = loginServer();
            if (ret == 0) {
                //printf("Login server ok! Password=%s\n", ftpPassword);
                printf("Login server ok! Password=**********\n");
                recordLog(1, 1, "Login server ok! Password=**********");
                isLogin = 1;
                break;
            } else {
                printf("Login server fail! -- %d\n", iii);
                if (iii == 1) {
                    recordLog(1, 1, "Login server fail!");
                }
                //if (iii == 30) break;
                if (iii == 5) {
                    recordLog(1, 1, "Login server fail! 10 times");
                    iii = 0;
                    break;
                }
            }
            sleep(5);
            dogFood = WATCHDOG_COUNT;
        } // end of if
    } else {
        printf("Can not login server. No internet.\n");
        recordLog(1, 1, "Can not login server. No internet");
    }

    /****************************************************************************
      10. Send statistics information to Service Node   send images to service node
     *****************************************************************************/
    printf("~~~~~~~~~~~~~~update statis here~~~~~~~~~~\n");
    update_statis();
    printf("~~~~~~~~~~after update statis here~~~~~~~~~~~\n");


    ret = loginServer();
    if (ret == 0) {
        //printf("Login server ok! Password=%s\n", ftpPassword);
        printf("\nLogin server ok! Password=**********\n");
        recordLog(1, 1, "Login server ok! Password=**********");
        isLogin = 1;
    } else {
        printf("Login server fail! -- %d\n", iii);
        recordLog(1, 1, "Login server fail!");
        isLogin = 0;
    }




    sleep(2);
    /************************
      ftpput log and alarm images
     *****************************/
    ret = pthread_create(&log_alarm_put, NULL, (void *)ftpput_log_alarm_thread, NULL);
    if (ret != 0) {
        printf("Create ftpput_log_alarm_thread thread failed.\n");
    }


    /****************************************************************************
      11. Check MAC addresses of CARTVU and MEDIA devices from Local Node
     ****************************************************************************/



    dogFood = WATCHDOG_COUNT;
    if (isLogin) {
        pthread_mutex_lock (&work_mutex);
        if (checkLane() == 0) {
            printf("Checking MAC addresses is ok!(checkLane())\n");
            recordLog(1, 44, "Checking MAC addresses is ok!(checkLane())");
        } else {
            printf("Checking MAC addresses failed!(checkLane())\n");
            recordLog(1, 44, "Checking MAC addresses failed!(checkLane())");
        }
        pthread_mutex_unlock (&work_mutex);
    }

    //return 0;
    /****************************************************************************
      13.Enter main loop
     *****************************************************************************/
    sprintf(sysCmd, "chmod 777 %s -R", CARTVU_IMAGE_PATH);
    printf("%s\n", sysCmd);
    system(sysCmd);                    // change the attribute of images directory for getting alarm images
    int sleepCount = 0;
    while (1) {
        printf("\n************Main Program of Local Node %s*********\n\n", VERSION);
        sleepCount++;
        printf("sleepCount = %d\n", sleepCount);
        if (isLogin) {
            dogFood = WATCHDOG_COUNT;

            // 1. send status and clear status   every 2*5 sec
            if ((sleepCount % 2) == 0) {
                if (sendStatus(2) >= 0 ) {                           // Local Node
                    printf("Sending Local Node Status is ok.\n");
                } else {
                    ;// recordLog(1, 20, "Sending Local Node Status is failed!");
                }
                if (sendStatus(0) >= 0) {                           // Cartvu
                    printf("Sending CARTVU Status is ok.\n");
                } else {
                    ;//recordLog(1, 20, "Sending CARTVU Status is failed!");
                }
                if (sendStatus(1) >= 0) {                          // Media
                    printf("Sending MEDIA Status is ok.\n");
                } else {
                    ;//recordLog(1, 20, "Sending MEDIA Status is failed!");
                }
            }






            //3 delete the machine ip on the useonline from every 60 seconds
            if ((sleepCount % 12) == 0) {
                printf("Execute updateUserOnline()\n");
                updateUserOnline();
            }


            // 4. Check new version of software every 720 * 5 seconds
            if ((sleepCount % 360) == 0) {
                update_statis();
            }




        } else { // end of if (isLogin)
            dogFood = WATCHDOG_COUNT;
            // login to service server again.
            ret = loginServer();
            if (ret == 0) {
                //printf("Login server ok! Password=%s\n", ftpPassword);
                printf("Login server ok! Password=**********\n");
                isLogin = 1;
                recordLog(1, 1, "Login server ok! Password=**********");
                login_count = 0;
            } else {
                login_count++;
                printf("Login server fail! -- %d\n", login_count);
                if (login_count >= 36) {
                    login_count = 0;
                    printf("!!!!!!!!restart network here!!!!!\n");
                    recordLog(1, 2, "!!!!!!!!restart network here!!!!!");
                    reset_network();
                    reset_count++;
                    if (reset_count >= 5) {
                        reset_count = 0;

                        //reset the cradle point
                        system("/test/set_router");

                        reset();
                        recordLog(1, 2, "!!!!!!!!restart machine here!!!!!");
                    }
                }
            }
        }  // end of if (isLogin)

        // 2. update the status in table every 40 seconds
        if ((sleepCount % 8) == 0) {
            printf("Execute updateStatus()\n");
            updateStatus();
            update_msg_status();
        }




        // 5. Relogin every 1 hour
        if ((sleepCount % 360) == 0) { 
            sleepCount = 0;
            isLogin = 0;
        } // end of if



        sleep(5);
    }  // end of while (1)

}  // end


