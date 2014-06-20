/*****************************************************
  Function: int getMac(char *)
  Description: Get MAC address
  
  Entry: char *mac    buffer for mac address
  return: 0  -- success
         -1  -- fail
******************************************************/
int getMac(char *mac) {
  FILE *fp;
  char *p, s[128];
  
  mac[0] = 0;
  fp = fopen(ETH0_SETTING_FILE, "r");
  if (fp == NULL) {
    printf("Open file fail - %s\n", ETH0_SETTING_FILE);
    return -1;
  }  //end of if
  while (fgets(s, 100, fp)) {
    s[100] = 0;
    p = index(s, '=');
    if (p != NULL) {
      *p = 0;
      p++;
      strlower(s);
      if (strcmp(s, "mac") == 0) {
        strcpy(mac, p);
        break;
      }  // end of if
    }  // end of if
  }  // end of while
  fclose(fp);
  if (mac == NULL) return -1;

  while ((p = index(mac, ':')) != NULL) {
    strcpy(p, p + 1);
  }  // end of if

  strcpy(s, mac);
  s[6] = 0;
  strcpy(m1m2m3, s);

  strcpy(ftpUser, "LocalNodeFtp");
  return 0;
}  // end of getMac
/**********************************************************************
  Function: int getIpViaDomain(char *, char *)
  Description: Get IP address via DNS
  
  Entry: char *domain      Domain name
         char *ip          buffer for ip address
  Return: 0  -- Success, put ip address in var ip
          гн1 -- Fail
***********************************************************************/
int getIpViaDomain(char *domain, char *ip) {
  struct hostent *h;
  
  ip[0] = 0;
  //printf("domain = %s\n", domain);
  if ((h = gethostbyname(domain)) == NULL) {
    herror("gethostbyname");
    ip[0] = '\0';
    return -1;
  }  // end of if
  strcpy(ip, inet_ntoa(*((struct in_addr *)h->h_addr)));
  //printf("Host name : %s\n", h->h_name);
  //printf("IP address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
  return 0;
}  // end

