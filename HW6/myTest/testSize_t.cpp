#include <iostream>
using namespace std;
int main()
{
   size_t** id = new size_t* [20];
   for (size_t i=0; i<20; i++){
      id[i] = new size_t [i];
      cout << id[i] << " -> " << ((size_t)id[i]%sizeof(size_t)==0) << endl;
   }
}
