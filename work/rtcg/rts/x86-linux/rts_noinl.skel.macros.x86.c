/*
** Tempo, copyright (C) Irisa 95
**
** file/version: @(#)@(#)$Id: rts.skel.macros.x86.c,v 1.1.1.1 2000/08/22 14:00:31 jll Exp $
** author(s):    F. Noel P. Boinot
** date:         $Date: 2000/08/22 14:00:31 $
*/


#define _SIZE_T
typedef unsigned int	size_t;
#include <malloc.h>
/* These files cause duplicate definitions if the program specialized includes
   the same header files. */
/*
#include <sys/types.h>
#include <stdio.h>
*/

#include "mydebug.h"

#define ENTRIES_PER_NODE 32
typedef struct temp_list_s {
  int no;
  char *entry[ENTRIES_PER_NODE];
  struct temp_list_s *next,*hnext;
} list_t;

static list_t *templists=0;

/* include the basic "option" file.  Normally rtcg_top_func has already
   included something else */
#include "basic.c"


/*-----------------------------------------------------------------------
**  Recopie d'un template. L'adresse d'implatation est memorisee dans le
**  pointeur de relocation du template. La nullite de ce dernier indique
**  si le template a ete implante (indispensable pour la relocation des
**  branchements avantsi).
-----------------------------------------------------------------------*/
/*
void dump_line(char *pt_code)
{
  int i;
  printf ("%x : ",pt_code);
  for (i=0;i<20;i++) printf ("%x ",*((unsigned char *) (pt_code+i)));
  printf ("\n");
}

*/

#define DUMP_TEMPLATE(code_ptr, tmp_ptr, name, size, written, read)\
{\
  DEBUG_PRINT("DUMP TEMPLATE " #name " %p..%p >-> %p..", name, name+size, code_ptr);\
  DUMP_TEMPLATE_EXTRA(code_ptr, tmp_ptr, name, size, written, read)\
  memcpy(code_ptr,name,size);\
  tmp_ptr   = code_ptr;\
  code_ptr += size;\
  DEBUG_PRINT("%p\n", code_ptr);\
}

#define DUMP_TEMPLATE_MP(code_ptr, tmp_lst_ptr, tmp_ptr, name, size, written, read)\
{\
  DEBUG_PRINT("DUMP TEMPLATE MP %p..", code_ptr);\
  DUMP_TEMPLATE_EXTRA(code_ptr, tmp_ptr, name, size, written, read);\
  if (tmp_ptr) ins_temp((list_t **) &tmp_lst_ptr,tmp_ptr); \
  memcpy(code_ptr,name,size);\
  tmp_ptr   = code_ptr;\
  code_ptr += size;\
  DEBUG_PRINT("%p\n", code_ptr);\
}


#define FIRST_POSTLUDE(code_ptr,tmp,postlude_sz,resid_params,ret_value) { \
    DEBUG_PRINT("FIRST POSTLUDE %p..%p >-> %p..", tmp, tmp+postlude_sz, code_ptr);\
    FIRST_POSTLUDE_EXTRA(code_ptr,tmp,postlude_sz,resid_params,ret_value) \
    memcpy(code_ptr, tmp, postlude_sz);\
    code_ptr += postlude_sz;\
    DEBUG_PRINT("%p\n", code_ptr);\
    printf("Generated %d byte\n", code_ptr-buffer_start); \
    {int i, j=0; for(i=0;i<32;i++) {if(nop_bytes[i]) \
    printf("%d %d-byte nopblocks\n", nop_bytes[i], i); j+=nop_bytes[i]*i;} \
    printf("Calculated two-pass size: %d\n", code_ptr-buffer_start-j); } \
} \

#define REST_POSTLUDE(code_ptr,tmp,postlude_sz,resid_params,ret_value) {\
 DEBUG_PRINT("POSTLUDE %p..%p >-> %p..", tmp, tmp+postlude_sz, code_ptr);\
 REST_POSTLUDE_EXTRA(code_ptr,tmp,postlude_sz,resid_params,ret_value)\
 memcpy(code_ptr, tmp, postlude_sz);\
 code_ptr += postlude_sz;\
 DEBUG_PRINT("%p\n", code_ptr);\
} \

/*-----------------------------------------------------------------------
**    Patche un trou
-----------------------------------------------------------------------*/
#define PATCH_HOLE_UNDEFINED(tmp_ptr, e, ho, lo, of)\
{\
 DEBUG_PRINT("PATCH HOLE UNDEFINED\n");\
 *((int *) (tmp_ptr+ho))=e+of;\
}

