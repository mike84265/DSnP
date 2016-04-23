/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

static size_t find_extra_space(string str)
{
   for (size_t i=0; i<str.size(); i++){
      if (str[i] == ' ')
         if (i==str.size()-1 || str[i+1] == ' ')
            return i;
   }
   return string::npos;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::CirMgr()
{
   CirGate* g = new Const();
   _gateList.push_back(g);
   _idList.push_back(0);
}

bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream inf(fileName);
   if (!inf){
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }
   if (! ( readHeader(inf) &&
           readPI(inf)     &&
           readPO(inf)     &&
           readAIG(inf)    &&
           setGate()       &&
           readSymbol(inf)
      )) return false;
   inf.close();
   return true;
}

CirGate* CirMgr::getGate(unsigned ID) const
{
   if (ID >= _idList.size()) return 0;
   if (ID!=0 && _idList[ID] == 0) return 0;
   size_t ret = (size_t)_gateList[ _idList[ID] ];
   #define INV size_t(0x1)
   return (CirGate*)(ret & ~INV);
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << setw(12) << right << _PINum << endl;
   cout << "  PO" << setw(12) << right << _PONum << endl;
   cout << "  AIG" << setw(11) << right << _AIGNum << endl;
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << right << _PINum+_PONum+_AIGNum << endl;
}

