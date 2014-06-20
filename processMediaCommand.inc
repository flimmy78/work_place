



void write_to_fail(char *command, char *record_file) {
    char buf[1024];

    sprintf(buf, "echo '%s'>>%s", command, record_file);
    system(buf);
    recordLog(1,47,buf);
}



struct  cfg_info {
    char id[50];
    char type[50];
    char length[50];
};

void read_cfg_info(char *file, struct  cfg_info *cfg) {


    FILE *fp;

    char *p;
    char s[256];


    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("cfg error\n");
        return ;
    }



    while (fgets(s, 256, fp) != NULL) {
        //printf("string is %s\n", s);
        strlower(s);
        p = index(s, '=');
        if (p != NULL) {
            *p = 0;
            p++;

            trim(s);
            trim(p);
            if (strcmp(s, "id") == 0) {
                strcpy(cfg->id, p);
            } else if  (strcmp(s, "type") == 0) {
                strcpy(cfg->type, p);
            } else if  (strcmp(s, "length") == 0) {
                strcpy(cfg->length, p);
            }

        }

    }
    printf("id:%s\ttype:%s\tlength;%s\n", cfg->id, cfg->type, cfg->length);
    fclose(fp);



}


// >0 :要下载新的  =0:不用下载  
int compare_cfg(char *new_cfg, char *old_cfg) {
    FILE *fp;


    struct  cfg_info  new_info;
    struct  cfg_info  old_info;


    fp = fopen(old_cfg, "r");
    if (fp == NULL) {
        printf("no old_cfg error\n");
        return 1;
    }
    fclose(fp);



    read_cfg_info(new_cfg, &new_info);



    read_cfg_info(old_cfg, &old_info);

    if ((strcmp(new_info.id, old_info.id) == 0) && (strcmp(new_info.type, old_info.type) == 0)  && (strcmp(new_info.length, old_info.length) == 0)){
        printf("no need to download the coresponse img or mp4\n");
        return 0;
    } else {
        printf("need to download img or mp4\n");
        return 1;
    }


}






/****************************************************************************
function: int processMediaommand()
Description: Read the media command file that download from the server
and excute command according to command file

Entry: NONE
Return: 0    success
<0   fail
 *****************************************************************************/
