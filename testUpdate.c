#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void update_adStatus(char *before, char *now) {

    FILE *fp;

    char buf[256];

    char lastTime[50];
    char nowTime[50];
    char *p;
    char year[10];
    char month[10];
    char day[10];
    char hour[10];
    char min[10];
    char sec[10];


    fp = fopen(before, "r");
    if (fp == NULL) {
        printf("open %s error\n", before);
        return ;
    }



    if (fgets(buf, 256, fp) != NULL) {
        //strlower(buf);
        //trim(buf);

        p = buf;
        bzero(year, sizeof(year));
        strncpy(year, p, 4);

        p += 4;
        bzero(month, sizeof(month));
        strncpy(month, p, 2);

        p += 2;
        bzero(day, sizeof(day));
        strncpy(day, p, 2);

        p += 2;
        bzero(hour, sizeof(hour));
        strncpy(hour, p, 2);

        p += 2;
        bzero(min, sizeof(min));
        strncpy(min, p, 2);

        p += 2;
        bzero(sec, sizeof(sec));
        strncpy(sec, p, 2);
        sprintf(lastTime, "%04s-%02s-%02s %02s:%02s:%02s", year, month, day, hour, min, sec);
        printf("%s\n", lastTime);

    }
    fclose(fp);

    fp = fopen(now, "r");
    if (fp == NULL) {
        printf("open %s error\n", before);
        return ;
    }



    if (fgets(buf, 256, fp) != NULL) {
        //strlower(buf);
        //trim(buf);

        p = buf;
        bzero(year, sizeof(year));
        strncpy(year, p, 4);

        p += 4;
        bzero(month, sizeof(month));
        strncpy(month, p, 2);

        p += 2;
        bzero(day, sizeof(day));
        strncpy(day, p, 2);

        p += 2;
        bzero(hour, sizeof(hour));
        strncpy(hour, p, 2);

        p += 2;
        bzero(min, sizeof(min));
        strncpy(min, p, 2);

        p += 2;
        bzero(sec, sizeof(sec));
        strncpy(sec, p, 2);
        sprintf(lastTime, "%04s-%02s-%02s %02s:%02s:%02s", year, month, day, hour, min, sec);
        printf("%s\n", lastTime);

    }
    fclose(fp); 


}

void compare_two_file_cfg(char *ok_file, char *fail_file) {


    int i = 0;
    int j = 0;
    int ok_num[50]={1,2,3,4,5,6};

    int fail_num[50]={2,4};

    //bzero(ok_num, sizeof(ok_num));
   // bzero(fail_num, sizeof(fail_num));

    //get_num_command(ok_file, ok_num);
    //get_num_command(fail_file, fail_num);

    i = 0;
    while(ok_num[i] != 0) {
        j = 0;
        while(fail_num[j] != 0) {
            if (ok_num[i] == fail_num[j]) {
                ok_num[i] = -1;
                break;
            }
            j++;
        }

        i++;
    }

    printf("ok num is below\n");
    i = 0;
    while(ok_num[i] != 0) {
        printf("%d\n", ok_num[i++]);
    }
    

    

}


int main(void) {

    update_adStatus("./command1.cfg", "./command.cfg");
    
    compare_two_file_cfg("./command1.cfg", "./command.cfg");

    return 0;

}

