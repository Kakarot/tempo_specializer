/*
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.7 $
** Date: %D%
** Author: Nic Volanski
*/
//
// Actree hierarchy
//
#ifndef _actree_H_
#define _actree_H_

#include "type.H"
#include "list.H"

typedef enum {STAT, DYN} BT;

// Expressions
class exp_ac {
  char *ctx; // a 'printf' pattern with a %s hole
             // in which the (ML) specialized code will be packed
public:
  exp_ac(char *context="Bad_context{%s}!!") : ctx(context) { }
  virtual exp_ac *copy() const = 0;
  virtual exp spec() = 0;
  ML_ast pack0();
  ML_ast pack1(exp e); // fill-in the whole in ctx with e
  ML_ast pack2(exp e, exp e); // fill-in the two wholes in ctx
};

class Exp_ev : public exp_ac { // EVAL(exp)
  exp e;
  ev_type *T;
  funptr f; // the function which evaluates e
public:
  Exp_ev(const ev_type& t, funptr F, ML_ctx ctx)
      : exp_ac(ctx), e(NULL), T(t.copy()), f(F) { ; }
  exp_ac *copy() const { return new Exp_ev(*this); }
  exp spec();
};

class Exp_id : public exp_ac { // ID(exp)
  exp e;
public:
  Exp_id(ML_ctx ctx) : exp_ac(ctx) { }
  exp_ac *copy() const { return new Exp_id(*this); }
  exp spec();
};

class Unop_reb : public exp_ac {
  exp_ac *e;
public:
  Unop_reb(const exp_ac& E, ML_ctx ctx)
    : exp_ac(ctx), e(E.copy()) { }
  exp_ac *copy() const { return new Unop_reb(*this); }
  exp spec();
};

class Binop_reb : public exp_ac {
  exp_ac *e1, *e2;
public:
  Binop_reb(const exp_ac& E1, const exp_ac& E2,
	    ML_ctx ctx)
      : exp_ac(ctx), e1(E1.copy()), e2(E2.copy()) {}
  exp_ac *copy() const { return new Binop_reb(*this); }
  exp spec();
};

class Assign_reb : public exp_ac {
  exp_ac *lhs, *rhs;
public:
  Assign_reb(const exp_ac& l, const exp_ac& r, ML_ctx ctx)
      : exp_ac(ctx), lhs(l.copy()), rhs(r.copy()) { }
  exp_ac *copy() const { return new Assign_reb(*this); }
  exp spec();
};


  class param { // tuple <name, bt, actual>
    BT bt;      // if struct, bt will be the lub of fields;
                // this means that struct are not splitted
    exp_ac *e;
    funptr f;
  public:
    param(const exp_ac& exp) : bt(DYN),
      e(exp.copy()) { f=NULL; }
    param(funptr F) : bt(STAT), f(F) { e=NULL; }

    exp do_it(); // if Stat init the static, (return NULL)
                 // if Dyn rebuild exp (and return it)
  };

cons<param> operator+=(const param& car, const cons<param>& cdr);
cons<param> operator+=(const param& p1, const param& p2);

class Call_reb : public exp_ac {

  cons<exp> *spec_args(cons<param> *); // auxiliary, recursive

  identifier fname;
  int index;  // the actree for fname's body, as an index in the global list.
              // Note that fname and the signature of the call ("SDSS...")
              // do not suffice, because you can have several actree
              // versions, specialized wrt different set of globals.
  cons<param> *args;
public:
  Call_reb(identifier name, cons<param> *l, int ix,
	   ML_ctx ctx) // contexte a 2 trous (fname et liste d'args)
    : exp_ac(ctx), fname(name), index(ix), args(l) { }
  Call_reb(identifier name, const cons<param>& l, int ix,
	   ML_ctx ctx) // contexte a 2 trous (fname et liste d'args)
    : exp_ac(ctx), fname(name), index(ix), args(new cons<param>(l)) { }
  exp_ac *copy() const { return new Call_reb(*this); }
  exp spec();
};

class Ecall_reb : public exp_ac {
  cons<exp_ac *> *args;

  cons<exp> *spec_args(cons<exp_ac *> *); // auxiliary, recursive
public:
  Ecall_reb(cons<exp_ac *> *l, ML_ctx ctx)
    : exp_ac(ctx), args(l) { }
  Ecall_reb(const cons<exp_ac *>& l, ML_ctx ctx)
    : exp_ac(ctx), args(new cons<exp_ac *>(l)) { }
  exp_ac *copy() const { return new Ecall_reb(*this); }
  exp spec();
};

class Struct_reb : public exp_ac {
  exp_ac *e;
  identifier id;
public:
  Struct_reb(const exp_ac& E, ML_ctx ctx)
    : exp_ac(ctx), e(E.copy()) { }
  exp_ac *copy() const { return new Struct_reb(*this); }
  exp spec();
};

