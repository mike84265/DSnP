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

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"
#include "cirFec.h"
#include "sat.h"

extern CirMgr *cirMgr;

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

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs();
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   CirGate* setUndef(unsigned);

private:
   // Functions called by readCircuit:
   bool readHeader(ifstream&);
   bool readPI(ifstream&);
   bool readPO(ifstream&);
   bool readAIG(ifstream&);
   bool setGate();
   bool readSymbol(ifstream&);

   // Functions for building DFS list:
   void pushAIG(CirGate*);
   void buildDfsList();
   void pushFanin(CirGate*);

   // Functions for cirOpt:
   void merge_strash(AIG*, AIG*);

   // Functions for cirSim:
   bool readPattern(ifstream&, vector<PatternType>&,
                    unsigned, unsigned&);
   void printPattern() const;
   void evaluateValue();

   // Functions for cirFraig:
   void merge_fraig(size_t, size_t);

   // Member variables:
   GateList                _gateList;           // Sorted in order of lineNo.
   IdList                  _idList;             // Mapping ID to lineNo.
   unsigned                _maxV;
   unsigned                _AIG_begin;
   unsigned                _PO_begin;
   unsigned                _PINum;
   unsigned                _PONum;
   unsigned                _AIGNum;
   GateList                _dfsList;
   FecGroupList            _FecList;
   ofstream*               _simLog;
};

class CirSort
{
public:
   bool operator() (CirGate* a, CirGate* b)
   { return a->getID() < b->getID() ; }
   bool operator() (size_t a, size_t b)
   { return (toPtr(a))->getID() < (toPtr(b))->getID(); }
};
#endif // CIR_MGR_H
