/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <vector>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   BSTreeNode(const T& x, BSTreeNode<T>* l=0, BSTreeNode<T>* r=0)
      :_data(x), _leftChild(l), _rightChild(r) {}

   //Member variables:
   T                _data;
   BSTreeNode<T>*   _leftChild;
   BSTreeNode<T>*   _rightChild;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree() : _root(0), _size(0),
              _head((BSTreeNode<T>*)0),
              _tail((BSTreeNode<T>*)0) {}
   ~BSTree() { clear(); }
   class iterator 
   { 
      friend class BSTree;
   public:
      iterator(BSTreeNode<T>* n=0) : _node(n) {}
      iterator(const iterator& i) : _node(i._node), _trace(i._trace) {}
      iterator(BSTreeNode<T>* n, vector< BSTreeNode<T>* > t) : 
         _node(n), _trace(t) {}
      ~iterator() {}
      //Operator:
      const T& operator * () const { return _node->_data; } 
      T& operator* () { return _node->_data; }
      // 1. Test whether iterator is tail, if is, do nothing.
      // 2. Use the helper function to get the next node.
      // 3. For the suffix ++, copy the iterator first.
      iterator& operator ++ () 
      { 
         *this = this->getNext(); 
         return *this;
      }
      iterator operator ++ (int)
      { 
         iterator ret(*this); 
         *this = this->getNext();
         return ret;
      }
      // 1. Test whether iterator is head, if is, do nothing.
      // 2. Use the helper function to get the previous node.
      // 3. For the suffix --, copy the iterator first.
      iterator& operator -- () {
         *this = this->getPrev(); 
         return *this; 
      }
      iterator  operator -- (int){
         iterator ret(*this); 
         *this = this->getPrev(); 
         return ret; 
      }
      iterator& operator = (const iterator& i){
         _node = i._node;
         _trace = i._trace;
         return *this;
      }
      bool operator == (const iterator& i) { return (_node == i._node); }
      bool operator != (const iterator& i) { return !(*this == i); }
    private:
      //Member variables:
      BSTreeNode<T>* _node;
      vector< BSTreeNode<T>* > _trace;
      
      iterator getNext () const
      {
         BSTreeNode<T>* n = this->_node;
         vector< BSTreeNode<T>* > trc = this->_trace;
         iterator endNode(0,trc);
         endNode._trace.push_back(n);
         if (n==0) return endNode;
         if (n->_rightChild!=0){
            trc.push_back(n);
            n = n->_rightChild;
            while (n->_leftChild!=0){
               trc.push_back(n);
               n = n->_leftChild;
            }
         }//end if
         else{
            if (trc.empty())
               return endNode;  //root, and end node
            while ( !(trc.empty()) &&
                     (trc.back())->_rightChild == n ){
               n = trc.back();
               trc.pop_back();
            }
            if (trc.empty())
               return endNode;  //End node
            n = trc.back();
            trc.pop_back();
         }//end else
         return iterator(n, trc);
      }//end getNext

      iterator getPrev() const
      {
         BSTreeNode<T>* n = this->_node;
         vector< BSTreeNode<T>* > trc = this->_trace;
         if (n==0){           //Special case for end()
            n = trc.back();
            trc.pop_back();
            return iterator(n, trc);
         }
         if (n->_leftChild!=0){
            trc.push_back(n);
            n = n->_leftChild;
            while (n->_rightChild!=0){
               trc.push_back(n);
               n = n->_rightChild;
            }
         }
         else{
            while ( !(trc.empty()) &&
                     (trc.back())->_leftChild == n  ){
               n = trc.back();
               trc.pop_back();
            }
            if (trc.empty())
               return *this;      //Head node.
            n = trc.back();
            trc.pop_back();
         }//end else
         return iterator(n, trc);
      }//end getPrev
   }; //end iterator
   iterator begin() const { return _head; }
   iterator end() const 
   { 
      iterator ret(_tail);
      ret._node = 0;
      (ret._trace).push_back(_tail._node);
      return ret;
   }
   bool empty() const { return (_size == 0); }
   // 1.Check whether _head and _tail would be changed.
   // 2.If x<*_head, push_front, if x>*_tail, push_back.
   // 3.Using binary search to find the proper position,
   //   where the relationship is correct and the node
   //   is not occupied.
   void insert(const T& x)
   {
      if (empty() || x < *_head )
      { push_front(x); return; }
      else if ( *_tail < x || *_tail == x )
      { push_back(x); return; }
      else{
         BSTreeNode<T>* pos = _root;
         while (1){
            if (x < pos->_data){
               if (pos->_leftChild != 0)
                  pos = pos->_leftChild;
               else
               { pos->_leftChild = new BSTreeNode<T>(x); break; }
            }
            else{
               if (pos->_rightChild != 0)
                  pos = pos->_rightChild;
               else
               { pos->_rightChild = new BSTreeNode<T>(x); break; }
            }
         }
         ++_size;
      }
   }
   // 1.Set _head->_leftChild=x
   // 2.Update _head by getPrev(), which also updates _trace.
   void push_front(const T& x)
   {
      if (empty()){
         _root = new BSTreeNode<T>(x);
         _head._node = _tail._node = _root;
      }
      else{
         (_head._node)->_leftChild = new BSTreeNode<T>(x);
         _head = _head.getPrev();
      }
      ++_size;
   }
   // 1.Set _tail->_rightChild=x
   // 2.Update _tail by getNext().
   void push_back(const T& x)
   {
      
      if (empty()) push_front(x);
      else{
         (_tail._node)->_rightChild = new BSTreeNode<T>(x);
         _tail = _tail.getNext();
         ++_size;
      }
   }
   // 1.Delete the _head and move it to the next
   // 2.Note that for degree-1 node, the topology is changed,
   //   the trace of _head should be updated accordingly.
   void pop_front()
   {
      if (empty()) return;
      iterator newHead(0);
      if (_size>1) newHead = _head.getNext();
      if (degree(_head._node) == 1){
         for (int i=(int)newHead._trace.size()-1; i>=0; i--){
            if (newHead._trace[i] == _head._node){
               newHead._trace.erase(newHead._trace.begin()+i);
               break;
            }
         }
      }
      if (!deleteNode(_head, degree(_head._node))){
         //Root is assigned to be deleted
         BSTreeNode<T>* temp = _root->_rightChild;
         delete _root;
         _root = temp;
      }
      _head = newHead;
      --_size;
   }
   // 1.Delete _tail(dummy node) and _tail-1, note the degree-1 case.
   // 2.Insert back the dummy node to the current _tail.
   void pop_back()
   {
      if (empty()) return;
      iterator newTail(0);
      if (_size>1) newTail = _tail.getPrev();
      if (degree(_tail._node) == 1){
         for (int i=(int)newTail._trace.size()-1;i>=0; i--){
            if (newTail._trace[i] == _tail._node){
               newTail._trace.erase(newTail._trace.begin()+i);
               break;
            }
         }
      }
      if (!deleteNode(_tail, degree(_tail._node))){
         //Root is assigned to be deleted
         BSTreeNode<T>* temp = _root->_leftChild;
         delete _root;
         _root = temp;
      }
      _tail = newTail;
      --_size;
   }
   // 1.Check whether the iterator is _head or _tail, if true,
   //   call pop() to free it.
   // 2.Otherwise, call deleteNode.
   bool erase(iterator i)
   {
      if (empty()) return false;
      if (i == _head) { pop_front(); return true; }
      if (i == _tail) { pop_back(); return true; }
      deleteNode(i, degree(i._node));
      --_size;
      return true;
   }
   // 1.Check whether it matches _head or _tail, if true,
   //   just call pop() to free it.
   // 2.Traverse each node from _head, using operator++.
   // 3.Find the iterator, then call erase(iterator).
   bool erase(const T& x)
   {
      if (empty()) return false;
      iterator i = find(x);
      if (i._node == 0) return false;
      return erase(i);
   }
   void clear()
   { while(!empty()) pop_front(); }
   size_t size() const { return _size; } 
   void sort() const {}    //No need to implement.
   void print() const
   { 
      if (empty()) return;
      inorder(_root); 
   }
   
 private:
   BSTreeNode<T>* _root;
   size_t _size;
   iterator _head;
   iterator _tail;
   // Helper functions:

   // Traverse from the first node to find x.
   iterator find(const T& x)
   {
      if (empty()) return iterator(0);
      if (*_tail == x) return _tail;
      for (iterator ret(_head); ret!=_tail; ++ret){
         if (*ret == x) return ret;
      }
      return iterator(0);
   }
   // 1. For degree-0 node, find its parent (by ++ or --), 
   //    reset the pointer, then delete the node.
   // 2. For degree-1 node, find its parent (by +-1, or +-2),
   //    deliver its child to the parent, then delete the node.
   // 3. For degree-2 node, find its successor,
   //    copy its content to the current position,
   //    then delete the position where its successor is.
   //    (converge to either of the above cases).
   size_t degree(const BSTreeNode<T>* n)
   {
      size_t ret = 0;
      if (n->_leftChild != 0) ++ret;
      if (n->_rightChild != 0) ++ret;
      return ret;
   }
   bool deleteNode(iterator i, const size_t deg)
   {
      switch(deg){
         case 0: {
            BSTreeNode<T>* n = i._node;
            if (!(i._trace).empty()){
               BSTreeNode<T>* parent = (i._trace).back();
               if (parent->_leftChild == n)
                  parent->_leftChild = 0;
               else
                  parent->_rightChild = 0;
            }
            delete n;
         } break;
         case 1: {
            BSTreeNode<T>* n = i._node;
            if (!(i._trace).empty()){
               BSTreeNode<T>* parent = (i._trace).back();
               (i._trace).pop_back();
               if (parent->_leftChild == n){
                  if (n->_rightChild == 0)
                     parent->_leftChild = n->_leftChild;
                  else
                     parent->_leftChild = n->_rightChild;
               }
               else if (parent->_rightChild == n){
                  if (n->_rightChild == 0)
                     parent->_rightChild = n->_leftChild;
                  else
                     parent->_rightChild = n->_rightChild;
               }
               if (n->_data < *_head){
                  for (int idx=(int)_head._trace.size()-1;idx>=0;idx--)
                     if (_head._trace[idx] == i._node)
                        _head._trace.erase(_head._trace.begin()+idx);
               }
               else{
                  for (int idx=(int)_tail._trace.size()-1;idx>=0;idx--)
                     if (_tail._trace[idx] == i._node)
                        _tail._trace.erase(_tail._trace.begin()+idx);
               }
            }
            else{    //Special case, _root will be changed.
               return false;
            }
            delete n;
         } break;
         case 2: {
            assert (i!=_head);
            assert (i!=_tail);
            iterator successor = i.getNext();  
            *i = *successor;
            deleteNode(successor,degree(successor._node));
         } break;
         default:
            cerr << "Error!! degree can't be" << deg << "!!!" << endl;
            return false;
      }//end switch
      return true;
   }//end deleteNode
   void inorder(BSTreeNode<T>* n, int d=0) const
   {
      for (int i=0; i<d; i++)
         cout << " ";
      cout << n->_data << endl;
      for (int i=0; i<d; i++)
         cout << " ";
      cout << "↙" << endl;
      if ( n->_leftChild != 0 )
         inorder(n->_leftChild, d+2);
      else{
         for (int i=0;i<d;i++) cout << " ";
         cout << "LNull" << endl;
      }
      for (int i=0; i<d; i++)
         cout << " ";
      cout << "↘" << endl;
      if ( n->_rightChild != 0)
         inorder(n->_rightChild, d+2);
      else{
         for (int i=0;i<d;i++) cout << " ";
         cout << "RNull" << endl;
      }
   }
};

#endif // BST_H