class Array_reb : public exp_ac {
  exp_ac *e, *ix;
public:
  Array_reb(const exp_ac& E, const exp_ac& Ix, ML_ctx ctx)
    : exp_ac(ctx), e(E.copy()), ix(Ix.copy()) { }
  exp_ac *copy() const { return new Array_reb(*this); }
  exp spec();
};



// Statements
class stmt_ac {
  char *ctx; // a 'printf' pattern with a %s hole
             // in which the (ML) specialized code will be packed
public:
  int active;

  stmt_ac(char *context="Bad_ctx{%s}!!") : ctx(context) { active=0; }
  virtual stmt_ac *copy() const = 0;
  virtual stmt spec() = 0;
  ML_ast pack0(); // ctx with no hole: return it as is
  ML_ast pack1(stmt s); // fill-in the whole in ctx with s
  ML_ast pack3(exp e, stmt s1, stmt s2); // ctx with 3 holes
};

class Stmt_ev : public stmt_ac { // EVAL(stmt)
  funptr f;
public:
  Stmt_ev(funptr F, ML_ctx ctx) : stmt_ac(ctx), f(F) { }
  stmt_ac *copy() const { return new Stmt_ev(*this); }
  stmt spec();
};

class Stmt_id : public stmt_ac {

public:
  Stmt_id(ML_ctx ctx) : stmt_ac(ctx) { }
  stmt_ac *copy() const { return new Stmt_id(*this); }
  stmt spec();
};

class Exp_reb : public stmt_ac {
  exp_ac *e;
public:
  Exp_reb(const exp_ac& E, ML_ctx ctx)
    : stmt_ac(ctx), e(E.copy()) { }
  stmt_ac *copy() const { return new Exp_reb(*this); }
  stmt spec();
};

class If_red : public stmt_ac {
  stmt_ac *s1, *s2;
  funptr f; // evaluates the test
public:
  If_red(const stmt_ac& S1, const stmt_ac& S2, funptr F, ML_ctx ctx)
    : stmt_ac(ctx), s1(S1.copy()), s2(S2.copy()), f(F) { }
  stmt_ac *copy() const { return new If_red(*this); }
  stmt spec();
};

class If_reb : public stmt_ac {
  exp_ac *e;
  stmt_ac *s1, *s2;
public:
  If_reb(const exp_ac& E, const stmt_ac& S1, const stmt_ac& S2,
	 ML_ctx ctx)
    : stmt_ac(ctx), e(E.copy()), s1(S1.copy()), s2(S2.copy()) { }
  stmt_ac *copy() const { return new If_reb(*this); }
  stmt spec();
};

class Do_reb : public stmt_ac {
  exp_ac *e;
  stmt_ac *s;
public:
  Do_reb(const exp_ac& E, const stmt_ac& S, ML_ctx ctx)
    : stmt_ac(ctx), e(E.copy()), s(S.copy()) { }
  stmt_ac *copy() const { return new Do_reb(*this); }
  stmt spec();
};

class Do_red : public stmt_ac {
  stmt_ac *s;
  funptr tst;
  ML_ctx label_pattern; // with one hole, to be filled with current index
    // used to generate a "continue" target, after each copy of the body

  cons<stmt> *Do_red::spec_seq(); // auxiliary, recursive
public:
  Do_red(const stmt_ac& S, funptr f, ML_ctx lbl, ML_ctx ctx)
    : stmt_ac(ctx), s(S.copy()), tst(f), label_pattern(lbl) { }
  stmt_ac *copy() const { return new Do_red(*this); }
  stmt spec();
};

class Block_reb : public stmt_ac {
  cons<stmt_ac *> *seq;

  cons<stmt> *spec_seq(cons<stmt_ac *> *); // auxiliary, recursive
public:
  Block_reb(cons<stmt_ac *> *l, ML_ctx ctx)
    : stmt_ac(ctx), seq(l) { }
  Block_reb(const cons<stmt_ac *>& l, ML_ctx ctx)
    : stmt_ac(ctx), seq(new cons<stmt_ac *>(l)) { }
  stmt_ac *copy() const { return new Block_reb(*this); }
  stmt spec();
};

class Return_reb : public stmt_ac {
  exp_ac *e;
public:
  Return_reb(const exp_ac& E, ML_ctx ctx)
    : stmt_ac(ctx), e(E.copy()) { }
  stmt_ac *copy() const { return new Return_reb(*this); }
  stmt spec();
};

class Continue_reb : public stmt_ac {

public:
  Continue_reb(ML_ctx ctx)
    : stmt_ac(ctx) { }
  stmt_ac *copy() const { return new Continue_reb(*this); }
  stmt spec();
};


extern cons<stmt_ac *> operator*=(const stmt_ac&, const cons<stmt_ac *>&);
extern cons<stmt_ac *> operator*=(const stmt_ac&, const stmt_ac&);
extern cons<exp_ac *> operator*=(const exp_ac&, const cons<exp_ac *>&);
extern cons<exp_ac *> operator*=(const exp_ac&, const exp_ac&);

#endif _actree_H_
