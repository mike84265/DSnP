/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <algorithm>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

unsigned CirGate::_globalref = 0;
unsigned CirGate::_printOrder = 0;
string CirGate::_indent = "";
// TODO: Implement memeber functions for class(es) in cirGate.h


/**************************************/
/*        PI member functions         */
/**************************************/

// Format:
// [order] PI ID (_symbol)
// e.g.
// [0] PI 1 (2GAT)
// 1. Base case, just print out the current gate.
// 2. Get _printOrder to decide the number to print.
void PI::printGate() 
{
   //if (_ref == _globalref) return;
   cout << "[" << _printOrder++ << "] PI  " << getID();
   if (_symbol.size()) cout << " (" << _symbol << ")";
   cout << endl;
   //_ref = _globalref;
}

void PI::reportGate() const
{
   stringstream ss;
   ss << "PI(" << getID() << ")";
   if (_symbol.size())
      ss << "\"" << _symbol << "\"";
   ss << ", line " << _lineNo+1;
   cout << "==================================================" << endl;
   cout << "= "; 
   cout << setw(47) << left;
   cout << ss.str() << "=\n";
   cout << "= FECs:                                          =" << endl;
   cout << "= Value: ";
   for (int i=8*sizeof(PatternType)-1; i>=0; --i){
      cout << ((_value & (1<<i)) >> i);
      if (i%4==0 && i>0) cout << "_";
   }
   cout << " =" << endl;
   cout << "==================================================" << endl;
}

void PI::reportFanin(int level, bool inv)
{
   assert(level>=0);
   cout << _indent;
   if (inv) cout << "!";
   cout << "PI " << getID() << endl;
}

// 1. Print PI (ID)
// 2. Keep a static member "string _indent" and properly handle it
//    everytime calling and returning a function.
// 3. Set the _ref to _globalref so that next time it's call,
//    it will not print out its fanouts again. 
// 4. Check its _ref, if it's not printed yet, call the report function
//    of next level. Otherwise, print a " (*) ".
// 5. When leaving a function, decrement the "indent".
void PI::reportFanout(int level, bool inv)
{
   assert(level>=0);
   assert(_indent == "");
   setGlobalref();
   if (inv) cout << "!";
   cout << "PI " << getID();
   if (level == 0)
   { cout << endl; return; }
   cout << endl;
   _indent += "  ";
   for (size_t i=0; i<_fanoutList.size(); ++i){
      bool inv_next = (_fanoutList[i] & INV);
      ((CirGate*)(_fanoutList[i] & ~INV))->reportFanout(level-1, inv_next);
   }
   _indent.resize(_indent.size()-2);

}

void PI::setFanout(size_t fi)
{ _fanoutList.push_back(fi); }

void PI::sortFanout()
{ std::sort(_fanoutList.begin(), _fanoutList.end(), CirSort()); }


/**************************************/
/*        PO member functions         */
/**************************************/

// Format:
// [order] PO ID _faninID (_symbol)
// e.g.
// [3] PO 11 3 (GATE$3)
// 1. Call the printGate() of _fanin.
// 2. Get _globalref to determine the printing order.
// 3. Increment _global ref.
// 4. Detect it whether its fanin is floating, if so,
//    put a '*' sign before '!' if existing.
void PO::printGate()
{
   //if (_ref == _globalref) return;
   cout << "[" << _printOrder++ << "] PO  " << getID() << " ";
   if (toPtr(_fanin)->getTypeStr()=="UNDEF") cout << "*";
   if (_fanin & INV) cout << "!";
   cout << toPtr(_fanin)->getID();
   if (_symbol.size()) cout << " (" << _symbol << ")";
   cout << endl;
   //_ref = _globalref;
}

void PO::reportGate() const
{
   stringstream ss;
   ss << "PO(" << getID() << ")";
   if (_symbol.size())
      ss << "\"" << _symbol << "\"";
   ss << ", line " << _lineNo+1;
   cout << "==================================================" << endl;
   cout << "= "; 
   cout << setw(47) << left;
   cout << ss.str() << "=\n";
   cout << "= FECs:                                          =" << endl;
   cout << "= Value: ";
   for (int i=8*sizeof(PatternType)-1; i>=0; --i){
      cout << ((_value & (1<<i)) >> i);
      if (i%4==0 && i>0) cout << "_";
   }
   cout << " =" << endl;
   cout << "==================================================" << endl;
}

void PO::reportFanin(int level, bool inv)
{
   assert(level>=0);
   assert(_indent=="");
   setGlobalref();
   if (inv) cout << "!";
   cout << "PO " << getID();
   if (level == 0)
   { cout << endl; return; }
   _indent += "  ";
   cout << endl;
   bool inv_next = (_fanin & INV);
   ((CirGate*)(_fanin & ~INV))->reportFanin(level-1, inv_next);
   _indent.resize(_indent.size()-2);
}

