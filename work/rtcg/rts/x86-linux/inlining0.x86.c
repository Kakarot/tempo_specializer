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


#ifdef OPTIMIZE_PLUDE

#define EAX 0x01
#define ECX 0x02
#define EDX 0x04
#define EBX 0x08
#define ESP 0x10
#define EBP 0x20
#define ESI 0x40
#define EDI 0x80

#define MAX_CALL_DEPTH 200

struct prelude_pointer {
    char *startaddr;
    int size;
    int dsp;
    int written;
    int read;
};

struct prelude_pointer preludes[MAX_CALL_DEPTH];

struct prelude_pointer *curprelude = preludes;
#endif

#define FIRST_PRELUDE(code_ptr,tmp,sz,dsp) \
{\
    {int my_i; for(my_i=0;my_i<32;my_i++) nop_bytes[my_i] = 0;} \
    buffer_start = spec_ptr = code_ptr = get_code_mem(65536/*4096*/);\
    DEBUG_PRINT("PRELUDE %p..%p >-> %p..", tmp, tmp+sz, code_ptr);\
    memcpy(code_ptr, tmp, sz);\
    code_ptr+=sz;\
    DEBUG_PRINT("%p\n", code_ptr);\
}

#define REST_PRELUDE(code_ptr,tmp,sz,dsp) \
{\
    spec_ptr = code_ptr = get_code_mem(65536/*4096*/);\
    DEBUG_PRINT("PRELUDE %p..%p >-> %p..", tmp, tmp+sz, code_ptr);\
    memcpy(code_ptr, tmp, sz);\
    code_ptr+=sz;\
    DEBUG_PRINT("%p\n", code_ptr);\
}

//#define REST_PRELUDE FIRST_PRELUDE

#ifndef OPTIMIZE_PLUDE
    #define REST_INLINED_PRELUDE(code_ptr,tmp,sz,dsp) \
    {\
        spec_ptr = code_ptr = inline_ptr; \
        /*      83 ec 04                subl   $0x4,%esp */             \
        *code_ptr = 0x83; code_ptr++; *code_ptr = 0xec; code_ptr++; *code_ptr = 0x04; code_ptr++; \
        DEBUG_PRINT("INLINED PRELUDE %p..%p >-> %p..", tmp, tmp+sz, code_ptr); \
        memcpy(code_ptr, tmp, sz);\
        code_ptr+=sz;\
        DEBUG_PRINT("%p\n", code_ptr);\
    }
#else
  #ifdef SAFE_PRELUDESIZE 
    #define REST_INLINED_PRELUDE(code_ptr,tmp,sz,desp)                  \
    {                                                                   \
	int i;							\
	i = persistent_read(prelude_size);				\
	DUMP_PRINTSTACK(inline_ptr,1);					\
        curprelude++;                                                   \
        curprelude->startaddr = spec_ptr = code_ptr = inline_ptr;       \
        curprelude->written=0; curprelude->read=0;                      \
        curprelude->size=i; curprelude->dsp = desp;                     \
	code_ptr += i;							\
    }

  #else
    #ifdef HEUR_TRACE
    #define REST_INLINED_PRELUDE(code_ptr,tmp,sz,desp)                  \
    {                                                                   \
	int i=12;							\
        curprelude++;                                                   \
        curprelude->startaddr = spec_ptr = code_ptr = inline_ptr;       \
        curprelude->written=0; curprelude->read=0;                      \
	i+=sz;								\
        curprelude->size=i; curprelude->dsp = desp;                     \
	code_ptr += i;							\
    }
    #else 
    #define REST_INLINED_PRELUDE(code_ptr,tmp,sz,desp)                  \
    {                                                                   \
	int i=12;							\
        curprelude++;                                                   \
        curprelude->startaddr = spec_ptr = code_ptr = inline_ptr;       \
        curprelude->written=0; curprelude->read=0;                      \
	if(sz==3) { i = 5; }						\
	if(sz==4) { i = 7; }						\
	if(sz==6) { i = 8; }						\
	if(sz==7) { i = 8/*4*/; }					\
	if(sz==8) { i = 5/*2*/; }					\
	if(sz==9) { i = 11/*9*/; }					\
	if(sz==12) { i = 12; }						\
        curprelude->size=i; curprelude->dsp = desp;                     \
	code_ptr += i;							\
    }
    #endif
  #endif
