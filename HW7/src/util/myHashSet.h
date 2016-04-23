/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;
   public:
      iterator() : _data(0), _bucketHead(0), _localNumBuckets(0) {}
      iterator(Data* d, vector<Data>* p, size_t n)
         : _bucketHead(p), _localNumBuckets(n) {
            _data = d;
         }
      ~iterator() {}
      Data& operator* () const { return *_data; }
      iterator& operator++() {
         #ifdef DEBUG
         cout << "_data = " << *_data << " (" << _data << ")\n";
         #endif
         vector<Data>* bucket = _bucketHead + localBucketNum(*_data);
         if ((_data - &(*bucket)[0] == bucket->size()-1)){
            if (localBucketNum(*_data) == _localNumBuckets-1)
               _data = (Data*)((size_t)_data | (size_t)(0x1));
            else {
               while (1){
                  ++bucket;
                  if (bucket->size() != 0)
                  { _data = &(*bucket)[0]; return *this;}
                  else if (bucket - _bucketHead == _localNumBuckets-1)
                  { _data = (Data*)((size_t)_data | (size_t)0x1); return *this; }
               }
            }
         }
         else 
            ++_data;
         return *this;
      }
      iterator operator++(int) {
         iterator ret = *this;
         ++(*this);
         return ret;
      }
      iterator& operator--() {
         if ((size_t)_data & (size_t)0x1){
            vector<Data>* bucket = _bucketHead + _localNumBuckets - 1;
            _data = &(bucket->back());
         }
         else {
            vector<Data>* bucket = _bucketHead + localBucketNum(*_data);
            if (_data == &(*bucket)[0]) {
               if (bucket == _bucketHead) 
                  return *this;
               while (1){
                  --bucket;
                  if (bucket->size() != 0)
                     _data = &bucket->back();
                  else if (bucket == _bucketHead)
                     return *this;
               }
            }
            else
               --_data;
         }
         return *this;
      }
      iterator operator--(int) {
         iterator ret = *this;
         --(*this);
         return ret;
      }
      Data& operator= (const iterator& d) { this->_data = d._data; }
      bool operator== (const iterator& d) const {
         if (((size_t)_data & (size_t)0x1) && ((size_t)d._data & (size_t)0x1) )
            return true;
         else 
            return this->_data == d._data; 
      }
      bool operator!= (const iterator& d) const { return !(this->_data == d._data); }

   private:
      size_t localBucketNum(const Data& d) const
      { return d() % _localNumBuckets; }
      Data*                _data;
      vector<Data>*        _bucketHead;
      size_t               _localNumBuckets;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      size_t first = 0;
      while (first<_numBuckets && _buckets[first].size()==0) ++first;
      if (first == _numBuckets) return iterator();
      return iterator(&(_buckets[0][0]), _buckets, _numBuckets); 
   }
   // Pass the end
   iterator end() const { 
      int last = _numBuckets-1;
      while (last>=0 && _buckets[last].size() == 0) --last;
      if (last < 0) return iterator();
      Data *d = &(_buckets[last].back());
      d = (Data*)((size_t)d | (size_t)0x1);
      return iterator(d, _buckets, _numBuckets); 
   }
   // return true if no valid data
   bool empty() const 
   { 
      for (size_t i=0;i<_numBuckets;++i){
         if (!_buckets[i].empty())
            return false;
      }
      return true;
   }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
      for (size_t i=0; i<_numBuckets; ++i)
         s += _buckets[i].size();
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const 
   { 
      const vector<Data>& bucket = _buckets[bucketNum(d)];
      for (int i=0;i<bucket.size();++i){
         if (bucket[i] == d)
            return true;
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const 
   { 
      const vector<Data>& bucket = _buckets[bucketNum(d)];
      for (int i=0;i<bucket.size();++i)
         if (d == bucket[i]){
            d = bucket[i];
            return true;
         }
      return false; 
   }

   // update the entry in hash that is equal to d
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      vector<Data>& bucket = _buckets[bucketNum(d)];
      for(size_t i=0; i<bucket.size(); ++i)
         if (bucket[i] == d){
            bucket[i] = d;
            return true;
         }
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if (check(d)) return false;
      _buckets[bucketNum(d)].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      if (!check(d)) return false;
      vector<Data>& bucket = _buckets[bucketNum(d)];
      for (size_t i=0; i<bucket.size(); ++i)
         if (bucket[i] == d){
            bucket[i] = bucket.back();
            bucket.pop_back();
            return true;
         }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