void PO::reportFanout(int level, bool inv)
{
   assert(level>=0);
   cout << _indent;
   if (inv) cout << "!";
   cout << "PO " << getID() << endl;
}

// 1. If fi is assigned, store this value temperarily into _fanin.
// 2. If fi is not assigned (i.e. as its default value=ULONG_MAX), 
//    convert the _fanin through cirMgr into cirGate*, 
//    and call _fanin->setFanout recursively.
// 3. If fi==ULONG_MAX while _fanin is still not assigned, do nothing.
void PO::setFanin(size_t fi)
{
   if (fi != ULONG_MAX){
      assert(_fanin==ULONG_MAX);
      _fanin = (size_t)fi; 
   } else {
      assert(_fanin != ULONG_MAX);
      bool inv = (_fanin%2 != 0);
      CirGate* t = cirMgr->getGate(_fanin/2);
      if (t == 0)
         _fanin = (size_t)cirMgr->setUndef(_fanin/2);
      else
         _fanin = (size_t)t;
      size_t p = inv? (size_t)this|INV : (size_t)this;
      ((CirGate*)_fanin)->setFanout(p);
      if (inv) _fanin |= INV;
   }
}

/**************************************/
/*        AIG member functions        */
/**************************************/

void AIG::printGate()
{
   //if (_ref == _globalref) return;
   cout << "[" << _printOrder++ << "] AIG " << getID();
   for (size_t i=0; i<2; ++i){
      cout << " ";
      /*******************here***************/
      if ( (toPtr(_fanin[i]))->getTypeStr()=="UNDEF" )
         cout << "*";
      /*************************************/
      if ( _fanin[i] & INV )
         cout << "!";
      cout << (toPtr(_fanin[i]))->getID();
   }
   cout << endl;
   //_ref = _globalref;
}

void AIG::reportGate() const
{
   stringstream ss;
   ss << "AIG(" << getID() << "), line " << _lineNo+1;
   cout << "==================================================" << endl;
   cout << "= "; 
   cout << setw(47) << left;
   cout << ss.str() << "=" << endl;
   ss.str("");
   cout << "= FECs: ";
   for (size_t i=0;i<_FecGroup.size();++i){
      if (_FecGroup[i] & INV)
         ss << "!";
      ss << toPtr(_FecGroup[i])->getID() ;
      ss << " ";
   }
   cout << setw(41) << left << ss.str() << "=" << endl;
   cout << "= Value: ";
   for (int i=8*sizeof(PatternType)-1; i>=0; --i){
      cout << ((_value & (1<<i)) >> i);
      if (i%4==0 && i>0) cout << "_";
   }
   cout << " =" << endl;
   cout << "==================================================" << endl;
}

void AIG::reportFanin(int level, bool inv)
{
   assert(level>=0);
   if (_indent == "") setGlobalref();
   cout << _indent;
   if (inv) cout << "!";
   cout << "AIG " << getID();
   if (level == 0)
   { cout << endl; return; }
   if (_ref != _globalref){
      _indent += "  ";
      cout << endl;
      for (size_t i=0; i<2; ++i){
         bool inv_next = (_fanin[i] & INV);
         (toPtr(_fanin[i]))->reportFanin(level-1, inv_next);
      }
      _ref = _globalref;
      _indent.resize(_indent.size()-2);
   } else {
      cout << " (*)" << endl;
   }
}

void AIG::reportFanout(int level, bool inv)
{
   assert(level>=0);
   if (_indent == "") setGlobalref();
   cout << _indent;
    if (inv) cout << "!";
    cout << "AIG " << getID();
    if (level == 0 || noFanout())
    { cout << endl; return; }
    if (_ref != _globalref){
       _indent += "  ";
       cout << endl;
       for (size_t i=0; i<_fanoutList.size(); ++i){
          bool inv_next = (_fanoutList[i] & INV);
          (toPtr(_fanoutList[i]))->reportFanout(level-1, inv_next);
       }
       _indent.resize(_indent.size()-2);
       _ref = _globalref;
    } else {
       cout << " (*)" << endl;
    }
 }

 // 1. If fi is assigned, store this number.
 //    Note that the size of fanin is exactly two.
 //    Check which position the input value should be placed.
 // 2. If fi is not assigned (i.e. as its default value ULONG_MAX), convert
 //    it through cirMgr into cirGate*.
 void AIG::setFanin(size_t fi)
 {
    if (fi != ULONG_MAX){
       if (_fanin[0] == ULONG_MAX) _fanin[0] = (size_t)fi;
       else if (_fanin[1] == ULONG_MAX) _fanin[1] = (size_t)fi;
       else assert(0);
    } else {
      for (size_t i=0;i<2;i++){
         assert(_fanin[i] != ULONG_MAX);   
         bool inv = (_fanin[i]%2 != 0);
         CirGate* t = cirMgr->getGate(_fanin[i]/2);
         if (t == 0)
            _fanin[i] = (size_t)(cirMgr->setUndef(_fanin[i]/2));
         else 
            _fanin[i] = (size_t)t;
         size_t p = inv? (size_t)this|INV : (size_t)this;
         ((CirGate*)_fanin[i])->setFanout(p);
         if (inv) _fanin[i] |= INV;
       }
   }
}

