#include <iostream>
#include <iomanip>
using namespace std;
int main()
{
   for (int i=0;i<(1<<25);++i){
      cout << "Counter: " << setw(8) << i << '\r';
   }
}
