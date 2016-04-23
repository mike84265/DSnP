/****************************************************************************
  FileName     [ memMgr.h ]
  PackageName  [ cmd ]
  Synopsis     [ Define Memory Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Turn this on for debugging
// #define MEM_DEBUG

//--------------------------------------------------------------------------
// Define MACROs
//--------------------------------------------------------------------------
#define MEM_MGR_INIT(T) \
MemMgr<T>* const T::_memMgr = new MemMgr<T>
//static member initialization

#define USE_MEM_MGR(T)                                                      \
public:                                                                     \
   void* operator new(size_t t) { return (void*)(_memMgr->alloc(t)); }      \
   void* operator new[](size_t t) { return (void*)(_memMgr->allocArr(t)); } \
   void  operator delete(void* p) { _memMgr->free((T*)p); }                 \
   void  operator delete[](void* p) { _memMgr->freeArr((T*)p); }            \
   static void memReset(size_t b = 0) { _memMgr->reset(b); }                \
   static void memPrint() { _memMgr->print(); }                             \
private:                                                                    \
   static MemMgr<T>* const _memMgr

// You should use the following two MACROs whenever possible to 
// make your code 64/32-bit platform independent.
// DO NOT use 4 or 8 for sizeof(size_t) in your code
//
#define SIZE_T      sizeof(size_t)
#define SIZE_T_1    (sizeof(size_t) - 1)

// TODO: Define them by SIZE_T and/or SIZE_T_1 MACROs.
//
// To promote 't' to the nearest multiple of SIZE_T; 
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16
// TODO
#define toSizeT(t)         \
(((t-1)/SIZE_T)+1)*SIZE_T
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
// TODO
#define downtoSizeT(t)  \
((t-1)/SIZE_T)*SIZE_T
// R_SIZE is the size of the recycle list
#define R_SIZE 256

//--------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------
template <class T> class MemMgr;


//--------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------
// T is the class that use this memory manager
//
// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemBlock
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemBlock(MemBlock<T>* n, size_t b) : _nextBlock(n) {
      _begin = _ptr = new char[b]; _end = _begin + b; }
   ~MemBlock() { delete [] _begin; }

   // Member functions
   void reset() { _ptr = _begin; }
   // 1. Get (at least) 't' bytes memory from current block
   //    Promote 't' to a multiple of SIZE_T
   // 2. Update "_ptr" accordingly
   // 3. The return memory address is stored in "ret"
   // 4. Return false if not enough memory
   bool getMem(size_t t, T*& ret) {
      // TODO
      t = toSizeT(t);
      if (getRemainSize() < t) return false;
      ret = (T*)_ptr;
      _ptr = _ptr + t;
      return true;
   }
   size_t getRemainSize() const { return size_t(_end - _ptr); }
      
   MemBlock<T>* getNextBlock() const { return _nextBlock; }

   // Data members
   char*             _begin;
   char*             _ptr;
   char*             _end;
   MemBlock<T>*      _nextBlock;
};

// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemRecycleList
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemRecycleList(size_t a = 0) : _arrSize(a), _first(0), _nextList(0) {}
   ~MemRecycleList() { reset(); }

   // Member functions
   // ----------------
   size_t getArrSize() const { return _arrSize; }
   MemRecycleList<T>* getNextList() const { return _nextList; }
   void setNextList(MemRecycleList<T>* l) { _nextList = l; }
   // pop out the first element in the recycle list
   T* popFront() {
      // TODO
      T* ret = _first;
      _first = getNext(_first);
      return ret;
   }
   // push the element 'p' to the beginning of the recycle list
   void  pushFront(T* p) {
      // TODO
      size_t NextAddr = (size_t)_first;
      *(size_t*)p = NextAddr;
      _first = p;
   }
   // Release the memory occupied by the recycle list(s)
   // DO NOT release the memory occupied by MemMgr/MemBlock
   void reset() {
      // TODO
      _first = 0;
      // To ensure the next time the program access the list,
      // there's nothing to return, and no way to "getNext".
      // But not deleting anything since the memory is occupied
      // by the memBlock.
      if (_nextList){
         delete _nextList;
         _nextList = 0;
      }
      //Recursively calling delete operator to free the whole linked list.
   }

   // Helper functions
   // ----------------
   // Iterate to the next element after 'p' in the recycle list
   // The first size_t bytes of 'p' is the address of the next element
   T* getNext(T* p) const {
      // TODO
      if (p==0) return 0;
      size_t addr = *(size_t*)p;
      return (T*)addr;
   }
   //
   // count the number of elements in the recycle list
   size_t numElm() const {
      size_t count = 0;
      T* p = _first;
      while (p) {
         p = getNext(p);
         ++count;
      }
      return count;
   }

   // Data members
   size_t              _arrSize;   // the array size of the recycled data
   T*                  _first;     // the first recycled data
   MemRecycleList<T>*  _nextList;  // next MemRecycleList
                                   //      with _arrSize + x*R_SIZE
};

template <class T>
class MemMgr
{
   #define S sizeof(T)

public:
   MemMgr(size_t b = 65536) : _blockSize(b) {
      assert(b % SIZE_T == 0);
      _activeBlock = new MemBlock<T>(0, _blockSize);
      for (int i = 0; i < R_SIZE; ++i)
         _recycleList[i]._arrSize = i;
   }
   ~MemMgr() { reset(); delete _activeBlock; }

   // 1. Remove the memory of all but the firstly allocated MemBlocks
   //    That is, the last MemBlock searched from _activeBlock.
   //    reset its _ptr = _begin (by calling MemBlock::reset())
   // 2. reset _recycleList[]
   // 3. 'b' is the new _blockSize; "b = 0" means _blockSize does not change
   //    if (b != _blockSize) reallocate the memory for the first MemBlock
   // 4. Update the _activeBlock pointer
   void reset(size_t b = 0) {
      if (b!=0) b = toSizeT(b);
      #ifdef MEM_DEBUG
      cout << "Resetting memMgr...(" << b << ")" << endl;
      #endif // MEM_DEBUG
      // TODO
      while (_activeBlock->getNextBlock()){
         MemBlock<T>* p = _activeBlock;
         _activeBlock = _activeBlock->getNextBlock();
         delete p;
      }
      _activeBlock->reset();
      for (int i=0;i<R_SIZE;i++)
         _recycleList[i].reset();
      if (b!=0 && b!=_blockSize){
         delete _activeBlock;
         _activeBlock = new MemBlock<T>(0,b);
         _blockSize = b;
      }
   }
   // Called by new
   T* alloc(size_t t) {
      assert(t == S);
      #ifdef MEM_DEBUG
      cout << "Calling alloc...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      try{
         return getMem(t);
      }catch(bad_alloc& ba){
         cerr << "Requested memory (" << toSizeT(t) << ") is greater than block s"
              << "(" << _blockSize << "). " << "Exception raised...\n";
         throw ba;
      }
   }
   // Called by new[]
   T* allocArr(size_t t) {
      #ifdef MEM_DEBUG
      assert(t%S == SIZE_T);     //optional check
      cout << "Calling allocArr...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // Note: no need to record the size of the array == > system will do
      try{
         return getMem(t);
      }catch(bad_alloc& ba){
         cerr << "Requested memory (" << toSizeT(t) << ") is greater than block size"
              << "(" << _blockSize << "). " << "Exception raised...\n";
         throw ba;
      }
   }
   // Called by delete
   void  free(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling free...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      getMemRecycleList(0)->pushFront(p);
   }
   // Called by delete[]
   void  freeArr(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling freeArr...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      // TODO
      // Get the array size 'n' stored by system,
      // which is also the _recycleList index
      size_t n = *((size_t*)p);
      #ifdef MEM_DEBUG
      cout << ">> Array size = " << n << endl;
      cout << "Recycling " << p << " to _recycleList[" << n << "]" << endl;
      #endif // MEM_DEBUG
      MemRecycleList<T>* r = getMemRecycleList(n);
      r->pushFront(p);
      // add to recycle list...
   }
   void print() const {
      cout << "=========================================" << endl
           << "=              Memory Manager           =" << endl
           << "=========================================" << endl
           << "* Block size            : " << _blockSize << " Bytes" << endl
           << "* Number of blocks      : " << getNumBlocks() << endl
           << "* Free mem in last block: " << _activeBlock->getRemainSize()
           << endl
           << "* Recycle list          : " << endl;
      int i = 0, count = 0;
      while (i < R_SIZE) {
         const MemRecycleList<T>* ll = &(_recycleList[i]);
         while (ll != 0) {
            size_t s = ll->numElm();
            if (s) {
               cout << "[" << setw(3) << right << ll->_arrSize << "] = "
                    << setw(10) << left << s;
               if (++count % 4 == 0) cout << endl;
            }
            ll = ll->_nextList;
         }
         ++i;
      }
      cout << endl;
   }

private:
   size_t                     _blockSize;
   MemBlock<T>*               _activeBlock;
   MemRecycleList<T>          _recycleList[R_SIZE];

   // Private member functions
   //
   // t: #Bytes; MUST be a multiple of SIZE_T
   // return the size of the array with respect to memory size t
   // [Note] t must >= S
   // [NOTE] Use this function in (at least) getMem() to get the size of array
   //        and call getMemRecycleList() later to get the index for
   //        the _recycleList[]
   size_t getArraySize(size_t t) const {
      assert(t >= S);
      // TODO
      if (t >= S+SIZE_T)
         return (t-SIZE_T)/S;
      return 0;
   }

   // Go through _recycleList[m], its _nextList, and _nextList->_nextList, etc,
   //    to find a recycle list whose "_arrSize" == "n"
   // If not found, create a new MemRecycleList with _arrSize = n
   //    and add to the last MemRecycleList
   // So, should never return NULL
   // [Note]: This function will be called by MemMgr->getMem() to get the
   //         recycle list. Therefore, the recycle list is first created
   //         by the MTNew command, not MTDelete.
   MemRecycleList<T>* getMemRecycleList(size_t n) {
      size_t m = n % R_SIZE;
      // TODO
      MemRecycleList<T>* RL = &(_recycleList[m]);
      if (RL->_arrSize == n) return RL;
      while (RL->_nextList!=0){
         RL = RL->_nextList;
         if (RL->_arrSize == n)
            return RL;
      }
      RL->_nextList = new MemRecycleList<T>(n);
      return RL->_nextList;
   }

   // t is the #Bytes requested from new or new[]
   // Note: Make sure the returned memory is a multiple of SIZE_T
   T* getMem(size_t t) {
      T* ret = 0;
      #ifdef MEM_DEBUG
      cout << "Calling MemMgr::getMem...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // TODO ---
      // 1. Make sure to promote t to a multiple of SIZE_T
      // 2. Check if the requested memory is greater than the block size.
      //    If so, throw a "bad_alloc()" exception.
      //    cerr << "Requested memory (" << t << ") is greater than block size"
      //         << "(" << _blockSize << "). " << "Exception raised...\n";
      // 3. Check the _recycleList first...
      //    #ifdef MEM_DEBUG
      //    cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
      //    #endif // MEM_DEBUG
      //    => 'n' is the size of array
      //    => "ret" is the return address

      // If no match from recycle list...
      // 4. Get the memory from _activeBlock
      // 5. If not enough, recycle the remained memory and print out ---
      //    Note: recycle to the as biggest array index as possible
      //    Note: rn is the array size
      //    #ifdef MEM_DEBUG
      //    cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
      //    #endif // MEM_DEBUG
      //    ==> allocate a new memory block, and print out ---
      //    #ifdef MEM_DEBUG
      //    cout << "New MemBlock... " << _activeBlock << endl;
      //    #endif // MEM_DEBUG
      // 6. At the end, print out the acquired memory address
      //    #ifdef MEM_DEBUG
      //    cout << "Memory acquired... " << ret << endl;
      //    #endif // MEM_DEBUG
      t = toSizeT(t);
      size_t arrSize = getArraySize(t);
      if (t>_blockSize){
         throw bad_alloc();
      }
      MemRecycleList<T>* r = getMemRecycleList(arrSize);
      if (r->_first!=0) {
         ret = r->popFront();
         #ifdef MEM_DEBUG
         cout << "Recycled from _recycleList[" << arrSize
              << "]..." << ret << endl;
         #endif // MEM_DEBUG
         return ret;
      }

      if (!(_activeBlock->getMem(t,ret))){
         size_t rmSize = _activeBlock->getRemainSize();
         if (rmSize >= S){
            size_t rmArrSize = getArraySize(rmSize);
            r = getMemRecycleList(rmArrSize);
            r->pushFront((T*)_activeBlock->_ptr);
            #ifdef MEM_DEBUG
            cout << "Recycling " << (T*)_activeBlock->_ptr
                 << " to _recycleList[" << rmArrSize << "]\n";
            #endif //MEM_DEBUG
         }
         MemBlock<T>* newBlock = new MemBlock<T>(_activeBlock, _blockSize);
         _activeBlock = newBlock;
         #ifdef MEM_DEBUG
         cout << "New MemBlock... " << _activeBlock << endl;
         #endif // MEM_DEBUG
         _activeBlock->getMem(t,ret);
      } 
      #ifdef MEM_DEBUG
      cout << "Memory acquired... " << ret << endl;
      #endif
      return ret;
   }//End getMem

   // Get the currently allocated number of MemBlock's
   size_t getNumBlocks() const {
      // TODO
      MemBlock<T>* p = _activeBlock;
      size_t count = p? 1 : 0;
      while (p->getNextBlock()){
         p = p->getNextBlock();
         count++;
      }
      return count;
   }

};

#endif // MEM_MGR_H
