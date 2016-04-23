#include "myHashMap.h"
#include "cirGate.h"
class AIGKey 
{
public:
   AIGKey(AIG* g)
   {
      for (size_t i=0;i<2;++i)
         _fanin[i] = g->getFanin(i);
   }
   ~AIGKey() {}
   size_t operator() () const { return (_fanin[0] + _fanin[1]); } 
   bool operator == (const AIGKey& k) const
   {
      return ( (_fanin[0]==k._fanin[0]  && 
                _fanin[1]==k._fanin[1]) ||
               (_fanin[0]==k._fanin[1]  && 
                _fanin[1]==k._fanin[0])   );
   }
   size_t getFanin(int i) const
   {
      if (i==0 || i==1) return _fanin[i];
      else return 0;
   }
private:
   size_t            _fanin[2];
};