#endif


#define RET 0x0c3

#define FIRST_POSTLUDE_EXTRA(code_ptr,tmp,postlude_sz,resid_params,ret_value,dsp)
#define REST_POSTLUDE_EXTRA(code_ptr,tmp,postlude_sz,resid_params,ret_value,dsp)

#ifndef OPTIMIZE_PLUDE
/* Because procedures are aligned, we have to hunt for the ret instruction.  */
#define REST_INLINED_POSTLUDE(code_ptr,tmp,postlude_sz,resid_params,ret_value,dsp) \
{ \
    DEBUG_PRINT("INLINED POSTLUDE %p..%p >-> %p..", tmp, tmp+postlude_sz, code_ptr);\
    if(postlude_sz==0) { DEBUG_PRINT("NO POSTLUDE\n"); } else { \
    memcpy(code_ptr, tmp, postlude_sz);\
    code_ptr+=postlude_sz;\
    while (*(unsigned char *)code_ptr != RET) ((unsigned char *) code_ptr)--; \
/*    	83 c4 04                addl   $0x4,%esp */			\
  *code_ptr = 0x83; code_ptr++; *code_ptr = 0xc4; code_ptr++; *code_ptr = 0x04; code_ptr++;\
    } \
    DEBUG_PRINT("%p\n", code_ptr);\
}
#else

/*
rc = number of regs to save
r = the current reg
rs[] = regs to save.

r = rc
if(ebp read) {
    if(rc==0) {
PRELUDE:
        pushl %ebp
        leal -4(%esp), %ebp
VARSIZE subl (-dsp) - 8        
POSTLUDE:
        leal 4(%ebp), %esp
        popl %ebp
    } else {
PRELUDE:
        r--;
        pushl rs[r]
        pushl %ebp 
        movl %esp, %ebp 
VARSIZE subl (-dsp) - 4 - 4*r 
        while(rc>0) {
            rc--; 
            pushl rs[rc]
        }
POSTLUDE:
        while(r < rc-1) {
            if(esp written) {
VARSIZE         movl dsp+4+r*4(%ebp), rs[r]
            } else {
                popl rs[r]
            } 
            r++;
        }
        movl %ebp, %esp
        popl %ebp
        popl rs[r( = rc - 1)]
    }   
} else {
PRELUDE :
    while(rc>0) {
        rc--;
        pushl rs[rc]
    }
    if(esp written) {
        pushl %ebp
        movl %esp, %ebp
POSTLUDE:
        movl %ebp, %esp
        popl %ebp
    } 
    while(r<rc) {
        popl rs[r]
        r++;
    }
}
*/

#define PTYPE unsigned char *

#  ifdef PERSISTENCE
extern char *do_rest_inlined_postlude(PTYPE pop, int postlude_sz, int dsp); 
#  else
char *do_rest_inlined_postlude(PTYPE pop, int postlude_sz, int dsp); 
#  endif

int prelude_pw_value = 0;

    #define REST_INLINED_POSTLUDE(pop,tmp,postlude_sz,	 		\
					resid_params,ret_value,dsp)     \
    {                                                                   \
	prelude_pw_value = 0;						\
	pop = do_rest_inlined_postlude(pop, postlude_sz, dsp); 		\
	persistent_write(prelude_size, prelude_pw_value); 		\
    }