void
CirMgr::printNetlist() const
{
   CirGate::resetPrintOrder();
   CirGate::setGlobalref();
   cout << endl;
   for (size_t i=0; i<_PONum; i++){
      (getGate(_maxV+i+1))->printGate();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t i=0; i<_PINum; ++i)
      cout << " " << _gateList[i+1]->getID();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (size_t i=0; i<_PONum; ++i)
      cout << " " << _gateList[_PINum+i+1]->getID();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   GateList noFanin;
   GateList noFanout;
   for (size_t i=1; i<_gateList.size(); i++){
      if (_gateList[i] == 0)
         break;
      if (_gateList[i]->noFanin())
         noFanin.push_back(_gateList[i]);
      if (_gateList[i]->noFanout())
         noFanout.push_back(_gateList[i]);
   }
   //Sort
   if (noFanin.size()){
      std::sort(noFanin.begin(), noFanin.end(),CirSort());
      cout << "Gates with floating fanin(s):";
      for (size_t i=0; i<noFanin.size(); ++i)
         cout << " " << noFanin[i]->getID();
      cout << endl;
   }
   if (noFanout.size()){
      std::sort(noFanout.begin(), noFanout.end(), CirSort());
      cout << "Gates defined but not used  :";
      for (size_t i=0; i<noFanout.size(); ++i)
         cout << " " << noFanout[i]->getID();
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile)
{
   if (_AIGList.size() == 0){
      CirGate::setGlobalref();
      for (int i=0; i<_PONum; i++)
         pushAIG(_gateList[ _idList[_maxV+i+1] ]);
   }
   char str[128];
   sprintf(str,"aag %d %d 0 %d %d\n",_maxV,_PINum,
           _PONum,(int)_AIGList.size());
   outfile << str;
   for (size_t i=1;i<=_PINum;++i){
      outfile << _gateList[i]->getID()*2 << endl;
      assert(_gateList[i]->getTypeStr() == "PI");
   }// Printing PIs
   for (size_t i=_maxV+1;i<=_maxV+_PONum;++i){
      assert(_gateList[ _idList[i] ]->getTypeStr() == "PO");
      PO* g = dynamic_cast<PO*>(_gateList[ _idList[i] ]);
      unsigned ID = (toPtr(g->getFanin()))->getID() * 2;
      if (g->getFanin() & INV)
         ++ID;
      outfile << ID << endl;
   }// Printing POs
   for (size_t i=0;i<_AIGList.size();++i){
      outfile << _AIGList[i]->getID()*2;
      for (size_t j=0;j<2;j++){
         unsigned ID = (toPtr(_AIGList[i]->getFanin(j)))->getID() * 2;
         if (_AIGList[i]->getFanin(j) & INV)
            ++ID;
         outfile << " " << ID;
      }
      outfile << endl;
   }// Printing AIGs
   for(size_t i=1;i<=_PINum;++i){
      PI* g = dynamic_cast<PI*>(_gateList[i]);
      string s = g->getSymbol();
      if (s.size() != 0){
         outfile << "i" << i-1 << " " << s << endl;
      }
   }// Printing symbols of PIs
   for (size_t i=_maxV+1;i<=_maxV+_PONum;++i){
      PO* g = dynamic_cast<PO*>(_gateList[ _idList[i] ]);
      string s = g->getSymbol();
      if (s.size() != 0){
         outfile << "o" << i-_maxV-1 << " " << s << endl;
      }
   }
   outfile << "c" << endl;
   outfile << "AAG output by CYT." << endl;
}

CirGate* 
CirMgr::setUndef(unsigned ID)
{
   CirGate* ret = new Undef(ID);
   _gateList.push_back(ret);
   _idList[ID] = _gateList.size()-1;
   return ret;
}

void
CirMgr::pushAIG(CirGate* g)
{
   if (g->refMatch()) return;
   if (g->getTypeStr() == "AIG"){
      AIG* t = dynamic_cast<AIG*>(g);
      for (int i=0;i<2;i++){
         if ( (toPtr(t->getFanin(i)))->getTypeStr() == "AIG")
            pushAIG(toPtr(t->getFanin(i)));
      }
      _AIGList.push_back(t);
      t->setRef();
   } else if (g->getTypeStr() == "PO"){
      PO* t = dynamic_cast<PO*>(g);
      if ((toPtr(t->getFanin()))->getTypeStr() == "AIG")
         pushAIG(toPtr(t->getFanin()));
   } else assert(0);
}

bool 
CirMgr::readHeader(ifstream& inf)
{
   string str,tok; 
   int num;
   getline(inf,str);
   lineNo = colNo = 0;
   size_t exSpacePos = find_extra_space(str);
   if (exSpacePos != string::npos){
      colNo = exSpacePos;
      return parseError(EXTRA_SPACE);
   }
   colNo = myStrGetTok(str,tok,0);
   if (tok != "aag"){
      errMsg = tok;
      return parseError(ILLEGAL_IDENTIFIER);
   }

   colNo = myStrGetTok(str,tok,colNo);
   if (!myStr2Int(tok,num))
      return parseError(ILLEGAL_NUM);
   _maxV = num;                //Max. var. index

   colNo = myStrGetTok(str,tok,colNo);
   if (!myStr2Int(tok,num))
      return parseError(ILLEGAL_NUM);
   _PINum = num;              //Num of PIs

   colNo = myStrGetTok(str,tok,colNo);
   if (!myStr2Int(tok,num))
      return parseError(ILLEGAL_NUM);
                              //Latches

   colNo = myStrGetTok(str,tok,colNo);
   if (!myStr2Int(tok,num))
      return parseError(ILLEGAL_NUM);
   _PONum = num;              //Num of POs
   _gateList.resize(_maxV + _PONum + 1);
   _idList.resize(_maxV + _PONum + 1);
   //ID of PO = [_maxV+1 , _maxV+_PONum]

   colNo = myStrGetTok(str,tok,colNo);
   if (!myStr2Int(tok,num))
      return parseError(ILLEGAL_NUM);
   _AIGNum = num;             //Num of AIGs

   ++lineNo;
   return true;
}

bool 
CirMgr::readPI(ifstream& inf) 
{
  string str;
  size_t exSpacePos;
  int num;
// Read PIs:
// 1. Assign lineNo and ID to the new PI gates.
// 2. Update _idList accordingly.
   for (size_t i=0;i<_PINum;++i, ++lineNo, colNo=0){
      getline(inf, str);
      exSpacePos = find_extra_space(str);
      if (exSpacePos != string::npos){
         colNo = exSpacePos;
         return parseError(EXTRA_SPACE);
      }
      if (!myStr2Int(str,num))
         return parseError(ILLEGAL_NUM);
      assert(num%2 == 0);
      _gateList[lineNo] = new PI(lineNo, num/2);  
      _idList[num/2] = lineNo;
   }//end for
   return true;
}

bool 
CirMgr::readPO(ifstream& inf)
{
// Read POs:
// 1. Assigned lineNo and ID to the new PO gates.
//    Note that ID here is obtained from the order of PI plus max. var. index
// 2. Update _idList accordingly.
   string str;
   size_t exSpacePos;
   int num;
   for (size_t i=0;i<_PONum;++i, ++lineNo, colNo=0){
      getline(inf, str);
      exSpacePos = find_extra_space(str);
      if (exSpacePos != string::npos){
         colNo = exSpacePos;
         return parseError(EXTRA_SPACE);
      }
      if (!myStr2Int(str,num))
         return parseError(ILLEGAL_NUM);
      _gateList[lineNo] = new PO(lineNo, _maxV+i+1);
      _idList[_maxV+i+1] = lineNo;
      _gateList[lineNo]->setFanin((size_t)num);
   }//end for
   return true;
}

bool 
CirMgr::readAIG(ifstream& inf)
{
// Read AIGs:
// 1. Assign lineNo and ID to the new AIG gates.
// 2. Update _idList accordingly.
// 3. Push the _faninList and update the fanout of the refered _fanin.
   string str, tok;
   int num;
   size_t exSpacePos;
   for (size_t i=0;i<_AIGNum; ++i, ++lineNo, colNo=0){
      getline(inf, str);
      exSpacePos = find_extra_space(str);
      if (exSpacePos != string::npos){
         colNo = exSpacePos;
         return parseError(EXTRA_SPACE);
      }
      colNo = myStrGetTok(str,tok,0);
      if (!myStr2Int(tok,num))
         return parseError(ILLEGAL_NUM);
      assert(num%2 == 0);
      unsigned ID = num/2;
      _gateList[lineNo] = new AIG(lineNo, ID);
      _idList[ID] = lineNo;

      for(size_t j=0;j<2;++j){
         colNo = myStrGetTok(str,tok,colNo);
         if (!myStr2Int(tok,num))
            return parseError(ILLEGAL_NUM);
         _gateList[lineNo]->setFanin((size_t)num);
      }
   }//end for
   return true;
}

bool 
CirMgr::setGate()
{
   //Set up pointers in _gateList
   for (size_t i=_PINum+1;i<=_maxV+_PONum; ++i){
      if (_gateList[i] == 0) break;
      _gateList[i]->setFanin();
   }
   return true;
}

bool 
CirMgr::readSymbol(ifstream& inf)
{
   string str, tok;
   int num;
   while(!inf.eof()) {
      getline(inf, str);
      if (str == "c" || str.size()==0) break;
      assert(str[0]=='i' || str[0]=='o');
      colNo = myStrGetTok(str,tok,1);
      if (!myStr2Int(tok,num))
         return parseError(ILLEGAL_NUM);
      colNo = myStrGetTok(str,tok,colNo);
      if (str[0] == 'i')
         dynamic_cast<PI*>(_gateList[num+1])->setSymbol(tok);
      else if (str[0] == 'o')
         dynamic_cast<PO*>(_gateList[_PINum+num+1])->setSymbol(tok);
   }
   return true;
}
