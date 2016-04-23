/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator ret(*this); 
                                 _node = _node->_next; return ret; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator ret(*this);
                                 _node = _node->_prev; return ret; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator == (const iterator& i) const { return (_node == i._node); }
      bool operator != (const iterator& i) const { return !(*this==i); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return empty()? 0 : iterator(_head->_next); }
   iterator end() const { return empty()? 0 :iterator(_head); }
   bool empty() const { return (_head->_next == _head); }
   size_t size() const 
   {
      if (empty()) return 0;
      else {
         DListNode<T>* n(_head);
         size_t count = 0;
         while (n->_next != _head){
            n = n->_next;
            count++;
         }
         return count;
      }
   }

   void push_back(const T& x) 
   {
      DListNode<T>* n = new DListNode<T>(x,_head->_prev,_head);
      (_head->_prev)->_next = n;
      _head->_prev = n;
   }
   void pop_front() 
   {
      if (empty()) return;
      DListNode<T>* second = (_head->_next)->_next;
      delete _head->_next;
      second->_prev = _head;
      _head->_next = second;
   }
   void pop_back() 
   {
      if (empty()) return;
      DListNode<T>* newTail = (_head->_prev)->_prev;
      delete _head->_prev;
      newTail->_next = _head;
      _head->_prev = newTail;
   }

   // return false if nothing to erase
   bool erase(iterator pos)
   {
      if (empty()) return false;
      DListNode<T>* n = pos._node;
      (n->_next)->_prev = n->_prev;
      (n->_prev)->_next = n->_next;
      delete n;
      return true;
   }
   bool erase(const T& x) 
   {
      if (empty()) return false;
      iterator i = begin();
      while (!(*i==x) && i!=end()) i++;
      if (i==end()) return false;   //x is not found in the list
      return erase(i);
   }

   void clear()      // delete all nodes except for the dummy node
   {
      while (!empty())
         pop_front();
   }

   void sort() const 
   {
      for (iterator i=++(begin()); i!=end(); i++)
         for (iterator j=begin(); j!=i; j++)
            if (*i < *j)
            { insert(j,i); break; }
   }

private:
   DListNode<T>*  _head;  // = dummy node if list is empty

   // [OPTIONAL TODO] helper functions; called by public member functions
   #define swap(i,j)              \
   T temp = *i;                   \
   *i = *j;                       \
   *j = temp        

   #define prev(i) (--i)++
   void insert(iterator head, iterator tail) const
   {
      for (iterator i=tail; i!=head; i--)
      { iterator j=prev(i); swap(i,j); } 
   }
};

#endif // DLIST_H
