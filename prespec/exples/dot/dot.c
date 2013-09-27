struct vector
{
  int size;
  int *vec;
};
 

int dotproduct( struct vector *u, struct vector *v)
{
  int i = 0;
  int sum = 0;
  if (u->size == v->size)
      while( i < u->size )
        {
              sum = sum + u->vec[i]*v->vec[i];
              i++;
         }
    else perror("sizes not equal");
  return sum;
}
  
