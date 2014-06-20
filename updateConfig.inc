/**********************************************************
  Function: int updateConfig()
  Description: Update config file of localnode
  
  Entry: NONE
  Return: 0     success
          <0    fail
*************************************************************/
int updateConfig() {
  FILE  *fp;
  int len, i;
  char s[256];
  
  // 1. open config file
  fp = fopen(TEMP_CONFIG_FILE, "r");
  if (fp == NULL) {
    printf("Temporary config file is not exist.\n");
    return -1;
  }  //end of if
  
  // 2. read the first line. it must be a date in first line
  // The first line is date & time yyyymmddhhmmss. it is yyyy-mm-dd before
  fgets(s, 255, fp);
  trim(s);
  len = strlen(s);
  if (len != 10 && len != 14) {
    printf("Wrong date formate in temp config file.%s--len=%d\n", s, len);
    fclose(fp);
    return -2;
  }  // end of if
  for (i = 0; i < len; i++) {
    if ((s[i] < '0' || s[i] > '9') && s[i] != '-') {
      printf("Wrong date in temp config file.%s\n", s);
      fclose(fp);
      return -3;
    }  // end of if
  } // end of for
  if (len == 10) {
    s[4] = s[5];
    s[5] = s[6];
    s[6] = s[8];
    s[7] = s[9];
    s[8] = 0;
    strcat(s, "000000");
  }
  // 3. compare the date between config file and temp config file
  if (strcmp(configDate, s) < 0) {
    sprintf(s, "cp -f %s %s", TEMP_CONFIG_FILE, CONFIG_FILE);
    system(s);
    printf("%s\n", s);
    recordLog(1, 4, s);
    readConfig();                        // update the config file
  } else {
	printf("no need to update the config.sys");
	recordLog(1, 4, "no need to update the config.sys");
  }
  unlink(TEMP_CONFIG_FILE);
  return 0;
}  // end of updateConfig
