/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "cirFraig.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
// 1. Sweep the unused gates by their ID, and print out when a unused gate is deleted.
// 2. Update _gateList and _idList.
void
CirMgr::sweep()
{
   for (size_t i=1;i<_idList.size();++i){
      CirGate* g = getGate(i);
      if (!g) continue;
      if (g->getTypeStr() != "AIG" && g->getTypeStr() != "UNDEF")
         continue;
      if (!g->getUsed()){
         cout << "Sweeping: " << g->getTypeStr() 
              << "(" << g->getID() << ") removed..." << endl;
         _idList[i] = 0;
         g->clearFanInOut();
         if (g->getTypeStr() == "AIG")
            --_AIGNum;
         delete g;
      }
   }
   _AIGNum = 0;
   for (size_t i=0;i<_dfsList.size();++i)
      if (_dfsList[i]->getTypeStr() == "AIG")
         ++_AIGNum;
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
// 4 cases that needs to be considered:
// (1) And with a const 0.
// (2) And with a const 1.
// (3) AIG with same fanin.
// (4) AIG with same fanin but inverted.
// How to replace a gate?
// 1. Decide what to replace.
// 2. Renew the fanin of its fanoutlist.
// 3. Inherit its fanout.
// 4. Delete the gate
// And after all, renew the _dfsList and _AIGList.
// Carefully handle the inverting problem.
void
CirMgr::optimize()
{
   bool reconstruct = false;
   for (size_t i=0;i<_dfsList.size();++i) {
      CirGate* g = _dfsList[i];
      if (g->getTypeStr() != "AIG") continue;
      size_t fi=0;
      FaninType t = dynamic_cast<AIG*>(g)->checkFaninType(fi);
      if (t == DIFFERENT) continue;
      else {
         const vector<size_t>& fanoutList = g->getFanout();
         size_t mask = (fi & INV);
         reconstruct = true;
         if (g->getTypeStr() == "AIG")
            --_AIGNum;
         switch(t){
          case CONST0:     
            _gateList[0]->deleteFanout(g);
          case INVERT:     
            toPtr(fi)->deleteFanout(g);
            cout << "Simplifying: " << "0 merging " << g->getID() << "..." << endl;
            for (size_t j=0;j<fanoutList.size();++j){
               _gateList[0]->setFanout(fanoutList[j] ^ mask);
               toPtr(fanoutList[j])->renewFanin(g, (size_t)_gateList[0]);
            }
            _idList[g->getID()] = 0;
            delete g;
            break;
          case CONST1:    
            _gateList[0]->deleteFanout(g);
          case SAME:     
            toPtr(fi)->deleteFanout(g);
            cout << "Simplifying: " << toPtr(fi)->getID() 
                 << " merging " << ((fi&INV)? "!" : "") << g->getID() << "..." << endl;
            for (size_t j=0;j<fanoutList.size();++j){
               toPtr(fi)->setFanout(fanoutList[j] ^ mask); 
               toPtr(fanoutList[j])->renewFanin(g, fi);
            }
            _idList[g->getID()] = 0;
            delete g;
            break;
          case DIFFERENT:
          default:
            break;
         }
         _dfsList.erase(_dfsList.begin()+i);
         --i;
      }//end else
   }//end for
   if (reconstruct){
      _dfsList.clear();
      buildDfsList();
      _AIGNum = 0;
      for (size_t i=0;i<_dfsList.size();++i)
         if (_dfsList[i]->getTypeStr() == "AIG")
            ++_AIGNum;
   }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
/***********************************************************/
/*   Member functions of CirGate to perform optimization   */ 
/***********************************************************/
void
PO::renewFanin(CirGate* oriGate, size_t newGate)
{
   assert(toPtr(_fanin) == oriGate);
   _fanin = ((size_t)_fanin & INV)? newGate^INV : newGate;
}

void
AIG::renewFanin(CirGate* oriGate, size_t newGate)
{
   for (size_t i=0;i<2;++i){
      if (toPtr(_fanin[i])!=oriGate) continue;
      _fanin[i] = ((size_t)_fanin[i] & INV)? newGate^INV : newGate;
      return;
   }
   assert(0);
}

void 
PI::deleteFanout(CirGate* fo)
{
   for (size_t i=0;i<_fanoutList.size();++i)
      if (toPtr(_fanoutList[i])==fo){
         _fanoutList.erase(_fanoutList.begin()+i);
         --i;
      }
}

void PO::deleteFanin(CirGate* fi)
{
   assert(fi == toPtr(_fanin));
   _fanin = 0;
}

FaninType AIG::checkFaninType(size_t& fi)
{
   if ( (toPtr(_fanin[0])) == (toPtr(_fanin[1])) ){
      fi = _fanin[0];
      if ( (_fanin[0] & INV) == (_fanin[1] & INV) )
         return SAME; 
      else
         return INVERT;
   }
   for (int i=0;i<2;++i){
      if ( (toPtr(_fanin[i]))->getTypeStr() == "CONST" ){
            fi = (i==0)? _fanin[1] : _fanin[0];
         if (_fanin[i] & INV)
            return CONST1;
         else
            return CONST0;

      }
   }
   return DIFFERENT;
}

void AIG::clearFanInOut() 
{
   for (int i=0;i<2;++i) 
      if (toPtr(_fanin[i])!=0)
         (toPtr(_fanin[i]))->deleteFanout(this);
   for (size_t i=0;i<_fanoutList.size();++i){
      (toPtr(_fanoutList[i]))->deleteFanin(this);
   }
}

void AIG::deleteFanin(CirGate* fi)
{
   for (size_t i=0;i<2;++i){
      if (toPtr(_fanin[i]) == fi){
         _fanin[i] = 0; return; }
   }
   assert(0);
}

void AIG::deleteFanout(CirGate* fo)
{
   for (size_t i=0;i<_fanoutList.size();++i)
      if (toPtr(_fanoutList[i])==fo){
         _fanoutList.erase(_fanoutList.begin()+i);
         --i;
      }
}

void Const::deleteFanout(CirGate* fo)
{
   for (size_t i=0;i<_fanoutList.size();++i)
      if (toPtr(_fanoutList[i])==fo){
         _fanoutList.erase(_fanoutList.begin()+i);
         --i;
      }
}

void Undef::clearFanInOut()
{
   for (size_t i=0;i<_fanoutList.size();++i){
      (toPtr(_fanoutList[i]))->deleteFanin(this);
   }
}

void Undef::deleteFanout(CirGate* fo)
{
   for (size_t i=0;i<_fanoutList.size();++i)
      if (toPtr(_fanoutList[i])==fo){
         _fanoutList.erase(_fanoutList.begin()+i);
         return; 
      }
   assert(0);
}
