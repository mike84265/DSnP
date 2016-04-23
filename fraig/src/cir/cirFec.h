#ifndef CIRFRAIG_H
#define CIRFRAIG_H
#include "myHashMap.h"
#include "cirGate.h"
#include "sat.h"
#include <queue>
#include <vector>
#define QUEUE
class FecGroupList
{
 public:
   FecGroupList() {} 
   ~FecGroupList() { reset(); }
   void update();
   void updateFecGroupInGates();
   void reset();
   bool empty() { return _FecGroupList.empty(); }
   void initialize(const GateList& gList, const CirGate* const_gate);
   size_t numGroups () const { return _FecGroupList.size(); }
   void print();
   void fraig(SatSolver&);
 private:
   #ifdef VECTOR
   vector<FecGroup>           _FecGroupList;
   #else
   queue<FecGroup>            _FecGroupList;
   #endif
};

class FecKey
{
 public:
   FecKey(size_t g);
   FecKey(const FecKey& k) : _value(k._value) {}
   size_t operator() () const { return (size_t)_value; }
   bool operator== (const FecKey& k) { return _value==k._value; }
   FecKey operator~ ();
 private:
   PatternType                _value;
};
#endif
