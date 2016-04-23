#include <iostream>
#include <queue>
using namespace std;
class myInt
{
 public:
   myInt(int i) : _val(i) {}
   ~myInt() { _val = 0; }
   myInt operator*(int n){ return myInt(_val*n); }
   myInt& operator= (const myInt& n) { _val = n._val; return *this;}
   int _val;
};
int main()
{
   queue<myInt> q;
   for (int i=1;i<5;++i){
      myInt n(i);
      q.push(n);
   }
//   for (int i=0;i<1024;++i){
//      int k = q.front();
//      q.pop();
//      cout << k << "\t";
//      for (int j=0;j<k;++j){
//         q.push(k*k+j);
//      }
//   }
   for (int i=0;i<q.size();++i){
      myInt &k = q.front();
      k = k * 2;
      cout << (q.front())._val << endl;
      q.pop();
      q.push(k);
   }
   for (int i=0;i<q.size();++i){
      myInt &k = q.front();
      cout << (q.front())._val << endl;
      q.pop();
      q.push(k);
   }
}
