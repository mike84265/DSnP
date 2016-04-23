/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) 
   { 
      size_t i = _data.size();
      _data.push_back(d);
      #define parent(x) (i-1)/2
      while (i>0 && d<_data[parent(i)]){
         swap(i, parent(i));
         i = parent(i);
      }
   }
   void delMin() { delData(0); }
   void delData(size_t i) 
   { 
      if (i>=_data.size()) return;
      _data[i] = _data.back();
      _data.pop_back();
      #define left(x) 2*x+1
      #define right(x) 2*x+2
      while (left(i) < _data.size()){
         if (right(i) == _data.size()){
            if (_data[left(i)] < _data[i])
               swap(i, left(i));
            break;
         }
         else if (!(_data[i] < _data[left(i)]) ||
             !(_data[i] < _data[right(i)])){
            if (_data[left(i)] < _data[right(i)])
            { swap(i, left(i)); i=left(i); }
            else
            { swap(i, right(i)); i=right(i); }
         }
         else break;
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;

   inline void swap(size_t i, size_t j){
      Data temp = _data[i];
      _data[i] = _data[j];
      _data[j] = temp;
   }
};

#endif // MY_MIN_HEAP_H
