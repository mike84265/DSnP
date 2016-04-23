/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <queue>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashMap.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   _FecList.initialize(_dfsList,_gateList[0]);
   size_t numSim ;
   if (_PINum < 10) numSim = _PINum;
   else numSim = (size_t)(3.5*sqrt(_PINum));
   cout << "MAX_FAILS = " << numSim << endl;
   for (size_t r=0;r<numSim;++r){
      for (size_t i=0; i<_PINum;++i){
         PatternType p = 0;
         for (size_t j=0;j<sizeof(PatternType);++j){
            p |= (rnGen(256) << (8*j));
            #ifdef DEBUG
            cout << "p = " << p << endl;
            #endif
         }
         dynamic_cast<PI*>(_gateList[i+1])->setValue(p);
      }
      evaluateValue();
      if (_simLog != 0)
         printPattern();
      _FecList.update();
      cout << "\rTotal #FEC Group = " << _FecList.numGroups();
   }
   for (size_t i=0;i<_dfsList.size();++i)
      _dfsList[i]->clearFecGroup();
   _FecList.updateFecGroupInGates();
   int numPattern = _PINum * 8 * sizeof(PatternType);
   cout << "\r" << numPattern << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   // 1. Read in patterns and compact it into 32-bit pattern for each PI
   // 2. Implement the gate-wise simulation function
   // 3. Recognize FEC groups and store it.
   // How to maintain FEC groups?
   vector<PatternType> pattern;
   unsigned numPattern = 0;
   _FecList.initialize(_dfsList,_gateList[0]);
   while (1) {
      if (!readPattern(patternFile,pattern,_PINum,numPattern))
         break;
      for (size_t i=0;i<_PINum;++i){
         PI* g = dynamic_cast<PI*>(_gateList[i+1]);
         assert(g!=0);
         g->setValue(pattern[i]);
      }
      evaluateValue();
      if (_simLog != 0)
         printPattern();
      _FecList.update();
      cout << "\rTotal #FEC Group = " << _FecList.numGroups();
      if (patternFile.eof()) break;
   }//end while
   if (patternFile.eof()){
      for (size_t i=0;i<_dfsList.size();++i)
         _dfsList[i]->clearFecGroup();
      _FecList.updateFecGroupInGates();
   }
   cout << "\r" << numPattern << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
bool 
CirMgr::readPattern(ifstream& ifs, 
      vector<PatternType>& pattern, unsigned length, unsigned& numPattern)
{
   vector<string> vs;
   string s;
   pattern.clear();
   for (int i=0;i<8*sizeof(PatternType);++i){
      ifs >> s;
      if (ifs.eof()) {
         if (i==0) return false;
         else break;
      }
      if (s.length() != length){
         cout << "\nError: Pattern(" << s << ") length(" << s.size()
              << ") does not match the number of inputs(" << length 
              << ") in a circuit!!" << endl;
         return false;
      }
      vs.push_back(s);
   }
   pattern.resize(length);
   for (int i=0;i<vs.size();++i)
      for (int j=0;j<length;++j){
         if (!(vs[i][j] == '0' || vs[i][j] == '1')){
            cout << "Error: Pattern(" << vs[i] << ") contains a non-0/1 character(\'"
                 << vs[i][j] << "\')" << endl;
            return false;
         }
         pattern[j] |= ((PatternType)(vs[i][j]-'0') << i);
      }
   numPattern += vs.size();
   return true;
}

void
CirMgr::printPattern() const
                    
{
   vector<PatternType> PIVal;
   vector<PatternType> POVal;
   for (size_t i=0;i<_PINum;++i){
      CirGate* g = _gateList[i+1];
      assert(g->getTypeStr() == "PI");
      PIVal.push_back(g->getValue());
   }
   for (size_t i=0;i<_PONum;++i){
      CirGate* g = _gateList[_PINum+i+1];
      assert(g->getTypeStr() == "PO");
      POVal.push_back(g->getValue());
   }
   for (size_t i=0;i<8*sizeof(PatternType);++i){
      for (size_t j=0;j<PIVal.size();++j){
         (*_simLog) << (( PIVal[j] & (1<<i) ) >> i);
      }
      (*_simLog) << " ";
      for (size_t j=0;j<POVal.size();++j){
         (*_simLog) << (( POVal[j] & (1<<i) ) >> i);
      }
      (*_simLog) << endl;
   }
}

void
CirMgr::evaluateValue()
{
   for (size_t i=0;i<_dfsList.size();++i){
      switch(_dfsList[i]->getType()){
       case PI_GATE: case CONST_GATE:
         continue;
       case AIG_GATE: {
         AIG* g = dynamic_cast<AIG*>(_dfsList[i]);
         g->evaluateValue();
         break;
                      }
       case PO_GATE: {
         PO* g = dynamic_cast<PO*>(_dfsList[i]);
         g->fetchValue();
         break;
                     }
       default:
         break;
      }//end switch
   }//end for
}
/**********************************************/
/*            Functions for FecKey            */ 
/**********************************************/

FecKey::FecKey(size_t g)
{
   if (g & INV) _value = ~(toPtr(g)->getValue());
   else _value = toPtr(g)->getValue();
}

FecKey
FecKey::operator~()
{
   _value = ~_value;
   FecKey ret(*this);
   _value = ~_value;
   return ret;
}

