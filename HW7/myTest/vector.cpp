#include <iostream>
#include <cstdio>
#include <vector>
using namespace std;
int main()
{
   vector<int>* p;
   p = new vector<int> [10];
   for (int i=0;i<10;++i)
      printf("&p[%d] = %p\n",i,p+i);
   printf("--------------------------------\n");
   for (int i=0;i<10;++i)
      for (int j=0;j<10*(i+1);++j)
         p[i].push_back(j+1);
   for (int i=0;i<10;++i)
      printf("&p[%d] = %p\n",i,p+i);
   printf("--------------------------------\n");
   for (int i=0;i<10;++i)
      printf("(p+%d)->size() = %lu\n",i, (p+i)->size());
   printf("--------------------------------\n");
   vector<int> v = *(p+5);
   for (int i=0;i<v.size(); ++i)
      printf("%3d, ",v[i]);
   ++(&v);
   for (int i=0;i<v.size(); ++i)
      printf("%3d, ",v[i]);

}
