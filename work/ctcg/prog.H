/* 
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.3 $ 
** Date: %D%
** Author: Nic Volanski
*/
#include <string.h>

#include "actree.H"

stmt_ac *get_fun_body(identifier fname, int index);
void prog_print();
ML_ast print_function(int index, identifier compound_name, ML_ast spec_body);

class fun_ac {
  identifier fname;
  stmt_ac *body;
  ML_ctx ctx;
public:
  fun_ac(identifier name, const stmt_ac& stmt, ML_ctx context) 
    : fname(name), body(stmt.copy()), ctx(context) {}
  int compare(identifier name) {
    return !strcmp(fname, name);
  }
  stmt_ac *get_fun_body() { return body; }
  void print();
  ML_ast pack2(identifier name, stmt body);
};

extern cons<fun_ac> operator+=(const fun_ac&, const cons<fun_ac>&);
extern cons<fun_ac> operator+=(const fun_ac&, const fun_ac&);

