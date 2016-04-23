/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <climits>
#include "dbTable.h"
#include "util.h"
using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   DBTable::printData(os,r[0]);
   for (size_t i=1; i<r.size(); i++){
      os << " " ;
      DBTable::printData(os,r[i]);
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
   for (size_t i=0; i<t.nRows(); i++){
      for (size_t j=0; j<t.nCols(); j++){
         os << setw(6) << right;
         t.printData(os,t.getData(i,j));
      }
      if (i<t.nRows()-1) os << "\n";
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
   string str,tok;
   getline(ifs,str);
   size_t LineBegIdx=myStrGetTok(str,tok,0,'\r');
   do{
      tok += ",";
      int beg=0, end=0, num;
      DBRow NewRow;
      while (end < (int)tok.size()){
         while (tok[end] != ',') end++;
         if (beg == end){
            NewRow.addData(INT_MAX);
         }
         else{
            if (!myStr2Int(string(tok,beg,end-beg),num) ){
               cerr << "Error of reading number!" << endl;
               ifs.setstate(ios::failbit);
            }
            NewRow.addData(num);
         }
         beg = end = end+1;
      }
      t.addRow(NewRow);
      LineBegIdx = myStrGetTok(str,tok,LineBegIdx,'\r');
   } while(tok.size() != 0);
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin()+c);
   //_data.begin() is of iterator type.
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   size_t index=0;
   while (r1[ _sortOrder[index] ] == r2[ _sortOrder[index] ] 
          && index < _sortOrder.size()-1 ) 
      index++; 
   return r1[ _sortOrder[index] ] <= r2[ _sortOrder[index] ] ; 
}
/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   for (size_t i=0;i<nCols();i++)
      _table.pop_back();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for (size_t i=0; i<nRows(); i++)
      _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   int max=INT_MIN;
   for (size_t i=0; i<nRows(); i++){
      if (_table[i][c] != INT_MAX && _table[i][c] > max)
         max = _table[i][c];
   }
   if (max == INT_MIN) return NAN;
   else return max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   int min=INT_MAX;
   for (size_t i=0; i<nRows(); i++){
      if (_table[i][c] != INT_MAX && _table[i][c] < min)
         min = _table[i][c];
   }
   if (min == INT_MAX) return NAN;
   else return min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   int sum=0;
   for (size_t i=0; i<nRows(); i++){
      if (_table[i][c] != INT_MAX)
         sum += _table[i][c];
   }
   if (sum == INT_MAX) return NAN;
   else return sum;
}

struct DBRsort
{
   bool operator() (int a, int b) {return a<b;}
};

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   vector<int> ColCpy;
   for (size_t i=0; i<nRows(); i++)
      ColCpy.push_back(_table[i][c]);
   std::sort(ColCpy.begin(), ColCpy.end());
   int count=1;
   for (size_t i=1; i<nRows(); i++){
      if (ColCpy[i] == INT_MAX) break;
      if (ColCpy[i] != ColCpy[i-1]) count++;
   }
   return count;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   int sum = this->getSum(c);
   if (sum == NAN) return NAN;
   size_t count=0;
   for (size_t i=0; i<nRows(); i++)
      if (_table[i][c] != INT_MAX)
         count++;
   return (float)sum/count;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   printData(cout,_table[0][c]);
   for (size_t i=1; i<nRows();i++){
      cout << " ";
      printData(cout,_table[i][c]);
   }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

