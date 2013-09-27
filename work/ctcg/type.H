/* 
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.4 $ 
** Date: %D%
** Author: Nic Volanski
*/
// type returned by en EV expression. 
// Can't be pointer or lval or void, or array or struct
#ifndef _type_H_
#define _type_H_

#include "defs.H"
#include "ast.H"

//typedef enum {CST, NON_CST} cst_spec;
//typedef enum {VOL, NON_VOL} vol_spec;
// left out for now


typedef enum {SIGNED=0, UNSIGNED} sign_spec;
typedef enum {SHORT=0, STD, LONG, LONG_LONG} size_spec;

class ev_type {
public:
  virtual ev_type *copy() const = 0;
  virtual exp call_f(funptr) = 0;
};

class Char_spec : public ev_type {
  sign_spec sgn;
public:
  ev_type *copy() const {
    return new Char_spec(*this);
  }
  Char_spec(sign_spec s=SIGNED) {
    sgn=s;
  }
  exp call_f(funptr);
};

class String_spec : public ev_type {
public:
  ev_type *copy() const {
    return new String_spec(*this);
  }
  String_spec() { }
  exp call_f(funptr);
};

class Int_spec : public ev_type {
  sign_spec sgn;
  size_spec size;
public:
  Int_spec(sign_spec sign=SIGNED, size_spec sz=STD) : 
    sgn(sign), size(sz) { }
  ev_type *copy() const {
    return new Int_spec(*this);
  }
  exp call_f(funptr);
};

class Float_spec : public ev_type {
  size_spec size; // long float = double (?)
public:
  Float_spec(size_spec sz=SHORT) : size(sz) { }
  ev_type *copy() const {
    return new Float_spec(*this);
  }
  exp call_f(funptr);
};

#endif _type_H_
