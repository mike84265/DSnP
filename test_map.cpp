#include <iostream>
#include <map>
using namespace std;
typedef map<string, int> myMap;
int main()
{
   myMap _map;
   _map["Mike"] = 180;
   _map["Sophia"] = 165;
   _map["Maurice"] = 170;
   _map["Year"] = 160;
   for (myMap::iterator i=_map.begin(); i!=_map.end(); ++i){
      cout << i->first << " => " << i->second << endl;
   }
}
