/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr();
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const; 

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&);

   CirGate* setUndef(unsigned ID);

private:
   // Helper function:
   void pushAIG(CirGate*);
   bool readHeader(ifstream&);
   bool readPI(ifstream&);
   bool readPO(ifstream&);
   bool readAIG(ifstream&);
   bool setGate();
   bool readSymbol(ifstream&);

   // Member variables:
   GateList                _gateList;           // Sorted in order of lineNo.
   IdList                  _idList;             // Mapping ID to lineNo.
   unsigned                _maxV;
   unsigned                _PINum;
   unsigned                _PONum;
   unsigned                _AIGNum;
   vector<AIG*>            _AIGList;
};

struct CirSort
{
   bool operator() (CirGate* a, CirGate* b)
   { return a->getID() < b->getID() ; }
   bool operator() (size_t a, size_t b)
   { return (toPtr(a))->getID() < (toPtr(b))->getID(); }
};

#endif // CIR_MGR_H