#define PATCH_HOLE_BYTE(tmp_ptr, e, ho, lo, of)\
{\
 DEBUG_PRINT("PATCH HOLE BYTE\n");\
 *((char *) (tmp_ptr+ho))=e+of;\
}

#define PATCH_HOLE_WORD(tmp_ptr, e, ho, lo, of)\
{\
 DEBUG_PRINT("PATCH HOLE WORD\n");\
 *((short *) (tmp_ptr+ho))=e+of;\
}

#define PATCH_HOLE_DWORD(tmp_ptr, e, ho, lo, of)\
{\
 DEBUG_PRINT("PATCH HOLE DWORD tmp_ptr=%p, e=%x, ho=%x, lo=%x, of=%x\n", tmp_ptr, e, ho, lo, of); \
 *((int *) (tmp_ptr+ho))=e+of;\
}

/*----------------------------------------------------------------------
**  Patche un trou d'appel  (+4 i.e. taille trou)

----------------------------------------------------------------------*/

#define PATCH_CALL_HOLE(tmp_ptr, e, ho, free_regs) \
 {  unsigned long addr = (unsigned long)e;\
 DEBUG_PRINT("PATCH CALL HOLE\n");\
 *((unsigned long *) (tmp_ptr+ho))=(addr-(unsigned long) (tmp_ptr+ho+4));}

/*----------------------------------------------------------------------
**  Relocation d'un appel de bibliotheque
----------------------------------------------------------------------*/
#define PATCH_LIB_CALL(tmp_ptr,name,offset) \
{ \
  DEBUG_PRINT("PATCH LIB CALL, tmp_ptr=%p, name=%p, offset=%p, *tmp+off=%p\n", tmp_ptr, name, offset,  *((char **)  (((char *)tmp_ptr) + offset)) ); \
*((unsigned long *) (tmp_ptr+offset))= ((unsigned long) (*((unsigned long *) (tmp_ptr+offset))+ ((char *)name-tmp_ptr)));\
}

/*
  DEBUG_PRINT("PATCH LIB CALL done  pos + *tmp+off=%p\n", tmp_ptr + offset + (*((long *)(((char *)tmp_ptr) + offset)))+4 ); \
*((unsigned long *) (tmp_ptr+offset))= ((unsigned long) (*((unsigned long *) (tmp_ptr+offset))+ ((char *)name-tmp_ptr))) - 1;\
  *((long *) (tmp_ptr+offset)) = *((long *) (tmp_ptr+offset))+ \
	(((char *)name)-tmp_ptr)); \ 
     *((long *) (((char *)tmp_ptr) + offset)) +=  \
	((char *) name - (char *)tmp_ptr) + offset + 4; \
  *((unsigned long *) (tmp_ptr+offset))= (long) ((long) (*((long *) (tmp_ptr+offset))+ ((char *)name-(tmp_ptr+offset-1)));  \ 
{ \
 DEBUG_PRINT("PATCH LIB CALL\n"); \
}
*/


/*----------------------------------------------------------------------
**  Relocation d'un branchement intertemplate.
**  La deuxieme macro doit etre utilisee pour un branchement avant car
**  il convient de tester si le brancheur a ete implante avant
**  de patcher!!!
-----------------------------------------------------------------------*/

#define PATCH_BRANCH_BYTE(code_ptr, stmp_ptr, bo, rtmp_ptr, to)		\
{									\
    int diff;								\
    diff = (int)(rtmp_ptr-stmp_ptr)+(to-(bo+1));			\
									\
    DEBUG_PRINT("PATCH BRANCH BYTE: stmp_ptr=%d, bo=%d, rtmp_ptr=%d, " 	\
		"to=%d, new=%d.\n", (int)stmp_ptr, (int)bo, 		\
		(int)rtmp_ptr, (int)to, diff);				\
    if(diff > 127  || diff < ~127) {					\
	/* EB 05 E9 XX XX XX XX= jmp +5; jmp +XXXXXXXX */		\
			  /*   ______patch pos________  instr */	\
	*(char *)(stmp_ptr+bo)=code_ptr-(stmp_ptr+bo)+2 -1;		\
	*code_ptr = 0xEB; code_ptr++; *code_ptr = 5; code_ptr++;	\
	*code_ptr = 0xE9; code_ptr++; 					\
	*(signed int *)code_ptr =(rtmp_ptr+to)-(code_ptr+4);		\
	code_ptr += 4;							\
    } else {								\
    	*(char *)(stmp_ptr+bo)=(rtmp_ptr-stmp_ptr)+(to-(bo+1));		\
    }									\
}

#define BACK_PATCH_BRANCH_BYTE(code_ptr, stmp_ptr, bo, rtmp_ptr, to) PATCH_BRANCH_BYTE(code_ptr, stmp_ptr, bo, rtmp_ptr, to)

