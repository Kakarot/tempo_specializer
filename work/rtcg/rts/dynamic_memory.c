/* Tempo, Copyright INRIA (C) 2002.  All rights reserved. */
extern char *malloc(int);
extern char *get_code_mem(unsigned int sz);
extern char *get_data_mem(unsigned int sz);

char *get_code_mem(unsigned int sz) {
  return malloc(sz);
}

char *get_data_mem(unsigned int sz) {
  return malloc(sz);
}

/* the following does nothing, but is useful for compatibility with
static_memory.c */
extern void init_rts_alloc();
void init_rts_alloc() {}

