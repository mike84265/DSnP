#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;
int main()
{
   vector<int*> k;
   k.resize(20);
   cout << k.size() << endl;
   for (int i=0; i<20; i++)
      printf("k[%d] = %p\n",i, k[i]);
   printf("size = %zu\n", sizeof(k));
}
