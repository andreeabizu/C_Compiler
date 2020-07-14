int f(int x, int y)
{int i,p;
 p=1;
  for(i=0; i<x;i=i+1)
     {p=p*y;}
    return p;
}

int main()
{int a;
   a=f(5,6);
   put_i(a);   //6^5
    return 0;
}
