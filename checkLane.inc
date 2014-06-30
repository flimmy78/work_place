/****************************************************************************
  function: int checkLaneCallback(.....)
  Description: CallBack function from database query of checkLane function
  
  Entry: NONE
  Return: 0     success
          <0    fail
*****************************************************************************/
int isChecked = 0;             // A FLAG. 0-no record.1-get record.2-no more lane
int checkLaneCallback(void *para, int n_column, char **column_value, char **column_name) {
  isChecked = 1;
  return  0;
}  // end
/****************************************************************************
  function: int checkLane()
  Description: Sending query to local node and getting reply from local node
  
  Entry: NONE
  Return: 0     success
          <0    fail
*****************************************************************************/
int checkLane() {
  sqlite3 *conn  = NULL;          // sqlite db handler
  char paras[1024], result[2048], sqlStr[512];
  char lane[4];
  char *p, *p1;
  int len, i, j;
  //FILE *fp;
  char temp_response[MAXLEN_RESPONSE];
  char response[MAXLEN_RESPONSE];
  
  paraStruc paraS[20];            //
  int ret;

  int count = 0;
  char buf_echo[2048];

  ret = sqlite3_open(STATIC_DATABASE, &conn);              // 打开数据库
  if (ret != SQLITE_OK) {
    printf("fail : Openning database fail(checkLane()) - %s", STATIC_DATABASE);
    recordLog(1, 16, "Openning database fail(checkLane) fail!");
    return -1;
  }
  for (i = 0; i < 50; i++) {                               // max. lane = 50
    isChecked = 0;                                         // JUST A FLAG. 0-no record.1-get record.2-no lane

    sprintf(paras, "mac=%s&ln=%d", mac, i + 1);
    sprintf(request,
            "POST /checklane.php HTTP/1.1\nHOST:%s\nConnection: close\nContent-Type:application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n\n",
            serverDomain1, strlen(paras), paras);
    //printf("request=%s\n", request);
    bzero(response, sizeof(response));
    bzero(temp_response, sizeof(temp_response));
    if (pthread_mutex_lock (&send_mutex) != 0) {
        sprintf(temp_response, "mutex lock fail %s", __func__);
        printf("%s\n", temp_response);
        recordLog(1, 2, temp_response);
    } else {
        printf("lock in %s \n", __func__);
    }
    ret = sendRequest(ip, request, temp_response, sizeof(temp_response));
    strcpy(response, temp_response);
    if (pthread_mutex_unlock(&send_mutex) != 0) {
        sprintf(temp_response, "mutex unlock fail %s", __func__);
        printf("%s\n", temp_response);
        recordLog(1, 2, temp_response);
    } else {
        printf("unlock in %s\n", __func__);
    }

    if (ret < 0) {
    	isLogin = 0;
    	printf("network maybe error\n");
    	sprintf(buf_echo, "network maybe error(checkLane) in %d", i+1);
    	printf("%s\n", buf_echo);
    	//recordLog(1, 2, "network maybe error(checkLane)");
    	//break;
    	recordLog(1, 2, buf_echo);
    	continue;
    }

    // cartvu=000011100DB3&media=000011F00D11
    //printf("response=%s\n", response);
    strcpy(buf_echo, response);
    strcat(buf_echo, paras);
    p = strstr(response, "\r\n\r\n");
    
    if (p == NULL) {
    	printf("the network maybe error in checklane 1\n");
    	sprintf(buf_echo, "network maybe error(checkLane) 1 lane=%d", i+1);
    	recordLog(1, 2, buf_echo);
    	continue;
    }
    p += 4;
  
    p1 = p;                            // length of response
    p = strstr(p, "\r\n");
    if (p == NULL) {
    	printf("the network maybe error in checklane 2\n");
    	recordLog(1, 2, "network maybe error(checkLane) 2");
    	continue;
    }
    *p = 0;
    p += 2;
  
    len = (int)strtol(p1, NULL, 16);
    p[len] = 0;

    strcpy(result, p);
    result[len] = 0;
    stringSeparate(result, paraS, 20);

    for (j = 0; j < 20; j++) {
      if (paraS[j].key == NULL) break;                         // end of traversing
      //printf("j=%d\tkey=%s\tvalue=%s\n", j, paraS[j].key, paraS[j].value);
      strlower(paraS[j].key);
      strlower(paraS[j].value);
      if (strcmp(paraS[j].key, "cartvu") == 0 || strcmp(paraS[j].key, "media") == 0) {
        if (paraS[j].value == NULL || strlen(paraS[j].value) == 0) {
          isChecked = 2;
          break;
        }
        if (strlen(paraS[j].value) == 12) {
          isChecked = 0;
          sprintf(sqlStr, "select * from Mac where mac='%s' and kind='%s'", paraS[j].value, paraS[j].key);
          //printf("sql = %s\n", sqlStr);
	  count = 0;
	  while (1) {
	      count++;
              ret = sqlite3_exec(conn, sqlStr, checkLaneCallback, NULL, NULL);  // 执行
	      if (ret != SQLITE_OK) {
	          if (count > TRY_COUNT) {
	              printf("fail : try %d times:Execute fail(checkLane()) - %s - %d\n", count, sqlStr, ret);
	              recordLog(1, 34, strcat(sqlStr, "Execute (checkLane()) fail!!"));
	              break;
	          }
	      } else {
	          break;
	      }
	  }
	  
          if (isChecked == 0) {
            // no record in table Mac. So insert a new record in table.
            // fields: mac,kind,lane,lasttime,enable,status,statustime
            strncpy(lane, &paraS[j].value[8], 2);
            lane[2] = 0;
            sprintf(sqlStr, "delete from Mac where Lane=%d and kind='%s'", 
                            (int)strtol(lane, NULL, 16), paraS[j].key);
            //printf("sql = %s\n", sqlStr);
           
	    count = 0;
	    while (1) {
	        count++;
                ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);             // 执行
	        if (ret != SQLITE_OK) {
	            if (count > TRY_COUNT) {
	                printf("fail : try %d times:Execute fail(checkLane()) - %s - %d\n", count, sqlStr, ret);
	                recordLog(1, 34, strcat(sqlStr, "Execute (checkLane()) fail!!"));
	                break;
	            }
	        } else {
	            break;
	        }
	    }

            // fields:Mac Kind Lane LastTime Enable Status StatusTime Operation
            sprintf(sqlStr, "insert into Mac values('%s','%s',%d,strftime('%%Y-%%m-%%d %%H:%%M:%%S', 'now'),1,0,strftime('%%Y-%%m-%%d %%H:%%M:%%S', 'now'),0)", 
                            paraS[j].value, paraS[j].key,
                            (int)strtol(lane, NULL, 16));
            //printf("sql = %s\n", sqlStr);
            count = 0;
	    while (1) {
	        count++;
                ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);             // 执行
	        if (ret != SQLITE_OK) {
	            if (count > TRY_COUNT) {
	                printf("fail : try %d times:Execute fail(checkLane()) - %s - %d\n", count, sqlStr, ret);
	                recordLog(1, 34, strcat(sqlStr, "Execute (checkLane()) fail!!"));
	                break;
	            }
	        } else {
	            break;
	        }
	    }
          }
        }   // end of if
      } else {
	printf("severvice node send error info(in checkLnae) %s\n", buf_echo);
	sprintf(buf_echo, "%s fail in (checkLnae)", buf_echo);
	recordLog(1, 34, buf_echo);
      }
    }  // end of for
    if (isChecked == 2) {
        printf("service node has no cartvu media in %d\n", i + 1);
        
        sprintf(sqlStr, "delete from Mac where Lane>=%d",i + 1);
        ret = sqlite3_exec(conn, sqlStr, NULL, NULL, NULL);  
        if (ret != SQLITE_OK) {
            printf("fail to %s\n", sqlStr);
            recordLog(1, 34, strcat(sqlStr, "Execute (checkLane()) fail!!"));
        
        } else {
              printf("execute %s ok\n", sqlStr);
              recordLog(1, 2, strcat(sqlStr, " execute ok"));
        }

        
        break;
    }
  }  // end of for




  
  sqlite3_close(conn);                                                               // 关闭数据库
  return 0;  

}

