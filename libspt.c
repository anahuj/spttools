

// Copyright 2007 Juhana Sadeharju

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "mylib/filesdirs.h"
#include "mylib/buffer.h"
#include "mylib/image.h"


void print_string(char *s)
{
  printf("string = \"%s\"\n",s);
}

void print_int(int n)
{
  printf("int = %i\n",n);
}

// XXXX Fix the float accuracy problem by printing the original
// XXXX float as hexadecimal. If the float is modified, the
// XXXX hexadecimal number should be changed to "---" or deleted.
void print_float(float f)
{
  printf("float = %.16f\n",f);
}

void print_byte(int n)
{
  printf("byte = %i\n",n);
}

void print_short(int n)
{
  printf("short = %i\n",n);
}


int main(int ac, char **av)
{
  unsigned char *buffer;
  int len,n,i;
  void *b;
  char *s;
  float f;
  int hierarchy;
  void *spt;

  if (ac < 2) {
    fprintf(stderr,"Usage: sptparser <file>\n");
    exit(-1);
  }

  file2buffer(av[1],&buffer,&len);

  b = buffer_new(buffer,len);

  spt = (ty_spt *)malloc(sizeof(ty_spt));

  hierarchy = 0;
  while (buffer_eof(b) != 1) {
    // Section number.
    n = buffer_read_int(b);
#if 0
    for (i = 0; i < hierarchy; i++) {
      fprintf(stderr,"  ");
    }
    fprintf(stderr,"Section = %i\n",n);
    // fprintf(stderr,"Section = %i in %s\n",n,av[1]);
#endif

    printf("section = %i\n",n);
    switch (n) {
    case 1000:
      s = buffer_read_intstring(b);
      print_string(s);
      spt->version = s;
      break;
    case 1001:
      break;
    case 1002:
      hierarchy++;
      break;
    case 1003:
      hierarchy--;
      break;
    case 1004:
      hierarchy++;
      break;
    case 1005:
      hierarchy--;
      break;
    case 1006:
      n = buffer_read_int(b);
      print_int(n);
      spt->numleavesbillboards = n;
      spt->idxleavesbillboards = 0;
      break;
    case 1007:
      hierarchy++;
      break;
    case 1008:
      hierarchy--;
      spt->idxleavesbillboards++;
      break;
    case 1009:
      hierarchy++;
      break;
    case 1010:
      hierarchy--;
      break;
    case 1011:
      hierarchy++;
      break;
    case 1012:
      hierarchy--;
      break;
    case 1013:
      // XXXX
      break;
    case 1014:
      n = buffer_read_int(b);
      print_int(n);
      hierarchy++;
      spt->numbranchlevels = n;
      spt->idxbranchlevels = 0;
      break;
    case 1015:
      hierarchy--;
      break;
    case 1016:
      hierarchy++;
      break;
    case 1017:
      hierarchy--;
      spt->idxbranchlevels++;
      break;
    case 2000:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 2001:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 2002:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 2003:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 2004:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 2005:
      n = buffer_read_byte(b);
      print_byte(n);
      n = buffer_read_short(b);
      print_short(n);
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 2006:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 2007:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 3000:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 3001:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 3002:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 3003:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 3004:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 3005:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 3006:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 3007:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 3008:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 3009:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 3010:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 4000:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 4001:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 4002:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 4003:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 4004:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 4005:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 4006:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 4007:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5000:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5001:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5002:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5003:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5004:
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5005:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 5006:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 6000:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6001:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6002:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6003:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6004:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6006:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6007:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 6008:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 6009:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 6010:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 6011:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 6012:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 6013:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 6014:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 6015:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 6016:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 6017:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 8000:
      hierarchy++;
      break;
    case 8001:
      hierarchy--;
      break;
    case 8002:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 8003:
      for (i = 0; i < 13; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 8004:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 8005:
      for (i = 0; i < 13; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 8006:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 8007:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 8008:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 8009:
      for (i = 0; i < 13; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 9000:
      hierarchy++;
      break;
    case 9001:
      hierarchy--;
      break;
    case 9002:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 9003:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9004:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9005:
      break;
    case 9006:
      break;
    case 9007:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 9008:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9009:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9010:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9011:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 9012:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9013:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 9014:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 10000:
      hierarchy++;
      break;
    case 10001:
      hierarchy--;
      break;
    case 10002:
      n = buffer_read_int(b);
      print_int(n);
      for (i = 0; i < 8*n; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 10003:
      n = buffer_read_int(b);
      print_int(n);
      for (i = 0; i < 8*n; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 10004:
      n = buffer_read_int(b);
      print_int(n);
      for (i = 0; i < 8*n; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 11000:
      hierarchy++;
      break;
    case 11001:
      hierarchy--;
      break;
    case 11002:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 12000:
      hierarchy++;
      // XXXX Counts of 12002 and 12003 are not directly given.
      // XXXX Are they given at all? How 12002 and 12003 are
      // XXXX used?
      // Update spt->numblock1200[23]s after reading the spt file.
      spt->idxblock12002s = 0;
      spt->idxblock12003s = 0;
      break;
    case 12001:
      hierarchy--;
      break;
    case 12002:
      for (i = 0; i < 4; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      spt->idxblock12002s++;
      break;
    case 12003:
      for (i = 0; i < 5; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      spt->idxblock12003s++;
      break;
    case 13000:
      hierarchy++;
      break;
    case 13001:
      hierarchy--;
      break;
    case 13002:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 13003:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 13004:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 13005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 13006:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 13007:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 13008:
      n = buffer_read_int(b);
      print_int(n);
      spt->numfrondbillboards = n;
      spt->idxfrondbillboards = 0;
      break;
    case 13009:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 13010:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 13011:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 13012:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 13013:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 14000:
      hierarchy++;
      break;
    case 14001:
      hierarchy--;
      spt->idxfrondbillboards++;
      break;
    case 14002:
      s = buffer_read_intstring(b);
      print_string(s);
      spt->frondtexturemapfile = s;
      break;
    case 14003:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 14004:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 14005:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 14006:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 14007:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 14008:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 15000:
      hierarchy++;
      // XXXX update after parsing spt->num15000 = spt->numbrachlevels;
      spt->idx15000 = 0;
      break;
    case 15001:
      hierarchy--;
      break;
    case 15002:
      n = buffer_read_byte(b);
      print_byte(n);
      spt->val15002[spt->idx15000] = n;
      break;
    case 15003:
      f = buffer_read_float(b);
      print_float(f);
      spt->val15003[spt->idx15000] = f;

      // Assumes that the order is 15002 15003.
      spt->idx15000++;
      break;
    case 16000:
      hierarchy++;
      break;
    case 16001:
      hierarchy--;
      break;
    case 16002:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16003:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 16004:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16005:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16006:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16007:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16008:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16009:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16010:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16011:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16012:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 16013:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 16014:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 18000:
      hierarchy++;
      break;
    case 18001:
      hierarchy--;
      break;
    case 18002:
      for (i = 0; i < 3; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 18003:
      for (i = 0; i < 3; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 18004:
      for (i = 0; i < 3; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 18005:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 19000:
      hierarchy++;
      break;
    case 19001:
      hierarchy--;
      break;
    case 19002:
      n = buffer_read_int(b);
      print_int(n);
      break;
    case 20000:
      hierarchy++;
      break;
    case 20001:
      hierarchy--;
      break;
    case 20002:
      s = buffer_read_intstring(b);
      print_string(s);
      free(s);
      break;
    case 20003:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 20004:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    case 20005:
      for (i = 0; i < 8; i++) {
	f = buffer_read_float(b);
	print_float(f);
      }
      break;
    case 21000:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 21001:
      f = buffer_read_float(b);
      print_float(f);
      break;
    case 22000:
      n = buffer_read_byte(b);
      print_byte(n);
      break;
    default:
      fprintf(stderr,"Unknown section %i --- %s\n",n,av[1]);
    }
  }

  spt->numblock12002s = spt->idxblock12002s;
  spt->numblock12003s = spt->idxblock12003s;
  spt->num15000 = spt->numbrachlevels;

  return 1;
}
