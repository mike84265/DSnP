/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <climits>
#include <algorithm>
#include "cirDef.h"
#include "sat.h"
#define toPtr(p)                    \
((CirGate*)(p & ~size_t(0x1)))
#define toAig(p)                    \
((AIG*)(p & ~(size_t)0x1))
#define INV (size_t)(0x1)
#define USED (1<<30)
typedef unsigned PatternType;

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(unsigned l, unsigned ID) : 
      _lineNo(l), _id(ID), _value(0) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual GateType getType() const = 0;
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return _lineNo; }
   unsigned getID() const { return (_id & ~USED); }
   virtual bool floatingFanin() const = 0;
   virtual bool noFanout() const = 0;
   PatternType getValue() const { return _value; }

   // Printing functions
   virtual void printGate() = 0;
   virtual void reportGate() const = 0;
   virtual void reportFanin(int level, bool inv=false) = 0;
   virtual void reportFanout(int level, bool inv=false) = 0;
   virtual void setFanin(size_t fi=ULONG_MAX) = 0;
   virtual void setFanout(size_t fo) = 0;
   virtual void sortFanout() = 0;

   // Delete and Renew functions used for optimization
   virtual void clearFanInOut() = 0;
   virtual void deleteFanin(CirGate* fi) = 0;
   virtual void deleteFanout(CirGate* fo) = 0;
   virtual void renewFanin(CirGate*, size_t) = 0;
   virtual const vector<size_t>& getFanout() const = 0; 

   // Printing parameters
   static void resetPrintOrder() { _printOrder = 0; }
   static void setGlobalref() { ++_globalref; }
   static void resetIndent() { _indent = ""; }
   void setRef() const { _ref = _globalref; }
   unsigned getRef() const { return _ref; }
   bool refMatch() const { return _ref == _globalref; }

   bool isAig() const { return this->getTypeStr() == "AIG"; } 
   void setUsed() { _id |= USED; }
   void setUnused() { _id &= ~USED; }
   bool getUsed() const { return ((_id&USED)!=0); }

   // Functions for constructing FEC groups:
   virtual void setFecGroup(const IGateList& l, bool inv) = 0;
   virtual void clearFecGroup() = 0;

   // Functions for cirFraig:
   virtual void setVar(const Var& v) = 0;
   virtual Var getVar() const = 0;

private:

protected:  
   unsigned                _lineNo;
   unsigned                _id;
   static unsigned         _globalref;
   static unsigned         _printOrder;
   static string           _indent;
   mutable unsigned        _ref;
   PatternType             _value;
};

class PI : public CirGate
{
public:
   PI(unsigned l, unsigned ID) : CirGate(l,ID) {}
   ~PI() {}
   GateType getType() const { return PI_GATE; }
   string getTypeStr() const { return "PI"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv); 
   void reportFanout(int level, bool inv);
   void setSymbol(string s) { _symbol = s; }
   void setFanin(size_t fi) { assert(0); }
   void setFanout(size_t fo);
   void sortFanout();
   bool floatingFanin() const { return false; }
   bool noFanout() const { return _fanoutList.size()==0;}
   void clearFanInOut() {}
   void deleteFanin(CirGate* fi) {}
   void deleteFanout(CirGate* fo);
   void renewFanin(CirGate*, size_t) {}
   const vector<size_t>& getFanout() const { return _fanoutList; }
   string getSymbol() const { return _symbol; }
   void setValue(PatternType p) { _value = p; }
   void setFecGroup(const IGateList& l, bool inv) {}
   void clearFecGroup() {}
   void setVar(const Var& v) { _var = v; }
   Var getVar() const { return _var; }
private:
   string                  _symbol;
   vector<size_t>          _fanoutList;
   Var                     _var;
};

