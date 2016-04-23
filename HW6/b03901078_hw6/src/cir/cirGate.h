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
#include "cirDef.h"
#define toPtr(p)             \
(CirGate*)(p & ~size_t(0x1))

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
      _lineNo(l), _id(ID){}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return _lineNo; }
   unsigned getID() const { return _id; }
   virtual bool noFanin() const = 0;
   virtual bool noFanout() const = 0;

   // Printing functions
   virtual void printGate() = 0;
   virtual void reportGate() const = 0;
   virtual void reportFanin(int level, bool inv=false) = 0;
   virtual void reportFanout(int level, bool inv=false) = 0;
   virtual void setFanin(size_t fi=ULONG_MAX) = 0;
   virtual void setFanout(size_t fo) = 0;

   static void resetPrintOrder() { _printOrder = 0; }
   static void setGlobalref() { ++_globalref; }
   static void resetIndent() { _indent = ""; }
   void setRef() const { _ref = _globalref; }
   unsigned getRef() const { return _ref; }
   bool refMatch() const { return _ref == _globalref; }

private:

protected:  
   unsigned                _lineNo;
   unsigned                _id;
   static unsigned         _globalref;
   static unsigned         _printOrder;
   static string           _indent;
   mutable unsigned                _ref;
};

class PI : public CirGate
{
public:
   PI(unsigned l, unsigned ID) : CirGate(l,ID) {}
   //arg = { lineNo, ID, fanout }
   ~PI() {}
   string getTypeStr() const { return "PI"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv); 
   void reportFanout(int level, bool inv);
   void setSymbol(string s) { _symbol = s; }
   void setFanin(size_t fi) { assert(0); }
   void setFanout(size_t fo);
   bool noFanin() const { return false; }
   bool noFanout() const { return _fanoutList.size()==0;}
   string getSymbol() const { return _symbol; }
private:
   string                  _symbol;
   vector<size_t>          _fanoutList;
};

class PO : public CirGate
{
public:
   PO(unsigned l, unsigned ID) : CirGate(l, ID), _fanin(ULONG_MAX){}
   //arg = { lineNo, ID, fanin }
   ~PO() {}
   string getTypeStr() const { return "PO"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setSymbol(string s) { _symbol = s; }
   void setFanin(size_t fi);
   void setFanout(size_t fo) { assert(0); }
   bool noFanin() const; 
   bool noFanout() const { return false; }
   size_t getFanin() const { return _fanin; }
   string getSymbol() const { return _symbol; }
private:
   string                  _symbol;
   size_t                  _fanin;
};

class AIG : public CirGate
{
public:
   AIG(unsigned l, unsigned ID) : CirGate(l, ID)
   { _fanin[0] = _fanin[1] = ULONG_MAX;}
   ~AIG() {}
   string getTypeStr() const { return "AIG"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setFanin(size_t fi);
   void setFanout(size_t fo);
   bool noFanin() const ;
   bool noFanout() const { return (_fanoutList.size()==0); }
   size_t getFanin(int i=0) const { return _fanin[i]; }
private:
   size_t                  _fanin[2];
   vector<size_t>          _fanoutList;
};

class Const : public CirGate
{
public:
   Const() : CirGate(0,0){}
   string getTypeStr() const { return "CONST0"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setFanin(size_t fi) { assert(0); }
   void setFanout(size_t fo) { _fanoutList.push_back(fo); }
   bool noFanin() const { return false; }
   bool noFanout() const { return _fanoutList.size()==0; }
private:
   vector<size_t>          _fanoutList;
};

class Undef : public CirGate
{
public:
   Undef(unsigned ID) : CirGate(0,ID) {}
   string getTypeStr() const { return "UNDEF"; }
   void printGate();
   void reportGate() const;
   void reportFanin(int level, bool inv);
   void reportFanout(int level, bool inv);
   void setFanin(size_t fi) { assert(0); }
   void setFanout(size_t fo) { _fanoutList.push_back(fo); }
   bool noFanin() const { return false; }
   bool noFanout() const { return _fanoutList.size()==0; }
private:
   vector<size_t>          _fanoutList;
};

#endif // CIR_GATE_H
