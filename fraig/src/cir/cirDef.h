/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;

typedef vector<CirGate*>         GateList;
typedef vector<unsigned>         IdList;
typedef vector<size_t>           IGateList;
typedef unsigned                 PatternType;
typedef vector<size_t>           FecGroup;

enum GateType {
   PI_GATE,
   PO_GATE,
   AIG_GATE,
   CONST_GATE,
   UNDEF_GATE
};

enum FaninType{
   CONST0,
   CONST1,
   SAME,
   INVERT,
   DIFFERENT
};

#endif // CIR_DEF_H
