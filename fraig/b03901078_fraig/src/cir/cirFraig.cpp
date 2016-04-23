/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "cirFraig.h"
#include "cirStrash.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   HashMap<AIGKey,AIG*> hashMap(_dfsList.size());
   for (size_t i=0;i<_dfsList.size();++i){
      if (_dfsList[i]->getTypeStr() != "AIG") continue;
      AIG* g = dynamic_cast<AIG*>(_dfsList[i]);
      AIGKey key(g);
      AIG* mergeGate;
      if (hashMap.check(key,mergeGate)){
         cout << "Strashing: " << mergeGate->getID() << " merging "
              << _dfsList[i]->getID() << "..." << endl;
         merge_strash(mergeGate,g);
      }
      else
         hashMap.forceInsert(key,g);
   }
   hashMap.reset();
   _dfsList.clear();
   buildDfsList();
   _AIGNum = 0;
   for (size_t i=0;i<_dfsList.size();++i){
      if (_dfsList[i]->getTypeStr() == "AIG")
         ++_AIGNum;
   }
}

void
CirMgr::fraig()
{
   // 1. Initialize the solver.
   // 2. Generate the vars.
   // 3. Adding AigCNF by dfsList.
   // 4. Adding XorCNF and assumption.
   // 5. Solve it.
   SatSolver s;
   s.initialize();
   _gateList[0]->setVar(s.newVar());   //CONST_GATE
   for (size_t i=0;i<_dfsList.size();++i){
      _dfsList[i]->setVar(s.newVar());
      if (_dfsList[i]->getTypeStr() == "AIG"){
         const AIG* g = dynamic_cast<AIG*>(_dfsList[i]);
         s.addAigCNF(g->getVar(),
                     toPtr(g->getFanin(0))->getVar(), (g->getFanin(0) & INV),
                     toPtr(g->getFanin(1))->getVar(), (g->getFanin(1) & INV));
      }
   }
   size_t n = _FecList.numGroups();
   for (size_t i=0;i<_dfsList.size();++i){
      if (_dfsList[i]->getUsed() == false){
         delete _dfsList[i];
         continue;
      }
      FecGroup* f;
      switch(_dfsList[i]->getType()){
       case CONST_GATE:
         f = &(dynamic_cast<Const*>(_dfsList[i])->getFecGroup());
         break;
       case AIG_GATE:
         f = &(dynamic_cast<AIG*>(_dfsList[i])->getFecGroup());
         break;
       default:
         continue;
      }
      if (f->empty()) continue;
      for (size_t j=0;j<f->size();++j){
         if (toPtr((*f)[j])->getTypeStr() == "CONST") continue;
         Var newV = s.newVar();
         cout << "\rProving (" << _dfsList[i]->getID() << ", "
              << (((*f)[j] & INV)? "!" : "")
              << toPtr((*f)[j])->getID() << ")...";
         s.addXorCNF(newV,_dfsList[i]->getVar(),false
                         ,toPtr((*f)[j])->getVar(),((*f)[j]&INV));
         s.assumeRelease();
         s.assumeProperty(_gateList[0]->getVar(),false);
         s.assumeProperty(newV,true);
         bool result = s.assumpSolve();
         if (!result){
            cout << "Fraig: " << _dfsList[i]->getID() << " merging " 
                 << (((*f)[j]&INV)? "!" : "") << toPtr((*f)[j])->getID() << endl;
            merge_fraig((size_t)_dfsList[i],(*f)[j]); 
         } else cout << "UNSAT!!";
         FecGroup& df = toAig((*f)[j])->getFecGroup();
         for (size_t k=0;k<df.size();++k){
            if (toPtr(df[k]) == toPtr((*f)[j]))
               df.erase(df.begin()+k);
         }
      }
      //FecGroup dummy;
      //f->swap(dummy);
      cout << "Updating by SAT... Total #FEC Group = " << --n  << endl;
      if (n==0) break;
   }
   _FecList.reset();
   _dfsList.clear();
   #ifdef DEBUG
   cout << "Building dfs list......" << endl;
   #endif
   buildDfsList();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::merge_strash(AIG* sGate, AIG* dGate)
{
   // 1. Sort the fanins to corresponding order
   // 2. Check INV
   // 3. Renew fanin
   // 4. Let sGate inherit the fanouts
   // 5. Update _idList, delete dGate.
   if (dGate->getFanin(0) != sGate->getFanin(0))
      dGate->swapFanin();
   #ifdef DEBUG
   assert(sGate->getFanin(0) == dGate->getFanin(0));
   assert(sGate->getFanin(1) == dGate->getFanin(1));
   #endif
   const vector<size_t>& fo = dGate->getFanout();
   for (size_t i=0;i<2;++i){
      size_t fi = dGate->getFanin(i);
      toPtr(fi)->deleteFanout((CirGate*)dGate);
   }
   for (size_t i=0;i<fo.size();++i){
      sGate->setFanout(fo[i]);
      toPtr(fo[i])->renewFanin((CirGate*)dGate,(size_t)sGate);
   }
   _idList[dGate->getID()] = 0;
   delete dGate;
}

void
AIG::swapFanin()
{
   size_t temp = _fanin[0];
   _fanin[0] = _fanin[1];
   _fanin[1] = temp;
}


/*****************************************/
/*     Member functions about fraig      */
/*****************************************/
void
CirMgr::merge_fraig(size_t sGate, size_t dGate)
{
   const vector<size_t>& fo = toPtr(dGate)->getFanout();
   size_t mask = (dGate & INV) ^ (sGate & INV);
   for (size_t i=0;i<fo.size();++i){
      #ifdef DEBUG
      toPtr(fo[i])->reportFanin(2);
      cout << "dGate ID = " << toPtr(dGate)->getID() << endl;
      cout << "sGate ID = " << toPtr(sGate)->getID() << endl;
      cout << "size of FecGroup = " << toAig(dGate)->getFecGroup().size() << endl;
      #endif
      toPtr(sGate)->setFanout(fo[i] ^ mask);
      toPtr(fo[i])->renewFanin(toPtr(dGate), (sGate ^ mask));
   }
   for (size_t i=0;i<2;++i)
      toPtr(toAig(dGate)->getFanin(i))->deleteFanout(toPtr(dGate)); 
   _idList[toPtr(dGate)->getID()] = 0;
   toPtr(dGate)->setUnused();
   toAig(dGate)->deleteFecGroup();
}

void
AIG::deleteFecGroup()
{
   for (size_t i=0;i<_FecGroup.size();++i){
      FecGroup& f = toAig(_FecGroup[i])->getFecGroup();
      for (size_t j=0;j<f.size();++j){
         if (toAig(f[j]) == this){
            f.erase(f.begin()+j); 
            #ifdef DEBUG
            cout << getID() << " deleted from " << toAig(_FecGroup[i])->getID() << "...\n";
            #endif
            break;
         }
      }
   }
}
