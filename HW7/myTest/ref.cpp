#include <iostream>
#include <cassert>
using namespace std;
class intSequence
{
public:
   intSequence() {
      for (int i=0; i<100; ++i)
         arr[i] = i;
   }
   bool find(int& n){
      for (int i=0;i<100;++i)
         if (n == arr[i]){
            n = arr[i];
            return true;
         }
      return false;
   }
   int getElement(int n) const { return arr[n]; }
private:
   int arr[100];
};
int main()
{
   int test=5;
   cout << &test << endl;
   intSequence s;
   cout << s.getElement(5) << endl;
   assert(s.find(test));
   cout << &test << endl;
   test*=2;
   cout << s.getElement(5) << endl;
}
