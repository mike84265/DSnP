#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
typedef unsigned PatternType;
bool readPattern(ifstream&, vector<PatternType>&, unsigned);
void print_binary(PatternType p);
int main()
{
   cout << "Enter pattern name: " ;
   string s;
   cin >> s;
   ifstream ifs(s);
   if (!ifs) cout << "File name invalid!!!" << endl;
   vector<PatternType> pattern;
   s.clear();
   getline(ifs,s);
   unsigned inputNum = s.size();
   ifs.seekg(0,ifs.beg);
   while (readPattern(ifs,pattern,inputNum)){
      for (int i=0;i<inputNum;++i)
         print_binary(pattern[i]);
      if (ifs.eof()) break;
   }
}

bool readPattern(ifstream& ifs, 
      vector<PatternType>& pattern, unsigned length)
{
   vector<string> vs;
   string s;
   pattern.clear();
   for (int i=0;i<8*sizeof(PatternType);++i){
      ifs >> s;
      if (ifs.eof()) break;
      if (s.length() != length){
         cout << "Pattern length not correct!!!" << endl;
         cout << "Expect: " << length << ", read " << s.size() << endl;
         return false;
      }
      vs.push_back(s);
   }
   pattern.resize(length);
   #ifdef DEBUG
   cout << "Before:" << endl;
   for (int i=0;i<length;++i)
      print_binary(pattern[i]);
   cout << "--------------------------" << endl;
   #endif
   for (int i=0;i<vs.size();++i)
      for (int j=0;j<length;++j){
         if (!(vs[i][j] == '0' || vs[i][j] == '1')){
            cout << "Invalid character found: " << vs[i][j] << " !!!" << endl;
            return false;
         }
         pattern[j] |= ((PatternType)(vs[i][j]-'0') << (8*sizeof(PatternType)-i-1));
      }
   #ifdef DEBUG
   cout << "Return true" << endl;
   for (int i=0;i<length;++i)
      print_binary(pattern[i]);
   cout << "--------------------------" << endl;
   #endif
   return true;
}

void print_binary(PatternType p)
{
   for (int i=8*sizeof(PatternType)-1; i>=0;--i){
      cout << ((p & (1<<i)) >> i);
      if (i%4 == 0) cout << " ";
   }
   cout << endl;
}
