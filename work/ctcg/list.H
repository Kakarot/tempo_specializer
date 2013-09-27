/* 
** Tempo, (C) 1995, Irisa
** Version: $Revision: 1.3 $ 
** Date: %D%
** Author: Nic Volanski
*/

template<class T>
class cons {
  T info;
  cons *next;
public:
  cons(const T& car, const cons *cdr=NULL) : info(car) {
   next=(cons<T> *)cdr;
  }
  T *hd() {return &info;}
  cons *tl() {return next;}
};


/*
template<class T>
cons<T> operator+=(const T& car, const cons<T>& cdr) {
  return cons<T>(car, new cons<T>(cdr));
}

template<class T>
cons<T> operator+=(const T& t1, const T& t2) {
  return cons<T>(t1, new cons<T>(t2));
}

*/
/*
** Example of use:

 class tuple{
   int i;
   float f;
 public:
   tuple(int ii, float ff) : i(ii), f(ff) {}
   void print(){
     cout<<"<"<<i<<","<<f<<">";
   }
 };

 main(){
   cons<tuple> *l; 

   l=new cons<tuple>(tuple(1,3.14),
 	     new cons<tuple>(tuple(2,6.18)));
   while(l) {
     l->hd()->print();
     l=l->tl();
   }
 }
**
*/

