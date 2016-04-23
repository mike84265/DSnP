#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
int main(int argn, char** argc)
{
   if (argn<3){
      cout << "Input two file names!\n";
      return 0;
   }
   ifstream if1(argc[1]), if2(argc[2]);
   ofstream of( argn>=4? argc[3] : "diff");
   of << "File 1 : " << argc[1] << endl;
   of << "File 2 : " << argc[2] << endl;
   string s1, s2;
   size_t lineNum = 0, count = 0;
   while (!if1.eof() && !if2.eof()){
      getline(if1,s1);
      getline(if2,s2);
      if (s1.compare(s2) != 0){
         of << lineNum << ":\ns1: " << s1
            << "\ns2: " << s2 << "\n\n";
         ++count;
      }
      lineNum++;
   }
   of << lineNum << " lines comparison completed."
      << endl;
   cout << lineNum << " lines comparison completed."
        << endl;
   cout << "The result is stored in \"" << 
      (argn>=4? argc[3] : "diff") << "\".\n";
   cout << count << " difference found.\n";
   return 0;
}
