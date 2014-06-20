/************************************************************
  Function: int updateMediaConfig()
  Description: Update config file of MEDIA
  
  Entry: NONE
  Return: 0     success
          <0    fail
*************************************************************/
int updateMediaConfig() {
  FILE  *fp;
  int len, i, valid = 0;
  char s[256], s1[20], *p;
  char fileName[50];
  
  // 1. open temp Media Config file
  sprintf(fileName, "%s%s", TEMP_PATH, MEDIA_CONFIG_FILE);
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("Temporary media config file is not exist.\n");
    return -1;
  }  //end of if
  
  // 2. read the first line. it must be a date in first line
  fgets(s, 255, fp);
  trim(s);
  if (s[4] == '=') {
    s[4] = 0;
    p = s;
    p += 5;
    strlower(s);
    if (strcmp(s, "date") != 0) {
      printf("Wrong date formate in temp media config file - 1.%s\n", s);
      fclose(fp);
      return -2;
    }  // end of if
  } else {
    p = s;
  }
  trim(p);
  len = strlen(p);
  
  if (len != 8 && len != 14) {
    printf("Wrong date formate in temp media config file - 2.%s\n", p);
    fclose(fp);
    return -3;
  }  // end of if
  
  for (i = 0; i < len; i++) {
    if (p[i] < '0' || p[i] > '9') {
      printf("Wrong date in temp media config file.%s\n", s);
      fclose(fp);
      return -4;
    }  // end of if
  } // end of for
  fclose(fp);
  strcpy(s1, p);
  if (len == 8) {
    strcat(s1, "000000");
  }

  // 3. open Media Config file
  sprintf(fileName, "%s%s", MEDIA_CONFIG_PATH, MEDIA_CONFIG_FILE);
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("Media config file is not exist.\n");
    valid++;
  } else {
    // 4. read the first line. it must be a date in first line
    fgets(s, 255, fp);
    trim(s);
    if (s[4] == '=') {
      s[4] = 0;
      p = s;
      p += 5;
      strlower(s);
      if (strcmp(s, "date") != 0) {
        printf("Wrong date formate in media config file - 1.%s\n", s);
        valid++;
      }
    } else {
      p = s;
    }
    trim(p);
    len = strlen(p);
  
    if (len != 8 && len != 14) {
      printf("Wrong date formate in temp media config file - 2.%s\n", p);
      valid++;
    } else {
 
      for (i = 0; i < len; i++) {
        if (p[i] < '0' || p[i] > '9') {
          printf("Wrong date in temp media config file.%s\n", s);
          valid++;
          break;
        }  // end of if
      } // end of for
      if (len == 8) {
        strcat(p, "000000");
      }
    }  // end of if
    fclose(fp);
  }  // end of if ... else

  // 5. compare the date between config file and temp config file
  if ((valid > 0) || (strcmp(p, s1) < 0)) {
    sprintf(s, "cp -f %s%s %s%s", 
               TEMP_PATH, MEDIA_CONFIG_FILE,
               MEDIA_CONFIG_PATH, MEDIA_CONFIG_FILE);
    system(s);
    sprintf(s, "/AdNode/proc/checksum %s%s", MEDIA_CONFIG_PATH, MEDIA_CONFIG_FILE);
    system(s);
  }  // end of if
  sprintf(fileName, "%s%s", TEMP_PATH, MEDIA_CONFIG_FILE);
  unlink(fileName);
  return 0;
}  // end of updateMediaConfig