int processMediaCommand(char *read_file, char *record_file) {
    FILE  *fp;
    // int len, i;
    char s[256], filename[50], *p;
    char sysCmd[256];
    char imgFile[50], cfgFile[50];
    char old_File[50], old_cfgFile[50];
    int wrong_flag = 0;
    int try_count = 5;
    char buf_echo[1024];
    char now_command[200] = {0};
    int ret;
    char type[20];

    char rm_buf[100];
    char file_path[100];



    //sprintf(buf_echo, "rm -f %s%s", MEDIA_COMMAND_PATH,COMMAMD_FAIL);
    sprintf(buf_echo, "rm -f %s", record_file);
    printf("%s\n", buf_echo);
    system(buf_echo);

    // 1. open media command file in temp directory
    //sprintf(filename, "%s%s", TEMP_PATH, MEDIA_COMMAND_FILE);
    sprintf(filename, "%s", read_file);
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Media command file is not exist.\n");
        return -1;
    }  //end of if

    sprintf(file_path, "%s%s", MEDIA_COMMAND_PATH, POST_AD_FAIL);
    if (access(file_path, F_OK) == 0) { 
        sprintf(rm_buf, "rm %s -f", file_path);
        system(rm_buf);
    }     


    // 2. read the first line. it must be a date in first line
    while (fgets(s, 256, fp) != NULL) {
        //去掉回车行
        trim(s);
        //        s[strlen(s)] = '\0';
        strcpy(now_command, s);
        p = index(s, '=');
        if (p != NULL) {
            *p = 0;
            p++;
            strlower(s);
            trim(s);
            trim(p);
            if (strcmp(s, "clearall") == 0) {
                sprintf(sysCmd, "rm -f %s*", MEDIA_ADVERTISING_PATH);
                system(sysCmd);
                sprintf(sysCmd, "rm -f %s*.cfg", MEDIA_CONFIG_PATH);
                system(sysCmd);
                recordLog(1, 47, sysCmd);
            } else if ((strcmp(s, "downloadimage") == 0) || (strcmp(s, "downloadmovie") == 0)) {

                if (strcmp(s, "downloadimage") == 0) {
                    sprintf(imgFile, "%s%s.jpg", TEMP_PATH, p);
                    sprintf(cfgFile, "%s%s.cfg", TEMP_PATH, p);

                    sprintf(old_cfgFile, "%s%s.cfg", MEDIA_CONFIG_PATH, p);
                    sprintf(old_File, "%s%s.jpg", MEDIA_ADVERTISING_PATH, p);
                    sprintf(type, "%s.jpg", p);
                } else if (strcmp(s, "downloadmovie") == 0){
                    sprintf(imgFile, "%s%s.mp4", TEMP_PATH, p);
                    sprintf(cfgFile, "%s%s.cfg", TEMP_PATH, p);

                    sprintf(old_cfgFile, "%s%s.cfg", MEDIA_CONFIG_PATH, p);
                    sprintf(old_File, "%s%s.mp4", MEDIA_ADVERTISING_PATH, p);
                    sprintf(type, "%s.mp4", p);
                }   

		post_ad_ok(p, POST_AD_OK_START);

                sprintf(sysCmd, "ftpget -u %s -p %s %s %s %sAdvertising/%s/%s.cfg 2>%s",       // 
                        ftpUser, ftpPassword, ip, cfgFile, REMOTE_FTP_MEDIA_PATH, m1m2m3, p, RECORD_ERROR);

                try_count = 5;
                while(try_count--) {
                    ftpFlag = 1;
                    ftpDogFood = WATCHDOG_COUNT;
                    printf("Downlod config file=%s\n", sysCmd);
                    system(sysCmd);
                    //recordLog(1, 47, sysCmd);
                    if (check_shell_error(RECORD_ERROR) < 0) {
                        sprintf(buf_echo, "%s execute fail", sysCmd); 
                        recordLog(1, 47, buf_echo);
                        wrong_flag = 1;
                    } else {
                        sprintf(buf_echo, "%s execute ok!", sysCmd); 
                        recordLog(1, 47, buf_echo);
                        try_count = 0;
                        wrong_flag = 0;
                    }
                    ftpFlag = 0;
                }

                if (wrong_flag == 0) {
                    printf("go ahead\n");
                } else {    
                    sprintf(sysCmd, "rm -f %s  %s %s %s", cfgFile, imgFile, old_cfgFile, old_File);
                    printf("%s\n", sysCmd);
                    system(sysCmd);
                    recordLog(1, 47, sysCmd);
                    write_to_fail(now_command, record_file);
                    continue;
                }

                if (checkSum(cfgFile) == 0) {
                    printf("go ahead\n");
                } else {
                    sprintf(sysCmd, "checksm of %s fail", cfgFile);
                    printf("%s\n", sysCmd);
                    recordLog(1, 19, sysCmd);
                    sprintf(sysCmd, "rm -f %s  %s %s %s", cfgFile, imgFile, old_cfgFile, old_File);
                    printf("%s\n", sysCmd);
                    system(sysCmd);
                    recordLog(1, 47, sysCmd);
                    write_to_fail(now_command, record_file);
                    continue;
                }

                ret = compare_cfg(cfgFile, old_cfgFile);
                if (ret >0) {
                    printf("go ahead\n");
                } else {
                    printf("no need to download %s\n", imgFile);
                    
                    
		    
                    sprintf(sysCmd, "cp -f %s %s",                                              // advertising config file
                            cfgFile, MEDIA_CONFIG_PATH);
                    recordLog(1, 47, sysCmd);
                    printf("%s\n", sysCmd);
                    system(sysCmd);
                    
		    sprintf(sysCmd, "chmod 777 %s%s.cfg", MEDIA_CONFIG_PATH, p);
		    recordLog(1, 47, sysCmd);
		    printf("%s\n", sysCmd);
		    system(sysCmd);

		    // post to service Node here
		    post_ad_ok(p, POST_AD_OK_END);
                    continue;
                }
                sprintf(sysCmd, "ftpget -u %s -p %s %s %s %sAdvertising/%s/%s 2>%s",               // 
                        ftpUser, ftpPassword, ip, imgFile, REMOTE_FTP_MEDIA_PATH, m1m2m3, type, RECORD_ERROR);

                try_count = 5;
                while(try_count--) {
                    ftpFlag = 1;
                    ftpDogFood = WATCHDOG_COUNT;
                    printf("Download  file:%s\n", sysCmd);
                    system(sysCmd);
                    //recordLog(1, 47, sysCmd);
                    if (check_shell_error(RECORD_ERROR) < 0) {
                        sprintf(buf_echo, "%s execute fail", sysCmd); 
                        recordLog(1, 47, buf_echo);
                        wrong_flag = 1;
                    } else {
                        sprintf(buf_echo, "%s execute ok!", sysCmd); 
                        recordLog(1, 47, buf_echo);
                        try_count = 0;
                        wrong_flag = 0;
                    }
                    ftpFlag = 0;
                }

                if (wrong_flag == 0) {
                    printf("go ahead\n");
                } else {    
                    sprintf(sysCmd, "rm -f %s  %s %s %s", cfgFile, imgFile, old_cfgFile, old_File);
                    printf("%s\n", sysCmd);
                    system(sysCmd);
                    recordLog(1, 47, sysCmd);
                    write_to_fail(now_command, record_file);
                    continue;
                }

                if (checkSum(imgFile) == 0 ) {
                    sprintf(sysCmd, "cp -f %s %s",                                              // advertising image
                            imgFile, MEDIA_ADVERTISING_PATH);
                    printf("%s\n", sysCmd);
                    recordLog(1, 47, sysCmd);
                    system(sysCmd);
                    
		    sprintf(sysCmd, "chmod 777 %s%s", MEDIA_ADVERTISING_PATH, type);
		    recordLog(1, 47, sysCmd);
		    printf("%s\n", sysCmd);
		    system(sysCmd);


                    
                    sprintf(sysCmd, "cp -f %s %s",                                              // advertising config file
                            cfgFile, MEDIA_CONFIG_PATH);
                    printf("%s\n", sysCmd);
                    system(sysCmd);

    		    sprintf(sysCmd, "chmod 777 %s%s.cfg", MEDIA_CONFIG_PATH, p);
		    recordLog(1, 47, sysCmd);
		    printf("%s\n", sysCmd);
		    system(sysCmd);

		    
                    // post to service Node here
		    post_ad_ok(p, POST_AD_OK_END);
                    
                } else {
               	    sprintf(sysCmd, "checksm of %s fail", imgFile);
                    printf("%s\n", sysCmd);
                    recordLog(1, 19, sysCmd);
                    
                    printf("file checksum=0X%02x\n", checkSum(imgFile));
                    printf("will not cp the file to destination\n");

                    sprintf(sysCmd, "rm -f %s  %s %s %s", cfgFile, imgFile, old_cfgFile, old_File);
                    printf("%s\n", sysCmd);
                    system(sysCmd);
                    recordLog(1, 47, sysCmd);
                    write_to_fail(now_command, record_file);
                    continue;

                }


                sprintf(sysCmd, "rm -f %s  %s", cfgFile, imgFile);
                system(sysCmd);
            } else if (strcmp(s, "deleteimage") == 0 || strcmp(s, "deletemovie") == 0) {
                sprintf(sysCmd, "rm -f %s%s.*",               // advertising image
                        MEDIA_ADVERTISING_PATH, p);
                system(sysCmd);
                recordLog(1, 47, sysCmd);
                sprintf(sysCmd, "rm -f %s%s.*",               // advertising image
                        MEDIA_CONFIG_PATH, p);
                system(sysCmd);
                recordLog(1, 47, sysCmd);
            }
        }
    }
    fclose(fp);


    return 0;
}

int processFailCommand(char *read_file, char *record_file) {
    char buf[1024];
    processMediaCommand(read_file, record_file);
    if (access(record_file, F_OK) == 0) {
        sprintf(buf, "mv %s  %s ", record_file, read_file);
        recordLog(1, 47, buf);
        printf("change to its original file\n");
        printf("%s\n", buf);
        system(buf);
    } else {
        sprintf(buf, "rm -f %s", read_file);
        recordLog(1, 47, buf);
        printf("%s\n", buf);
        system(buf);
    }

    return 0;

}

