#ifndef CIRFRAIG_H
#define CIRFRAIG_H
#include <vector>
#include "cirGate.h"
class Gate
{
 public:
   Gate(CirGate* g) : _gatePtr(g) {}
   ~Gate() {}
   void setVar(Var v) { _var = v; }
   Var getVar() const { return _var; }
   string getTypeStr() const { return _gatePtr->getTypeStr(); }
   unsigned getID() const { return _gatePtr->getID(); }
 private:
   CirGate*       _gatePtr;
   Var            _var;
};
#endif
