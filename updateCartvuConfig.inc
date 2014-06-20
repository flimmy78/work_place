/************************************************************
  Function: int updateCartvuConfig()
  Description: Update config file of CARTVU
  
  Entry: NONE
  Return: 0     success
          <0    fail
*************************************************************/



int downloadUserdef(void) {
	char sysCmd[1024];
	char buf_echo[1024];
	int count = 5;

        sprintf(sysCmd, "ftpget -u %s -p %s %s %s %sConfig/%s/%s 2>%s",
                ftpUser, ftpPassword, ip, TEMP_USERDEF_ZIP, REMOTE_FTP_CARTVU_PATH, m1m2m3, USERDEF_FILE, RECORD_ERROR);

        printf("sysCmd = %s\n", sysCmd);


  	while(count--) {
	        ftpDogFood = WATCHDOG_COUNT/3;
	        ftpFlag = 1;


	        system(sysCmd);
	        if (check_shell_error(RECORD_ERROR) < 0) {
	            sprintf(buf_echo, "%s execute fail", sysCmd); 
	            recordLog(1, 47, buf_echo);
	            recordLog(1, 4, "Getting userdef.zip file of localnode is fail");
	           // return -1;
	        } else {
	            sprintf(buf_echo, "%s execute ok!", sysCmd); 
	            recordLog(1, 47, buf_echo);
	            printf("~~~Getting userdef.zip file of localnode is ok.\n");
	            recordLog(1, 4, "Getting userdef.zip file of localnode is ok");
	            break;

	        }


	       
        }
        ftpFlag = 0;

	if (count == -1) {
		return -1;
	}

	sprintf(sysCmd, "mv %s %s%s 2>%s", TEMP_USERDEF_ZIP, CARTU_SOFTWARE_DIR, USERDEF_FILE, RECORD_ERROR);
	printf("%s\n", sysCmd);
	system(sysCmd);
	
	if (check_shell_error(RECORD_ERROR) < 0) {
		sprintf(buf_echo, "%s execute fail", sysCmd); 
		recordLog(1, 47, buf_echo);
		return -1;
	} else {
		sprintf(buf_echo, "%s execute ok!", sysCmd); 
		recordLog(1, 4, "Getting userdef.zip file of localnode is ok");
	}

	sprintf(buf_echo, "chmod 777 %s%s", CARTU_SOFTWARE_DIR, USERDEF_FILE);
	system(buf_echo);

    return 0;

}


int updateCartvuConfig() {
  FILE  *fp;
  int len, i, valid = 0;
  char s[256], s1[20];
  char fileName[50];
  int ret = -1;
  
  // 1. open temp Cartvu Config file
  sprintf(fileName, "%s%s", TEMP_PATH, CARTVU_CONFIG_FILE);
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("Temporary cartvu config file is not exist.--%s\n", fileName);
    return -1;
  }  //end of if
  
  // 2. read the first line. it must be a date in first line
  fgets(s, 255, fp);
  trim(s);
  len = strlen(s);
  
  if (len != 10 && len != 14) {
    printf("Wrong date formate in temp cartvu config file.%s\n", s);
    fclose(fp);
    return -2;
  }  // end of if
  
  for (i = 0; i < len; i++) {
    if ((s[i] < '0' || s[i] > '9') && (s[i] != '-')) {
      printf("Wrong date in temp cartvu config file.%s\n", s);
      fclose(fp);
      return -3;
    }  // end of if
  } // end of for
  fclose(fp);
  strcpy(s1, s);
  if (len == 10) {
    s1[4] = s1[5];
    s1[5] = s1[6];
    s1[6] = s1[8];
    s1[7] = s1[9];
    s1[8] = 0;
    strcat(s1, "000000");
  }
  // 3. open Cartvu Config file
  sprintf(fileName, "%s%s", CARTVU_CONFIG_PATH, CARTVU_CONFIG_FILE);
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("Cartvu config file is not exist. - %s\n", fileName);
    valid++;
  } else {
    // 4. read the first line. it must be a date in first line
    fgets(s, 255, fp);
    trim(s);
    len = strlen(s);
  
    if (len != 10 && len != 14) {
      printf("Wrong date formate in cartvu config file.%s\n", s);
      valid++;
    } else {
      for (i = 0; i < len; i++) {
        if ((s[i] < '0' || s[i] > '9') && (s[i] != '-')) {
          printf("Wrong date in cartvu config file.%s\n", s);
          valid++;
          break;
        }  // end of if
      } // end of for
    }  // end of if
    fclose(fp);
  }  // end of if ... else

  if (len == 10) {
    s[4] = s[5];
    s[5] = s[6];
    s[6] = s[8];
    s[7] = s[9];
    s[8] = 0;
    strcat(s, "000000");
  }
  // 5. compare the date between config file and temp config file
  // valid > 0 means invalid date format
  if ((valid > 0) || (strcmp(s, s1) < 0)) {

    ret = downloadUserdef();
    if (ret == 0) {
   
	    sprintf(s, "cp -f %s%s %s%s", 
	               TEMP_PATH, CARTVU_CONFIG_FILE,
	               CARTVU_CONFIG_PATH, CARTVU_CONFIG_FILE);
	    //printf("cmd = %s\n", s);
	    system(s);
	    sprintf(s, "/AdNode/proc/checksum %s%s", CARTVU_CONFIG_PATH, CARTVU_CONFIG_FILE);
	    system(s);
	    printf("update the cartvu config.sys here\n");
	    recordLog(1, 1, "update the cartvu config.sys here");
    } else {

	printf("~~~~~download userdef.zip fail so no update the config.sys here\n");
	recordLog(1, 1, "~~download userdef.zip fail so no update the config.sys here");
    }

    
  }  // end of if
  sprintf(fileName, "%s%s", TEMP_PATH, CARTVU_CONFIG_FILE);
  unlink(fileName);
  return 0;
}  // end of updateCartvuConfig

