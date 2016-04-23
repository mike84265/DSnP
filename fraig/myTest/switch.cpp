#include <iostream>
#include <iomanip>
using namespace std;
int main()
{
   for (int i=0;i<100;++i){
      switch(i%4){
       case 0: case 1:
         continue;
       case 2:
         cout << "i = " << setw(3) << i;
         break;
       case 3:
         cout << ", i = " << setw(3) << i << endl;
      }
   }
}
