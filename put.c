#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include   <sqlite3.h>

#define  DYNAMIC_DATABASE             "/AdNode/database/LocalNode.db"         // 动态数据库文件名

int meidaStatisCallback(void *para, int n_column, char **column_value, char **column_name) {
    printf("hello\n");
    return 0;
}


int main(void)
{
/*
    FILE *fp;
    char buf[1024];



    fp = fopen("./sql.txt", "w");
    if (fp == NULL) {
        perror("open fail");
        return 1;
    }

    strcpy(buf, "hello");
    fputs(buf, fp);
    strcpy(buf, "good");
    fputs(buf, fp);
    fclose(fp);

    */
    
   sqlite3 *conn  = NULL;          // sqlite db handler
  char    sqlStr[256] = {0};            // sql statement
  int     ret;
  sqlite3_stmt* stmt = NULL;
  int len;   
   
   
     ret = sqlite3_open(DYNAMIC_DATABASE, &conn);                             // 打开数据库
  if (ret != SQLITE_OK) {
    printf("fail : Openning database fail(sendStatistics) - %s\n", DYNAMIC_DATABASE);
    return -1;
  } 
    
     strcpy(sqlStr, "update MediaStatis set flag=1 where StatisDate='2013-12-26' and AdName='Tola-2'\n\n");             // SQL语句
     
     
     
     
     
     
     
     
     #if 0
     
     
           ret = sqlite3_exec(conn, sqlStr, meidaStatisCallback, NULL, NULL);
      if (ret != SQLITE_OK) {
        printf("fail: %s\n", sqlStr);
      }
      
      #else 
      
      
            len = strlen(sqlStr);
            if (sqlite3_prepare_v2(conn,sqlStr,len,&stmt,NULL) != SQLITE_OK) {
                perror("sqlite3_prepare_v2 database fail(sendStatistics) !");
                if (stmt) {
                    sqlite3_finalize(stmt);
                }

                sqlite3_close(conn); 
                
                
                return -1;
            }

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                perror("sqlite3_step database fail(sendStatistics)");
                sqlite3_finalize(stmt);


                sqlite3_close(conn); 
                

                return -1;

            }


            sqlite3_finalize(stmt);
            
            
      #endif
      
      
      
      
      
      
        ret = sqlite3_close(conn);                                                     // 关闭数据库
  if (ret != SQLITE_OK) {
    printf("fail : Closing database fail(sendStatistics) - %s", DYNAMIC_DATABASE);
    return -1;
  }
    
    return 0;
}
