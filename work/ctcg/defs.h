/* 
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.1 $ 
** Date: %D%
** Author: Nic Volanski
*/

// definitions for the C part of the specializer (vs the C++ part)

typedef enum {
  // tells whether a 'break'/'continue' exception occured in a sub-tree,
  // and hasn't been consumed by the corresponding loop

  X_NONE=0,
  X_BREAK, 
  X_CONTINUE,
  X_RETURN
} flow_exception;

#ifdef __cplusplus
extern "C" flow_exception global_x;
#else
extern flow_exception global_x;
#endif
