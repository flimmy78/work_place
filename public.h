/***************************************************************************
   �ļ�����public.h
   
   ������һ������LocalServer���ӳ���
   
   ���ߣ�������
   
   �汾��v1.0
   
   ʱ�䣺2013-8-31
   
   �޸���־��

****************************************************************************/

/**********************************************************
  ��������int getLocalIp(char *ip)
  
  ˵������ñ�����IP��ַ
  
  ��ڣ�ip   ���ڴ洢IP��ַ���ַ�����ַ
  ���أ�0    ִ�гɹ�  IP��ַ����ip��
        ��1  ִ��ʧ�ܣ�ip=NULL

  ˵�����������б���include�����ļ�
      #include <arpa/inet.h>
      #include <sys/ioctl.h>
      #include <net/if.h>
***********************************************************/
int getLocalIp(char *ip) {
  int inet_sock;
  struct ifreq ifr;
  struct sockaddr_in *device_data;

  inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
  strcpy(ifr.ifr_name, "eth0");                     // �豸���� 

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
  ��������int checkIPBlock(char *ip)
  
  ˵��������Ƿ��뱾����ͬһ��IP��
  
  ��ڣ�ip   ip��ַ
  ���أ� =0    IP��ַ�뱾������ͬһ��ip��
       >0   ip��ַ�뱾����ͬһ��ip��
       <0   ip��ַ�����쳣����

  ˵�����������б���include�����ļ�
      #include <arpa/inet.h>
      #include <sys/ioctl.h>
      #include <net/if.h>
***********************************************************/
int checkIPBlock(char *ip) {
  char localIp[20];
  char *pos;

  getLocalIp(localIp);                  // ����IP��ַ

  pos = rindex(localIp, '.');
  if (pos != 0) {
    *pos = 0;
    pos = strstr(ip, localIp);
    if (pos != ip) return 0;
    return 1;
  } // end of if
  return -1;
}  // end

