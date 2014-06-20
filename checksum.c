/*******************************************************************
  File Name: check.c
  Description: Calculate the checksum of a file and then add a char
               at the end of the file to let the checksum of this
               file equal 0.
  
  Compile: gcc -o checksum checksum.c
  Usage: checksum filename
  
  Author: Songqing Hua
  Version: v1.0
  Date: 2013-10-2
********************************************************************/
#include <stdio.h>

int main(int argc, char **argv) {
  FILE *m;
  unsigned char sum, oneChar;
  char newChar;

  if (argc < 2) {
	printf("Usage : checksum [filename]\n");
	return 1;
  }
  m = fopen(argv[1], "rb");
  if (m == NULL) {
	printf("\n File Not Found!");
	return 1;
  }
  sum = 0;

  while (fread(&oneChar, 1, 1, m) > 0) {
	sum += oneChar;
  }
  fclose(m);

  printf("\nChar is 0X%02x\n", sum);
  if (sum != 0) {
	newChar = -sum;

	m = fopen(argv[1], "ab");
	fwrite(&newChar, 1, 1, m);
	fclose(m);
  }
  return 0;
}

