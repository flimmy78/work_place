


#define  VERSION                      "V2.00"   


#define  DYNAMIC_DATABASE             "/AdNode/database/LocalNode.db"         // 动态数据库文件名
#define  STATIC_DATABASE              "/AdNode/database/LocalNodeInfo.db"     // 静态数据库文件名
#define  VERSION_DATABASE             "/AdNode/database/LocalNodeVersion.db"  //




#define ROOT_DIR                      "/AdNode/"
#define SEND_INFO_DATABASE            "/AdNode/database/SendInfo.db"
#define SEND_INFO_TABLE               "SENDINFO"

#define  FTPPASSWD_FILE               "/AdNode/config/.ftppasswd"             // 存放FTP密码的文件名
#define  FTPPASSWD_LEN                8                                       // FTP密码长度

#define  LOGIN_TIMEOUT                600                                     // 登录超时时间，秒

#define  ETH0_SETTING_FILE            "/etc/eth0-setting"                     // 网络配置文件
#define  CONFIG_FILE                  "/AdNode/config/config.sys"             // 系统配置文件
#define  TEMP_CONFIG_FILE             "/AdNode/tmp/config.sys"                // 下载下的临时配置文件


#define USERDEF_FILE                  "userdef.zip"

#define TEMP_USERDEF_ZIP              "/AdNode/tmp/"USERDEF_FILE



#define  TEMP_PATH                    "/AdNode/tmp/"

#define  MEDIA_COMMAND_PATH           "/AdNode/media/instruction/"            //
#define  MEDIA_CONFIG_PATH            "/AdNode/media/config/"                 //
#define  MEDIA_ADVERTISING_PATH       "/AdNode/media/ad/"


#define  CARTU_SOFTWARE_DIR           "/AdNode/cartvu/software/"
#define  CARTVU_IMAGE_PATH            "/AdNode/cartvu/images/"                 // 存放CARTVU告警图片的路径
#define  CARTVU_CONFIG_PATH           "/AdNode/cartvu/config/"                //

#define  REMOTE_FTP_ROOT              "/CartvuMedia/FTP"
#define  REMOTE_FTP_LOCALNODE_PATH    "/LocalNode/"                           // 在ServiceNode上，FTP的根目录在/CartvuMedia/FTP
#define  REMOTE_FTP_CARTVU_PATH       "/Cartvu/"                              //
#define  REMOTE_FTP_MEDIA_PATH        "/Media/"                               //

#define SEND_INFO_PHP                 "/SendInfo.php"

#define POST_AD_PHP                   "/adStatus.php"


#define  REMOTE_FTP_SOFTWARE_PATH     "Software"
#define  REMOTE_FTP_ADVERTISING_PATH  "Advertising"

#define  MEDIA_COMMAND_FILE           "command.cfg"                           // MEDIA的命令文件
#define  MEDIA_CONFIG_FILE            "config.sys"                            // MEDIA的配置文件
#define  CARTVU_CONFIG_FILE           "config.sys"                            // CARTVU的配置文件


#define COMMAMD_FAIL    "command.fail"
#define COMMAMD_FAIL2   "command.fail2"

#define POST_AD_FAIL    "ad.fail"
#define POST_AD_FAIL2    "ad.fail2"


#define CONNECT_TIMEOUT  6*2

#define BUF_RECEIVE_SIZE    2048

#define INFO_TEXT_SIZE     2048

#define FTP_THREAD_COUNT              140      // about 1hour

#define WATCHDOG_TIME_OUT             30
#define WATCHDOG_COUNT                24     // (24-1)*0.9*30=621sec  =10min  21ec
//#define WATCHDOG_COUNT                15     // (15-1)*0.9*30=378sec  =6min  18sec

#define TRY_COUNT                 100


#define TMP_SIZE  200




#define CREATE_STATUS   0
#define KNOWN_CREATE    1
#define SCREEN_SHOW     2
#define CASHIER_CONFIRMED  3
#define CASHIER_TIMEOUT   4
#define SERVICENODE_RECEIVE  5


#define TIME_OUT_SEC        60 // sec


#define SERV_RE_PORT     1169
#define SELF_IP     "127.0.0.1"
#define UDP_ADDR   "255.255.255.255"


#define IPADDR_SIZE   50


#define ASK_SERVER  "who is localserver"

#define ETH_SIZE   10

#define RECEIVE_TIMEOUT   10















