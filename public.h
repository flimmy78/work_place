/***************************************************************************
   文件名：public.h
   
   描述：一组用于LocalServer的子程序
   
   作者：华松青
   
   版本：v1.0
   
   时间：2013-8-31
   
   修改日志：

****************************************************************************/

/**********************************************************
  函数名：int getLocalIp(char *ip)
  
  说明：获得本机的IP地址
  
  入口：ip   用于存储IP地址的字符串地址
  返回：0    执行成功  IP地址放在ip中
        －1  执行失败，ip=NULL

  说明：主程序中必须include下列文件
      #include <arpa/inet.h>
      #include <sys/ioctl.h>
      #include <net/if.h>
***********************************************************/
int getLocalIp(char *ip) {
  int inet_sock;
  struct ifreq ifr;
  struct sockaddr_in *device_data;

  inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
  strcpy(ifr.ifr_name, "eth0");                     // 设备名称 

  if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0) {
    ip = NULL;
    return -1;
  } else {
    device_data = (struct sockaddr_in*)&(ifr.ifr_addr);
    strcpy(ip, inet_ntoa(device_data->sin_addr));
  }  // end if
  return 0 ;
}  // end

/**********************************************************
  函数名：int checkIPBlock(char *ip)
  
  说明：检查是否与本机在同一个IP段
  
  入口：ip   ip地址
  返回： =0    IP地址与本机不在同一个ip段
       >0   ip地址与本机在同一个ip段
       <0   ip地址出现异常错误

  说明：主程序中必须include下列文件
      #include <arpa/inet.h>
      #include <sys/ioctl.h>
      #include <net/if.h>
***********************************************************/
int checkIPBlock(char *ip) {
  char localIp[20];
  char *pos;

  getLocalIp(localIp);                  // 本地IP地址

  pos = rindex(localIp, '.');
  if (pos != 0) {
    *pos = 0;
    pos = strstr(ip, localIp);
    if (pos != ip) return 0;
    return 1;
  } // end of if
  return -1;
}  // end

