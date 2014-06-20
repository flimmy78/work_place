/************************************************************************
  Function: int readConfig()
  Description: Read config file
  
  Entry:  None
  Return: 0 -- Success
         -1 -- Fail
*************************************************************************/
int readConfig() {
  FILE  *fp;
  int len, i;
  char s[256], *p;
  
  serverDomain1[0] = 0;
  serverDomain2[0] = 0;
  strcpy(openningTime, "08:00");
  strcpy(closeTime, "22:00");
  strcpy(resetTime, "03:00");
  timeZone = 0;
  // 1. open config file
  fp = fopen(CONFIG_FILE, "r");
  if (fp == NULL) {
    printf("Open file fail\n");
    return -1;
  }  //end of if
  
  // 2. read rhe first line. it must be a date in first line
  fgets(s, 255, fp);
  trim(s);
  len = strlen(s);
  if (len != 10 && len != 14) {
    printf("Wrong date formate in config file.%s--len=%d\n", s, len);
    fclose(fp);
    return -1;
  }  // end of if
  for (i = 0; i < len; i++) {
    if ((s[i] < '0' || s[i] > '9') && s[i] != '-') {
      printf("Wrong date in config file.%s\n", s);
      fclose(fp);
      return -1;
    }  // end of if
  } // end of for
  if (len == 14) {
    strcpy(configDate, s);
  } else {
    strncpy(configDate, s, 4);
    configDate[4] = s[5];
    configDate[5] = s[6];
    configDate[6] = s[8];
    configDate[7] = s[9];
    configDate[8] = 0;
    strcat(configDate, "000000");
  }
  
  // 3. read every line in the config file
  while (fgets(s, 100, fp)) {
    p = index(s, '=');
    if (p != NULL) {
      *p = 0;
      p++;
      strlower(s);
      trim(s);
      trim(p);
      //printf("key=%s\tvalue=%s\n", s, p);
      if (strcmp(s, "servicenode1") == 0) {
        strcpy(serverDomain1, p);
      } else if (strcmp(s, "servicenode2") == 0) {
        strcpy(serverDomain2, p);
      } else if (strcmp(s, "openningtime") == 0) {
        strcpy(openningTime, p);
      } else if (strcmp(s, "closingtime") == 0) {
        strcpy(closeTime, p);
      } else if (strcmp(s, "resettime") == 0) {
        strcpy(resetTime, p);
      } else if (strcmp(s, "timezone") == 0) {
        //printf("TimeZong = %s\n", p);
        timeZone = atoi(p);
      }  // end of if
    }  // end of if
  }  // end of while
  fclose(fp);
  return 0;
} // end readConfig

