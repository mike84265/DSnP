#include <sstream>
#include <iostream>
using namespace std;
int main()
{
   stringstream ss, ss2;
   int a=10;
   ss << "\"" << a << "\"" << endl;
   cout << ss.str();
   cout << "ss2->" << ss2.str();
}
