int f(int x)
{
if(x<3)
  return x;
 return x*f(x-1);
}

int main()
{
put_i(f(6));
return 0;
}