class PO : public CirGate
{
public:
   PO(unsigned l, unsigned ID) : CirGate(l, ID), _fanin(ULONG_MAX){}
   ~PO() {}
   GateType getType() const { return PO_GATE; }
   string getTypeStr() const { return "PO"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setSymbol(string s) { _symbol = s; }
   void setFanin(size_t fi);
   void setFanout(size_t fo) {}
   void sortFanout() {}
   bool floatingFanin() const { return toPtr(_fanin)->getTypeStr() == "UNDEF"; }
   bool noFanout() const { return false; }
   size_t getFanin() const { return _fanin; }
   string getSymbol() const { return _symbol; }
   void clearFanInOut() {}
   void deleteFanin(CirGate* fi);
   void deleteFanout(CirGate* fo) {}
   void renewFanin(CirGate*, size_t);
   const vector<size_t>& getFanout() const {}
   void fetchValue();
   void setFecGroup(const IGateList& l, bool inv) {}
   void clearFecGroup() {}
   void setVar(const Var& v) { _var = v; }
   Var getVar() const { return _var; }
private:
   string                  _symbol;
   size_t                  _fanin;
   Var                     _var;
};

class AIG : public CirGate
{
public:
   AIG(unsigned l, unsigned ID) : CirGate(l, ID)
   { _fanin[0] = _fanin[1] = ULONG_MAX;}
   ~AIG() {}
   // Function for identifying type:
   GateType getType() const { return AIG_GATE; }
   string getTypeStr() const { return "AIG"; }
   // Functions for printing:
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   // Setting functions:
   void setFanin(size_t fi);
   void setFanout(size_t fo); 
   void sortFanout();
   bool floatingFanin() const { return (toPtr(_fanin[0])->getTypeStr() == "UNDEF" ||
                                        toPtr(_fanin[1])->getTypeStr() == "UNDEF"); }
   bool noFanout() const { return (_fanoutList.size()==0); }
   size_t getFanin(int i=0) const { return _fanin[i]; }
   void clearFanInOut();
   void deleteFanin(CirGate* fi);
   void deleteFanout(CirGate* fo);
   FaninType checkFaninType(size_t& fi);
   const vector<size_t>& getFanout() const { return _fanoutList; }
   void renewFanin(CirGate*, size_t);
   void swapFanin();
   void evaluateValue();
   void setFecGroup(const IGateList& l, bool inv);
   void clearFecGroup() { _FecGroup.clear(); }
   void setVar(const Var& v) { _var = v; }
   FecGroup& getFecGroup() { return _FecGroup; }
   Var getVar() const { return _var; }
   void deleteFecGroup();
private:
   size_t                  _fanin[2];
   vector<size_t>          _fanoutList;
   FecGroup                _FecGroup;
   Var                     _var;
};

class Const : public CirGate
{
public:
   Const() : CirGate(0,0) {}
   ~Const() {}
   GateType getType() const { return CONST_GATE; }
   string getTypeStr() const { return "CONST"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setFanin(size_t fi) { assert(0); }
   void setFanout(size_t fo); 
   void sortFanout();
   bool floatingFanin() const { return false; }
   bool noFanout() const { return _fanoutList.size()==0; }
   void clearFanInOut() {}
   void deleteFanin(CirGate* fi) {}
   void deleteFanout(CirGate* fo);
   const vector<size_t>& getFanout() const { return _fanoutList; }
   void renewFanin(CirGate*, size_t) {}
   void setFecGroup(const IGateList& l, bool inv);
   void clearFecGroup() { _FecGroup.clear(); }
   void setVar(const Var& v) {} 
   FecGroup& getFecGroup() { return _FecGroup; }
   Var getVar() const { return _var; } 
private:
   vector<size_t>          _fanoutList;
   FecGroup                _FecGroup;
   Var                     _var;
};

class Undef : public CirGate
{
public:
   Undef(unsigned ID) : CirGate(0,ID) {}
   ~Undef() {}
   GateType getType() const { return UNDEF_GATE; }
   string getTypeStr() const { return "UNDEF"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setFanin(size_t fi) { assert(0); }
   void setFanout(size_t fo);
   void sortFanout();
   bool floatingFanin() const { return false; }
   bool noFanout() const { return _fanoutList.size()==0; }
   void clearFanInOut();
   void deleteFanin(CirGate* fi) {}
   void deleteFanout(CirGate* fo);
   const vector<size_t>& getFanout() const { return _fanoutList; }
   void renewFanin(CirGate*, size_t) {}
   void setFecGroup(const IGateList& l, bool inv) {}
   void clearFecGroup() {}
   void setVar(const Var& v) {}
   Var getVar() const {}
private:
   vector<size_t>          _fanoutList;
};

#endif // CIR_GATE_H