void AIG::setFanout(size_t fi)
{ _fanoutList.push_back(fi); }

void AIG::sortFanout()
{ std::sort(_fanoutList.begin(), _fanoutList.end(), CirSort()); }

/**************************************/
/*       Const member functions       */
/**************************************/
void Const::printGate() 
{
   assert(_fanoutList.size() != 0);
   //if (_ref == _globalref) return;
   cout << "[" << _printOrder++ << "] CONST0" << endl;
   //_ref = _globalref;
}

void Const::reportGate() const
{
   cout << "==================================================" << endl;
   cout << "= ";
   cout << setw(47) << left;
   cout << "CONST(0), line 0"; 
   cout << "=" << endl;
   stringstream ss;
   cout << "= FECs: ";
   for (size_t i=0;i<_FecGroup.size();++i){
      if (_FecGroup[i] & INV)
         ss << "!";
      ss << toPtr(_FecGroup[i])->getID() ;
      ss << " ";
   }
   cout << setw(41) << left << ss.str() << "=" << endl;
   cout << "= Value: ";
   for (int i=8*sizeof(PatternType)-1; i>=0; --i){
      cout << ((_value & (1<<i)) >> i);
      if (i%4==0 && i>0) cout << "_";
   }
   cout << " =" << endl;
   cout << "==================================================" << endl;
}

void Const::reportFanin(int level, bool inv)
{
   assert(level>=0);
   cout << _indent;
   if (inv) cout << "!";
   cout << "CONST 0" << endl;
}

void Const::reportFanout(int level, bool inv)
{
   assert(level>=0);
   assert(_indent=="");
   setGlobalref();
   if (inv) cout << "!";
   cout << "CONST " << getID();
   if (level == 0 || noFanout())
   { cout << endl; return; }
   _indent += "  ";
   cout << endl;
   for (size_t i=0; i<_fanoutList.size(); ++i){
      bool inv_next = (_fanoutList[i] & INV);
      (toPtr(_fanoutList[i]))->reportFanout(level-1, inv_next);
   }
   _indent.resize(_indent.size()-2);
}

void Const::setFanout(size_t fi)
{ _fanoutList.push_back(fi); }

void Const::sortFanout()
{ std::sort(_fanoutList.begin(), _fanoutList.end(), CirSort()); }

/**************************************/
/*       Undef member functions       */
/**************************************/
void Undef::printGate() 
{
   //assert(_fanoutList.size() != 0);
   //if (_ref == _globalref) return;
   cout << "[" << _printOrder++ << "] UNDEF" << endl;
   //_ref = _globalref;
}
void Undef::reportGate() const
{
   stringstream ss;
   ss << "UNDEF(" << getID() << "), line 0";
   cout << "==================================================" << endl;
   cout << "= ";
   cout << setw(47) << left;
   cout << ss.str() << "=" << endl;
   cout << "= FECs:                                          =" << endl;
   cout << "= Value: ";
   for (int i=8*sizeof(PatternType)-1; i>=0; --i){
      cout << ((_value & (1<<i)) >> i);
      if (i%4==0 && i>0) cout << "_";
   }
   cout << " =" << endl;
   cout << "==================================================" << endl;
}

void Undef::reportFanin(int level, bool inv)
{
   assert(level>=0);
   cout << _indent;
   if (inv) cout << "!";
   cout << "UNDEF " << getID() << endl;
}

void Undef::reportFanout(int level, bool inv)
{
   assert(level>=0);
   assert(_indent=="");
   setGlobalref();
   if (inv) cout << "!";
   cout << "UNDEF " << getID();
   if (level == 0 || noFanout())
   { cout << endl; return; }
   _indent += "  ";
   cout << endl;
   for (size_t i=0; i<_fanoutList.size(); ++i){
      bool inv_next = (_fanoutList[i] & INV);
      (toPtr(_fanoutList[i]))->reportFanout(level-1, inv_next);
   }
   _indent.resize(_indent.size()-2);
}

void Undef::setFanout(size_t fi)
{ _fanoutList.push_back(fi); }

void Undef::sortFanout()
{ std::sort(_fanoutList.begin(), _fanoutList.end(), CirSort()); }

