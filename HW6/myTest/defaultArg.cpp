#include <iostream>
using namespace std;
class A
{
public:
   virtual void foo(int i=0) const = 0;
};

class B : public A
{
public:
   void foo(int i) const
   { cout << i << endl; }
};

int main()
{
   A* aPtr = new B();
   aPtr->foo();
   aPtr->foo(6);
}