#define BACK_PATCH_BRANCH_BYTE_MP(code_ptr, stmp_lst,stmp_ptr,bo,rtmp_ptr,to) \
{\
  DEBUG_PRINT("BACK PATCH BRANCH BYTE\n");\
  back_patch_lst_byte(code_ptr, (list_t *) stmp_lst,bo,rtmp_ptr,to); \
 *(char *)(stmp_ptr+bo)=(rtmp_ptr-stmp_ptr)+(to-(bo+1));\
}

#define BACK_PATCH_BRANCH_BYTE_COND(code_ptr, stmp_ptr, bo, rtmp_ptr, to)\
{\
  DEBUG_PRINT("BACK PATCH BRANCH BYTE COND\n");\
  if ( stmp_ptr ) PATCH_BRANCH_BYTE(code_ptr, stmp_ptr, bo, rtmp_ptr, to);\
}
#define BACK_PATCH_BRANCH_BYTE_COND_MP(code_ptr, stmp_ptr, bo, rtmp_ptr, to)\
{\
  DEBUG_PRINT("BACK PATCH BRANCH BYTE COND MP\n");\
  if ( stmp_ptr ) PATCH_BRANCH_BYTE_MP(code_ptr, stmp_ptr, bo, rtmp_ptr, to);\
}

#define PATCH_BRANCH_DWORD(code_ptr, stmp_ptr, bo, rtmp_ptr, to)\
{\
  DEBUG_PRINT("PATCH BRANCH DWORD\n");\
 *(unsigned int *)(stmp_ptr+bo)=(rtmp_ptr-stmp_ptr)+(to-(bo+4));\
}

#define BACK_PATCH_BRANCH_DWORD(code_ptr, stmp_ptr, bo, rtmp_ptr, to)\
    PATCH_BRANCH_DWORD(code_ptr, stmp_ptr, bo, rtmp_ptr, to)
#define BACK_PATCH_BRANCH_DWORD_MP(code_ptr, stmp_lst, stmp_ptr, bo, rtmp_ptr, to)\
{\
  DEBUG_PRINT("BACK PATCH BRANCH DWORD\n");\
  back_patch_lst_dword(code_ptr, (list_t *) stmp_lst,bo,rtmp_ptr,to); \
 *(unsigned int *)(stmp_ptr+bo)=(rtmp_ptr-stmp_ptr)+(to-(bo+4));\
}

#define BACK_PATCH_BRANCH_DWORD_COND(code_ptr, stmp_ptr, bo, rtmp_ptr, to)\
{\
  DEBUG_PRINT("BACK PATCH BRANCH DWORD COND\n");\
  if ( stmp_ptr ) PATCH_BRANCH_DWORD(code_ptr, stmp_ptr, bo, rtmp_ptr, to);\
}
#define BACK_PATCH_BRANCH_DWORD_COND_MP(code_ptr, stmp_ptr, bo, rtmp_ptr, to)\
{\
  DEBUG_PRINT("BACK PATCH BRANCH DWORD COND MP\n");\
  if ( stmp_ptr ) PATCH_BRANCH_DWORD_MP(code_ptr, stmp_ptr, bo, rtmp_ptr, to);\
}

static void ins_temp(list_t **l, char *temp_ptr)

{
  list_t *node;

  if ((*l)&&((*l)->no<ENTRIES_PER_NODE)) {
    (*l)->entry[(*l)->no]=temp_ptr;
    (*l)->no++;
  } else {
    node=malloc(sizeof(list_t));
    node->entry[0]=temp_ptr;
    node->no=1;
    node->next=*l;
    *l=node;
    node->hnext=templists;
    templists=node;
  }
}

static void back_patch_lst_byte(char *code_ptr, list_t *l, int bo, char *rtmp_ptr, int to)
{
  int i;

  while (l) {
    for (i=0; i<l->no; i++) {
      DEBUG_PRINT("bytebackpatching...\n");
      BACK_PATCH_BRANCH_BYTE(code_ptr, l->entry[i], bo, rtmp_ptr, to);
    }
    l=l->next;
  }
}

static void back_patch_lst_dword(char *code_ptr, list_t *l, int bo, char *rtmp_ptr, int to)

{
  int i;

  while (l) {
    for (i=0; i<l->no; i++) {
      BACK_PATCH_BRANCH_DWORD(code_ptr, l->entry[i], bo, rtmp_ptr, to);
    }
    l=l->next;
  }
}

void rtcg_cleanup()

{
  list_t *junk;
  while (templists) {
    junk=templists;
    templists=templists->hnext;
    free(junk);
  }
}