/**************************************************/
/*        Member Functions of FecGroupList        */ 
/**************************************************/
void
FecGroupList::update()
{
   size_t n = _FecGroupList.size();
   for (size_t i=0;i<n;++i){
      #ifdef QUEUE
      FecGroup& f = _FecGroupList.front();
      #else
      FecGroup& f = _FecGroupList[0];
      #endif
      HashMap<FecKey,IGateList*> hashMap(f.size());
      for (size_t i=0;i<f.size();++i){
         FecKey key(f[i]);
         IGateList* list;
         if (hashMap.check(key,list)){
            list->push_back(f[i]);
         } else if (hashMap.check(~key,list)) {
            list->push_back(f[i] ^ INV);
         } else {
            IGateList* igl = new IGateList;
            igl->push_back(f[i] & ~INV);
            if (f[i] & INV)
               hashMap.forceInsert(~key,igl);
            else
               hashMap.forceInsert(key,igl);
         }
      }
      #ifdef QUEUE
      _FecGroupList.pop(); // pop after the reference has been used out
      #else
      _FecGroupList.erase(_FecGroupList.begin());
      #endif
      HashMap<FecKey,IGateList*>::iterator it;
      for (it=hashMap.begin();it!=hashMap.end();++it){
         if (((*it).second)->size() > 1) {
            #ifdef QUEUE
            _FecGroupList.push(*((*it).second));
            #else
            _FecGroupList.push_back(*((*it).second));
            #endif
         }
      }
      //cout << "\rTotal #FEC Group = " << _FecGroupList.size();
      hashMap.reset();
   }
}

void
FecGroupList::updateFecGroupInGates() 
{
   size_t n = _FecGroupList.size();
   #ifdef DEBUG
   cout << "size = " << n << endl;
   #endif
   #ifdef QUEUE
   for (size_t i=0;i<n;++i){
      FecGroup f = _FecGroupList.front();    
      // Copy the element since the element will be destructed by pop()
      std::sort(f.begin(),f.end(),CirSort());
      _FecGroupList.pop();
      _FecGroupList.push(f);
      for (size_t j=0;j<f.size();++j)
         toPtr(f[j])->setFecGroup(f, (bool)(f[j]&INV));
   }
   assert(_FecGroupList.front().size() != 0);
   #else
   for (size_t i=0;i<n;++i){
      FecGroup& f = _FecGroupList[i];
      std::sort(f.begin(),f.end(),CirSort());
      for (size_t j=0;j<f.size();++j)
         toPtr(f[j])->setFecGroup(f,(bool)(f[j]&INV));
   }
   #endif
}

void
FecGroupList::reset()
{
   #ifdef QUEUE
   while (!_FecGroupList.empty())
      _FecGroupList.pop();
   #else
   for (size_t i=0;i<_FecGroupList.size();++i)
      _FecGroupList[i].clear();
   #endif
   // pop() will call the destructor of the element.
}

void
FecGroupList::initialize(const GateList& gList, const CirGate* const_gate)
{
   FecGroup f;
   if (_FecGroupList.size() != 0) return;
   for (size_t i=0;i<gList.size();++i){
      if (gList[i]->getTypeStr() == "AIG")
         f.push_back((size_t)(gList[i]));
   }
   f.push_back((size_t)const_gate);
   #ifdef QUEUE
   _FecGroupList.push(f);
   #else
   _FecGroupList.push_back(f);
   #endif
}

void
FecGroupList::print()
{
   int n = _FecGroupList.size();
   for (int i=0;i<n;++i){
      #ifdef QUEUE
      FecGroup f = _FecGroupList.front();
      _FecGroupList.pop();
      _FecGroupList.push(f);
      #else
      FecGroup& f = _FecGroupList[i];
      #endif
      cout << "[" << i << "]";
      for (size_t j=0;j<f.size();++j){
         cout << " ";
         if (f[j]&INV) cout << "!";
         cout << toPtr(f[j])->getID();
      }
      cout << endl;
   }
}

/********************************************/
/*       Member Functions of CirGate        */
/********************************************/
void
PO::fetchValue()
{
   PatternType p = toPtr(_fanin)->getValue();
   _value = (_fanin&INV)? ~p : p;
}

void
AIG::evaluateValue()
{
   PatternType f1 = toPtr(_fanin[0])->getValue();
   PatternType f2 = toPtr(_fanin[1])->getValue();
   f1 = (_fanin[0]&INV)? ~f1 : f1;
   f2 = (_fanin[1]&INV)? ~f2 : f2;
   _value = (f1 & f2);
}

void
AIG::setFecGroup(const IGateList& l, bool inv)
{
   FecGroup dummy;
   _FecGroup.swap(dummy);
   for (size_t i=0;i<l.size();++i){
      if (toPtr(l[i]) == this) continue;
      if (inv)
         _FecGroup.push_back(l[i] ^ INV);
      else
         _FecGroup.push_back(l[i]);
   }
}

void
Const::setFecGroup(const IGateList& l, bool inv)
{
   FecGroup dummy;
   _FecGroup.swap(dummy);
   for (size_t i=0;i<l.size();++i){
      if (toPtr(l[i]) == this) continue;
      if (inv)
         _FecGroup.push_back(l[i] ^ INV);
      else
         _FecGroup.push_back(l[i]);
   }
}
