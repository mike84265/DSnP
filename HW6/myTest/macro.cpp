#include <iostream>
#define sizeToPtr(x)       \
(int*)(p+4)
using namespace std;
int main()
{
   int *a = new int [10];
   for (int i=0; i<10; i++)
      a[i] = 2*(i+1);
   size_t p = (size_t)a;
   cout << *sizeToPtr(p) << endl;
}
