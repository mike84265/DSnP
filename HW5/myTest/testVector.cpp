#include <iostream>
#include <vector>
using namespace std;
int main()
{
   vector<char*> v1;
   vector<char*> v2(v1);
   vector<char*> v3 = v1;
}
