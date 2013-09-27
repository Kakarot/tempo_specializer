struct XDR;
struct xdr_ops
{
  int (*x_getlong) (struct XDR *, int *);
  int (*x_putlong) (struct XDR *, int *);
};
struct XDR
{
  int x_op;
  struct xdr_ops *x_ops;
  char *x_public;
  char *x_private;
  char *x_base;
  int x_handy;
};

extern struct xdr_ops xdrmem_ops;

extern int xdrmem_getlong(struct XDR *, int *);
extern int ntohl();
extern int xdrmem_putlong(struct XDR *, int *);
extern int htonl();
extern int xdr_short(struct XDR *, short *);
extern int xdr_long(struct XDR *, int *);
extern int xdr_int(struct XDR *, int *);
extern void xdrmem_create(struct XDR *, char *, int, int);
extern void decode(char *, struct XDR *, int *);
extern int encode(char *, struct XDR *, int);
extern void test(char *, struct XDR *, int);
extern int printf();

struct xdr_ops xdrmem_ops = {xdrmem_getlong, xdrmem_putlong};

extern int
xdrmem_getlong (struct XDR *xdrs, int *lp){
  int *suif_tmp0;
  int suif_tmp1;
  char * *suif_tmp3;

  suif_tmp0 = &(*xdrs).x_handy;
  suif_tmp1 = *suif_tmp0 - 4;
  *suif_tmp0 = suif_tmp1;
  if (suif_tmp1 < 0)
    {
      return 0;
    }
  *lp = ntohl (*((int *)(*xdrs).x_private));
  suif_tmp3 = &(*xdrs).x_private;
  *suif_tmp3 = 4 + *suif_tmp3;
  return 1;
}


extern int
xdrmem_putlong (struct XDR *xdrs, int *lp){
  int *suif_tmp0;
  int suif_tmp1;
  char * *suif_tmp3;

  suif_tmp0 = &(*xdrs).x_handy;
  suif_tmp1 = *suif_tmp0 - 4;
  *suif_tmp0 = suif_tmp1;
  if (suif_tmp1 < 0)
    {
      return 0;
    }
  *((int *)(*xdrs).x_private) = htonl (*lp);
  suif_tmp3 = &(*xdrs).x_private;
  *suif_tmp3 = 4 + *suif_tmp3;
  return 1;
}


extern int
xdr_short (struct XDR *xdrs, short *sp){
  int l;
  int suif_tmp0;

  suif_tmp0 = (*xdrs).x_op;
  if (suif_tmp0 == 0)
    goto L2;
  if (suif_tmp0 == 1)
    goto L3;
  if (suif_tmp0 == 2)
    goto L5;
  goto L1;
  L2: ;
  l = *sp;
  return ((*(*xdrs).x_ops).x_putlong)(xdrs, &l);
  L3: ;
  if ((((*(*xdrs).x_ops).x_getlong)(xdrs, &l) != 0) == 0)
    {
      return 0;
    }
  *sp = l;
  return 1;
  L5: ;
  return 1;
  L1: ;
  return 0;
}


extern int
xdr_long (struct XDR *xdrs, int *lp){
  int suif_tmp0;

  suif_tmp0 = (*xdrs).x_op;
  if (suif_tmp0 == 0)
    goto L2;
  if (suif_tmp0 == 1)
    goto L3;
  if (suif_tmp0 == 2)
    goto L4;
  goto L1;
  L2: ;
  return ((*(*xdrs).x_ops).x_putlong)(xdrs, lp);
  L3: ;
  return ((*(*xdrs).x_ops).x_getlong)(xdrs, lp);
  L4: ;
  return 1;
  L1: ;
  return 0;
}


extern int
xdr_int (struct XDR *xdrs, int *ip){
  return xdr_long (xdrs, ip);
  goto __done2;
  L1: ;
  return xdr_short (xdrs, (short *)ip);
  __done2: ;
}


extern void
xdrmem_create (struct XDR *xdrs, char *addr, int size, int op){
  char *suif_tmp1;

  (*xdrs).x_op = op;
  (*xdrs).x_ops = &xdrmem_ops;
  suif_tmp1 = addr;
  (*xdrs).x_base = suif_tmp1;
  (*xdrs).x_private = suif_tmp1;
  (*xdrs).x_handy = size;
  return;
}


extern void
decode (char *addr, struct XDR *xdrs, int *number){
  xdrmem_create (xdrs, addr, 4, 1);
  xdr_int (xdrs, number);
  return;
}


extern int
encode (char *addr, struct XDR *xdrs, int number){
  int enc_number;

  xdrmem_create (xdrs, addr, 4, 0);
  xdr_int (xdrs, &number);
  enc_number = *((int *)(*xdrs).x_base);
  return enc_number;
}


extern void
test (char *addr, struct XDR *xdrs, int number){
  int enc_number;
  int dec_number;

  printf ("Le nombre est %i\n", number);
  enc_number = encode (addr, xdrs, number);
  printf ("La valeur encodee est : %i (%i)\n", enc_number, ntohl (enc_number));
  decode (addr, xdrs, &dec_number);
  printf ("La valeur decodee est : %i\n", dec_number);
  return;
}



