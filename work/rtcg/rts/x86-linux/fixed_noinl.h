#ifndef OPTION
#define OPTION
extern int persistent_read(int);
extern void persistent_write(int, int);
extern unsigned char persistent_ifread(unsigned char);
extern void persistent_ifwrite(unsigned char, unsigned char);
extern unsigned int persistent_bitread(unsigned int);
extern void persistent_bitwrite(unsigned int, unsigned int);

/* inlining, no save/restore removal */

#define dyn(x) x
#define GLOBAL_DECLS
#define DEBUG(s)

#define PARAMS
#define INLINED_PARAMS

#define ARGS
#define INLINED_ARGS

#define FIRST_LOCAL_DECLS
#define REST_LOCAL_DECLS

#define OPTIMIZE_PLUDE
//#define SAFE_PRELUDESIZE

//#define HEUR_TRACE

#ifdef HEUR_TRACE
#define HEUR_PRINT(...) printf(__VA_ARGS__)
#else 
#define HEUR_PRINT(...) 
#endif


//#define PERSISTENCE

#ifdef PERSISTENCE 
#  define REST_INLINED_LOCAL_DECLS int prelude_size = 0; 
#  define INIT_PRELUDE_SIZE 
/*prelude_size = 0;*/
#  define SAFE_PRELUDESIZE
#else
   /* The maximum number of bytes used for prelude is 12 */	
#  define REST_INLINED_LOCAL_DECLS 
#  define INIT_PRELUDE_SIZE 
#  define persistent_read(...) 12
#  define prelude_size 0
#  define persistent_write(...)
#  define REST_INLINED_LOCAL_DECLS
#endif


#include "mydebug.h"

//#define RUNTIMEDEBUG

#ifdef RUNTIMEDEBUG
void debugprintstack(int type);

#define DUMP_PRINTSTACK(code_ptr, type) {	\
 /* pushl type */				\
 *code_ptr = 0x68; code_ptr++;			\
 *((int *)code_ptr) = type; code_ptr += 4;	\
 /* call debugprintstack */				\
 *code_ptr = 0xe8; code_ptr++;			\
 *((int *)code_ptr) = ((int)debugprintstack) - ((int)(code_ptr+4)); code_ptr += 4;\
 /* addl 4, esp */				\
 *code_ptr = 0x83; code_ptr++;			\
 *code_ptr = 0xc4; code_ptr++;			\
 *code_ptr = 0x4; code_ptr++;			\
}

#else

#define DUMP_PRINTSTACK(a, b)

#endif

char *buffer_start;
int nop_bytes[32];


#define FIRST_PRELUDE(code_ptr,tmp,sz) \
{\
    {int my_i; for(my_i=0;my_i<32;my_i++) nop_bytes[my_i] = 0;} \
    buffer_start = spec_ptr = code_ptr = get_code_mem(65536/*4096*/);\
    DEBUG_PRINT("PRELUDE %p..%p >-> %p..", tmp, tmp+sz, code_ptr);\
    memcpy(code_ptr, tmp, sz);\
    code_ptr+=sz;\
    DEBUG_PRINT("%p\n", code_ptr);\
}

#define REST_PRELUDE(code_ptr,tmp,sz) \
{\
    spec_ptr = code_ptr = get_code_mem(65536/*4096*/);\
    DEBUG_PRINT("PRELUDE %p..%p >-> %p..", tmp, tmp+sz, code_ptr);\
    memcpy(code_ptr, tmp, sz);\
    code_ptr+=sz;\
    DEBUG_PRINT("%p\n", code_ptr);\
}



#define RET 0x0c3

#define FIRST_POSTLUDE_EXTRA(code_ptr,tmp,postlude_sz,resid_params,ret_value)
#define REST_POSTLUDE_EXTRA(code_ptr,tmp,postlude_sz,resid_params,ret_value)
    #define DUMP_TEMPLATE_EXTRA(code_ptr, tmp_ptr, name, size, w, r)

#define PTYPE unsigned char *

#include "rts_noinl.skel.macros.x86.c"
#endif
