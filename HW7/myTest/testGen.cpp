#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
using namespace std;
#define my_srandom srandom
#define my_random random
class RandomNumGen
{
public:
   RandomNumGen() { my_srandom(getpid()); }
   RandomNumGen(unsigned seed) { my_srandom(seed); }
   const int operator() (const int range) const {
      return int(range * (double(my_random()) / INT_MAX));
   }
};
int main()
{
   ofstream of("do10",ios::out);
   RandomNumGen rn(time(NULL));
   string s;
   int dataNum;
   cout << "Num of data?";
   cin >> dataNum;
   int base = dataNum>101? 101 : dataNum;
   of << "taski " << dataNum/3 << endl;
   for (int i=0;i<dataNum;++i){
      s.clear();
      for (int j=0;j<5;++j)
         s.push_back('a'+rn(26));
      of << "taskn -n " << s << " " << dataNum*rn(10)+i << endl;
   }
   of << "taskr -r " << dataNum/5 << endl;
   of << "taskq -min" << endl;
   of << "taskq -hash" << endl;
   of << "taskq -heap" << endl;
   of << "taska 350 -r 200" << endl;
   of << "taskq -hash" << endl;
   of << "taska 1000 -r 20" << endl;
   of << "taskq -hash" << endl;
   of << "taskq -heap" << endl;
   of << "q -f" << endl;
   of << "q -f" << endl;
   of.close();
}
