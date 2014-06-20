/************************************************************
Function: int updateMediaCmd()
Description: Update command file of MEDIA

Entry: NONE
Return: 0     success
<0    fail
 *************************************************************/
int updateMediaCmd() {
    FILE  *fp;
    int len, i, valid = 0;
    char s[256], s1[20];
    char fileName[50];
    char *p = NULL;

    // 1. open temp Media Command file
    sprintf(fileName, "%s%s", TEMP_PATH, MEDIA_COMMAND_FILE);
    fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Temporary media command file is not exist.\n");
        recordLog(1, 14, "Temporary media command file is not exist");
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
            printf("Wrong date formate in temp command.cfg - 1.%s\n", s);
            fclose(fp);
            unlink(fileName);
            return -2;
        }  // end of if
    } else {
        p = s;
    }
    trim(p);
    len = strlen(p);






    if (len != 8 && len != 14) {
        printf("Wrong date formate in temp command.cfg  file - 2:%s\n", p);
        fclose(fp);
        unlink(fileName);
        return -3;
    }  // end of if

    for (i = 0; i < len; i++) {
        if (p[i] < '0' || p[i] > '9') {
            printf("Wrong date in temp command.cfg file -3:%s\n", s);
            fclose(fp);
            unlink(fileName);
            return -4;
        }  // end of if
    } // end of for
    fclose(fp);
    strcpy(s1, p);
    if (len == 8) {
        strcat(s1, "000000");
    }



    // 3. open Media Command file
    sprintf(fileName, "%s%s", MEDIA_COMMAND_PATH, MEDIA_COMMAND_FILE);
    fp = fopen(fileName, "r");
    valid = 0;
    if (fp == NULL) {
        printf("Media command file is not exist. - %s\n", fileName);
        valid++;
    } else {
        // 4. read the first line.
        fgets(s, 255, fp);
        trim(s);
        if (s[4] == '=') {
            s[4] = 0;
            p = s;
            p += 5;
            strlower(s);
            if (strcmp(s, "date") != 0) {
                printf("Wrong date formate in command file - 1.%s\n", s);
                recordLog(1, 46, "the local command format is not right date");
                valid++;
            }
        } else {
            p = s;
        }
        trim(p);
        len = strlen(p);

        if (len != 8 && len != 14) {
            printf("Wrong date formate in temp command file - 2.%s\n", p);
            recordLog(1, 46, "the local command format is not right in len");
            valid++;
        } else {

            for (i = 0; i < len; i++) {
                if (p[i] < '0' || p[i] > '9') {
                    printf("Wrong date in temp command file.%s\n", s);
                    recordLog(1, 46, "the local command format is not right in scope");
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



    // 5. compare the date between command file and temp command file

    if ((valid > 0) || (strcmp(p, s1) < 0)) {
        recordLog(1, 48,"need to update the command.cfg file");
        /*
        sprintf(s, "cp -f %s%s %s%s", 
                TEMP_PATH, MEDIA_COMMAND_FILE,
                MEDIA_COMMAND_PATH, MEDIA_COMMAND_FILE);
        system(s);
        */
        
        sprintf(s, "/AdNode/proc/checksum %s%s 2>%s", MEDIA_COMMAND_PATH, MEDIA_COMMAND_FILE, RECORD_ERROR);
        system(s);
        if (check_shell_error(RECORD_ERROR) < 0) {
        	recordLog(1, 19, "/AdNode/proc/checksum fail");
    	}
	printf("need to update the command.cfg file\n");
	return 100;
    } else {
    // delete command file in temp directory if not updating the command file
    	sprintf(fileName, "%s%s", TEMP_PATH, MEDIA_COMMAND_FILE);
      	unlink(fileName);
      	printf("no need to update the command.cfg file\n");
      	recordLog(1, 48,"no need to update the command.cfg file");
      	return 200;
    }
    return 0;
}  // end of updateMediaCmd

