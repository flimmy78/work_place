/*****************************************************************************************
   文件名：strseparate.h
   
   描述：主要函数为stringSeparate()，规范key=value&key=value字符串的处理
         衍生出三个小函数ltrim(),rtrim()和trim()，可以去掉字符串的左右空格和前后空格
   
   作者：华松青
   
   版本：1.1
   
   时间：2013－8－29
   
   修改日志：2013-8-30  增加strlower()和strupper()两个函数

******************************************************************************************/
typedef struct {
  char *key;
  char *value;
} paraStruc;

/********************************************************************
  函数名：void strlower(char *s)

  功能：将字符串变成小写字母
  
    入口：s    指向字符串的指针
    返回：s    指向已经转换好的字符串
*********************************************************************/
void strlower(char *s) {
  char *p;
  int i;
  
  p = s;
  for (i = 0; i < strlen(s); i++) {
    if (*p >= 'A' && *p <= 'Z') *p += 0x20;
    p++;
  }  // end of for
} // end

/********************************************************************
  函数名：void strupper(char *s)

  功能：将字符串变成大写字母
  
    入口：s    指向字符串的指针
    返回：s    指向已经转换好的字符串
*********************************************************************/
void strupper(char *s) {
  char *p;
  int i;
  
  p = s;
  for (i = 0; i < strlen(s); i++) {
    if (*p >= 'a' && *p <= 'z') *p -= 0x20;
    p++;
  }  // end of for
} // end

/********************************************************************
  函数名：void ltrim(char *s)

  功能：去掉一个字符串左边的空格和TAB
  
    入口：s    指向字符串的指针
    返回：s   指向已经去掉左边空格的字符串及回车行字符串
*********************************************************************/
void ltrim(char *s) {
  char *p;
  
  if (strlen(s) == 0) return;
  p = s;
  while((*p <= ' ' || *p > 'z') & (*p != '\0')) {
    p++;
  }  // end
  strcpy(s, p);
} // end

/********************************************************************
  函数名：void rtrim(char *s)
  
  功能：去掉一个字符串右边的空格和TAB
  
    入口：s   指向字符串的指针
    返回：s  指向已经去掉右边空格的字符串及回车行字符串
*********************************************************************/
void rtrim(char *s) {
  int i;

  if (strlen(s) == 0) return;
  i = strlen(s) - 1;
  while((s[i] <= ' ' || s[i] > 'z') && i >= 0) {
    i--;
  } // end
  s[i + 1] = '\0';
} // end

/********************************************************************
  函数名：void trim(char *s)
  
  功能：去掉一个字符串前后的空格和TAB
  
    入口：s   指向字符串的指针
    返回：s   已经规范好的字符串
*********************************************************************/
void trim(char *s){
    ltrim(s);
    rtrim(s);
} // end

/*********************************************************************************
  函数名：void stringSeparate(char *p, paraStruc *paraP, int paraLen)  

  功能：将字符串按＆分割成子串，再按＝分割成key/value对的形式
  
    入口：p       指向准备分割的字符串的指针
        paraP    指向一个paraStruc结构数组的指针
        paraLen  paraP指针指向的结构数组的数量
    返回：将分割后的key/value放在paraStruct结构数组中
          如果有key=value对，则放到结构中，如果没有，则key=NULL，
        value＝字符串
**************************************************************************/
void stringSeparate(char *p, paraStruc *paraP, int paraLen) {
  int  i;
  char *p1, *p2, *p3, *p4;

  if (paraLen <= 0 || p == NULL || paraP == NULL) return;

  for (i = 0; i < paraLen; i++) {
    paraP[i].key = NULL;
    paraP[i].value = NULL;
  }  // end

  p1 = p;
  p2 = p1;
  p3 = NULL;
  i = 0;
  for (i = 0; i < paraLen; i++) {
    p3 = strchr(p2, '&');
    if (p3 != NULL) {
      *p3 = 0;
      p2 = p3 + 1;
    } else {
      p2 = NULL;
    }  // end
    if (p1 != NULL) {
      p3 = strchr(p1, '=');
      if (p3 != NULL) {
        *p3 = 0;
        p4 = p3 + 1;
        trim(p1);                  // key
        trim(p4);                  // value
        paraP[i].key   = p1;
        paraP[i].value = p4;
      } else {
        paraP[i].key   = NULL;
        trim(p1);
        paraP[i].value = p1;
      }  //end
    } // end
    if (p2 == NULL || strlen(p2) == 0) break;
    p1 = p2;
  }  // END of for
}  // end

