/* Tempo, Copyright INRIA (C) 2002.  All rights reserved. */
#include <stdio.h>
#include <strings.h>

extern void init_rts_alloc();
extern char *get_code_mem(unsigned int sz);
extern char *get_data_mem(unsigned int sz);

#ifndef BUF_CODE_SIZE
#define BUF_CODE_SIZE 1000*4096
#endif
#ifndef BUF_DATA_SIZE
#define BUF_DATA_SIZE 1000*64
#endif

char code_buffer[BUF_CODE_SIZE];
double tmp_data_buffer[BUF_DATA_SIZE];
char *data_buffer = (char *)tmp_data_buffer;

static int data_pos = 0;
static int code_pos = 0;

void init_rts_alloc()
{
  bzero(data_buffer,sizeof(double)*BUF_DATA_SIZE);
  bzero(code_buffer,sizeof(char)*BUF_CODE_SIZE);
  code_pos=0;
  data_pos=0;
}

#define ICALIGN(x) ((char *)(((unsigned int)((x) + 31))&(0xffffffe0)))

char *get_code_mem(unsigned int sz) {
  char *ret;

#ifdef OVERFLOW_CHECK
  if (code_buffer[code_pos] != 0)
    puts("Function overflow.");
#endif

#ifdef ALIGN
  ret=ICALIGN(code_buffer+code_pos);
  code_pos = ret - code_buffer;
#else
  ret=(code_buffer+code_pos);
#endif

  code_pos+=sz;
  if (code_pos>BUF_CODE_SIZE) {
    puts("Code buffer overflow.");
    exit(1);
  }
  return(ret);
}

char *get_data_mem(unsigned int sz)

{
  char *ret;

  ret=data_buffer+data_pos;
  data_pos+=sz;
  if (data_pos>BUF_DATA_SIZE) {
    puts("Data buffer overflow.");
    exit(1);
  }
  return(ret);
}
