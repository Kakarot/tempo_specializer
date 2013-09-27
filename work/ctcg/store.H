/* 
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.6 $ 
** Date: %D%
** Author: Nic Volanski
*/
char *get_crt_store();
char *copy_crt_store();
void install_store(char *);
void install_global_store(char *);
void install_immediate_store(char *);
int equal_store(char *, char *);
int equal_global_store(char *p1, char *p2) ;
int equal_immediate_store(char *p1, char *p2) ;
void store_enter_call();
void store_leave_call();
void free_store(char *p);
