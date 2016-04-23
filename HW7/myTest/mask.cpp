#include <iostream>
using namespace std;
int main()
{
   int* a = new int [10];
   a[0] = 13;
   a = (int*)((size_t)a | (size_t)0x1);
   cout << a << endl;
   a = (int*)((size_t)a & ~(size_t)0x1);
   cout << a << endl;
   cout << *a << endl;

}
