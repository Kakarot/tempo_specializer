int power(int base, int expon)
{
  int accum = 1;
  while (expon > 0) {
    accum *= base;
    expon--;
  }
  return(accum);
}