//#  ifndef PERSISTENCE
char *do_rest_inlined_postlude(PTYPE pop, int postlude_sz, int dsp) 
    {
	int t, written = curprelude->written, read = curprelude->read;	\
	int rc=0, r;							\
	unsigned char rs[8];							\
	PTYPE prp = curprelude->startaddr;			\
	PTYPE prend = curprelude->startaddr + curprelude->size;	\

INIT_PRELUDE_SIZE 
    prelude_pw_value = 0;
									\
    if(postlude_sz==0) { DEBUG_PRINT("NO POSTLUDE\n"); } \
									\
        /*printf("%p + %p..%p: written: %p, read: %p, desp = %d, prsize=%d\n", \
                buffer_start, curprelude->startaddr - buffer_start      \
                ,pop-buffer_start, written, read, dsp, 	                \
                curprelude->size);*/                                      \
									\
        /* nop marker */						\
	/* *prp = 0x90; prp++;*/						\
        /* nop marker */						\
	/* *pop = 0x90; pop++;*/						\
									\
	if(written & EBX) {						\
	    rs[rc] = 3;							\
	    rc++;							\
 	}								\
									\
	if(written & ESI) {						\
	    rs[rc] = 6;							\
	    rc++;							\
 	}								\
									\
	if(written & EDI) {						\
	    rs[rc] = 7;							\
	    rc++;							\
 	}								\
									\
	r = rc;								\
	if(EBP & read) {						\
    	    if(rc==0) {							\
									\
/*PRELUDE:*/								\
        	/* pushl %ebp */					\
		*prp = 0x55; prp++;					\
        	/* leal -4(%esp), %ebp */				\
		*prp = 0x8d; prp++;					\
		*prp = 0x6c; prp++;					\
		*prp = 0x24; prp++;					\
		*prp = 0xfc; prp++;					\
prelude_pw_value += 5;
		if((-dsp)>8) {						\
		/* VARSIZE subl (-dsp) - 4, %esp */			\
		t = (-dsp) - 4;						\
		if(t>127) {						\
		    *prp = 0x81; prp++;					\
		    *prp = 0xec; prp++;					\
		    *((int *)prp) = t; prp+=4;				\
prelude_pw_value += 6;
		} else {						\
		    *prp = 0x83; prp++;					\
		    *prp = 0xec; prp++;					\
		    *prp = (unsigned char) t; prp++;			\
prelude_pw_value += 3;
		}							\
		}							\
									\
/*POSTLUDE:*/								\
        	/* leal 4(%ebp), %esp */				\
		*pop = 0x8d; pop++;					\
		*pop = 0x65; pop++;					\
		*pop = 0x04; pop++;					\
									\
        	/* popl %ebp */						\
		*pop = 0x5d; pop++;					\
prelude_pw_value += 4;
    	    } else {							\
									\
/*PRELUDE:*/								\
        	r--;							\
									\
        	/* pushl rs[r] */					\
		*prp = 0x50 + rs[r]; prp++;				\
									\
        	/* pushl %ebp */					\
		*prp = 0x55; prp++;					\
									\
        	/* movl %esp, %ebp */					\
		*prp = 0x89; prp++;					\
		*prp = 0xe5; prp++;					\
									\
prelude_pw_value += 4;
		/* VARSIZE subl (-dsp) - 4 - 4*r, esp */		\
		t = (-dsp) - 4 - 4*r;					\
		if(t>127) {						\
		    *prp = 0x81; prp++;					\
		    *prp = 0xec; prp++;					\
		    *((int *)prp) = t; prp+=4;				\
prelude_pw_value += 6;
		} else {						\
		    *prp = 0x83; prp++;					\
		    *prp = 0xec; prp++;					\
		    *prp = (unsigned char) t; prp++;			\
prelude_pw_value += 3;
		}							\
									\
        	while(r>0) {						\
            	    r--; 						\
									\
            	    /* pushl rs[r] */					\
		    *prp = 0x50 + rs[r]; prp++;				\
prelude_pw_value += 1;
        	}							\
									\
/*POSTLUDE:*/								\
	        while(r < rc-1) {					\
            	    if(ESP & written) {					\
									\
			/* VARSIZE movl dsp+4+r*4(%ebp), rs[r] */	\
			t = dsp+4+r*4;					\
			if(t > ~128 && t < 128) {			\
		    	    *pop = 0x8b; pop++;				\
		    	    *pop = 0x45+(rs[r]<<3); pop++;		\
			    *pop = (signed char) t; pop++;		\
 			} else {					\
		    	    *pop = 0x8b; pop++;				\
		    	    *pop = 0x85+(rs[r]<<3); pop++;		\
			    *((int *)pop) = t; pop+= 4;			\
			}						\
									\
            	    } else {						\
									\
                	/* popl rs[r] */				\
			*pop = 0x58 + rs[r]; pop++;			\
            	    } 							\
            	    r++;						\
        	}							\
									\
        	/* movl %ebp, %esp */					\
		*pop = 0x89; pop++;					\
		*pop = 0xec; pop++;					\
									\
        	/* popl %ebp */						\
		*pop = 0x5d; pop++;					\
									\
        	/* popl rs[r( = rc - 1)] */				\
		*pop = 0x58 + rs[r]; pop++;				\
    	    }   							\
	} else {							\
									\
/* PRELUDE : */								\
    	    while(r>0) {						\
        	r--;							\
									\
            	/* pushl rs[r] */					\
		*prp = 0x50 + rs[r]; prp++;				\
prelude_pw_value += 1;
    	    }								\
    	    if(ESP & written) {						\
									\
        	/* pushl %ebp */					\
		*prp = 0x55; prp++;					\
									\
        	/* movl %esp, %ebp */					\
		*prp = 0x89; prp++;					\
		*prp = 0xe5; prp++;					\
prelude_pw_value += 3;
									\
/* POSTLUDE: */								\
									\
        	/* movl %ebp, %esp */					\
		*pop = 0x89; pop++;					\
		*pop = 0xec; pop++;					\
									\
        	/* popl %ebp */						\
		*pop = 0x5d; pop++;					\
    	    } 								\
    	    while(r<rc) {						\
        	/* popl rs[r] */					\
		*pop = 0x58 + rs[r]; pop++;				\
        	r++;							\
     	    }								\
	}								\
									\
	/* prelude_pw_value = (int)(prp-curprelude->startaddr);*/		\

	HEUR_PRINT("Heuristic for %d should at least be  %d\n", curprelude->size - 12, ((int)prp)-((int)(curprelude->startaddr)));
	(nop_bytes[prend-prp])++;  					\
	
									\
        /* nop marker */						\
	/* *pop = 0x90; pop++; */					\
									\
	/* NOP's of different sizes */					\
	while(prp <= prend - 6) { 					\
	    *prp = 0x8d; prp++; 					\
	    *prp = 0xb6; prp++; 					\
	    *prp = 0x00; prp++; 					\
	    *prp = 0x00; prp++; 					\
	    *prp = 0x00; prp++; 					\
	    *prp = 0x00; prp++; 					\
	}								\
	if(prp <= prend - 4) {	 					\
	    *prp = 0x8d; prp++; 					\
	    *prp = 0x74; prp++; 					\
	    *prp = 0x26; prp++; 					\
	    *prp = 0x00; prp++; 					\
	}								\
	if(prp == prend - 3) {						\
	    *prp = 0x8d; prp++;						\
	    *prp = 0x76; prp++;						\
	    *prp = 0x00; prp++;						\
	}								\
	if(prp == prend - 2) {						\
	    *prp = 0x89; prp++;						\
	    *prp = 0xf6; prp++;						\
	}								\
	while(prp <= prend - 1) {					\
	    *prp = 0x90; prp++;						\
	}								\
									\
	curprelude--;							\
	DUMP_PRINTSTACK(pop,2);						\
	return pop;							\
    }									
// #  endif /* PERSISTENCY */
#endif


#ifdef OPTIMIZE_PLUDE
    #define DUMP_TEMPLATE_EXTRA(code_ptr, tmp_ptr, name, size, w, r) \
        { curprelude->written |= w; curprelude->read |= r;/* printf("%p: (%p,%p)\n", code_ptr - buffer_start, w,r); */} 
#else
    #define DUMP_TEMPLATE_EXTRA(code_ptr, tmp_ptr, name, size, w, r) 
#endif

#define INLINE_CALL(begin_ptr, code_ptr, e, ho, nodelay, free_regs) 	\
{									\
  DEBUG_PRINT("INLINE CALL\n");						\
  code_ptr = begin_ptr + ho - 1;					\
  code_ptr = ((char *)(e)); 						\
									\
}\

#endif
