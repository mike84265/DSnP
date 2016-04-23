#include <string>
#include <iostream>
#include <fstream>
using namespace std;
int main()
{
   ifstream ifs("input");
   string s;
   int count = 0;
   while (!ifs.eof()){
      getline(ifs, s);
      cout << ++count << "=> size=" << s.size() 
           << "=>" << s << endl;
   }
   ifs.close();
}
