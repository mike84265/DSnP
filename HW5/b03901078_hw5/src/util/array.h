/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return *(_node); }
      T& operator * () { return *(_node); }
      iterator& operator ++ () { ++(_node); return *this; }
      iterator  operator ++ (int) { iterator ret(*this);
                                    ++(_node); return ret; }
      iterator& operator -- () { --(_node); return *this; }
      iterator  operator -- (int) { iterator ret(*this);
                                    --(_node); return ret; }

      iterator operator + (int i) const { return iterator(_node+i); }
      iterator operator - (int i) const { return iterator(_node-i); }
      iterator& operator += (int i) { _node+=i; return *(this); }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator == (const iterator& i) const { return (_node==i._node); }
      bool operator != (const iterator& i) const { return !(*this==i); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const 
   {  
      if (_capacity == 0) return 0;
      return iterator(_data);
   }
   iterator end() const 
   { 
      if (_capacity == 0) return 0; 
      return iterator(_data + _size);
   }
   bool empty() const { return (_capacity==0 || _size==0); }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[0]; }
   const T& operator [] (size_t i) const { return _data[0]; }

   void push_back(const T& x) 
   {
      if (_capacity == 0){
         _data = new T[1];
         _capacity = 1;
      }
      else if (_size == _capacity){
         reserve(_capacity*2);
         _capacity *= 2;
      }

      _data[_size] = x;
      ++(_size);
   }
   void pop_front() 
   {
      if (empty()) return;
      for (size_t i=1; i<_size; i++)
         _data[i-1] = _data[i];
      --(_size);
   }
   void pop_back() 
   {
      if (empty()) return;
      --(_size);
   }

   bool erase(iterator pos) 
   { 
      if (empty() || pos==end() ) return false; 
      for (iterator i=pos+1; i!=end(); i++)
         *(i-1) = *i;
      --(_size);
      return true;
   }
   bool erase(const T& x) { 
      if (empty()) return false; 
      iterator i;
      for (i=begin(); i!=end(); i++)
         if (*i == x) { return erase(i); } 
      return false;
   }

   void clear() { _size = 0; }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   void reserve(size_t n) 
   {
      if (n <= _capacity) return;
      T* newArr = new T[n];
      for (size_t i=0; i<_size; i++)
         newArr[i] = *(_data+i);
      _data = newArr;
   }
   // void resize(size_t n) { ... }

private:
   T*           _data;
   size_t       _size;       // number of valid elements
   size_t       _capacity;   // max number of elements

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
