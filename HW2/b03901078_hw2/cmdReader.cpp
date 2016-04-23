/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2015 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO */ 
                               if(moveBufPtr(_readBufPtr-1))
                                   deleteChar();
                               break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */
                               moveBufPtr(_readBufPtr+1); break;
         case ARROW_LEFT_KEY : /* TODO */ 
                               moveBufPtr(_readBufPtr-1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */ 
                               {
                                  int repeat = 8 - ((_readBufPtr - _readBuf)%8);
                                  insertChar(' ',repeat);
                                  break;
                               }
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY  : mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
    // TODO...
    if ( (ptr > _readBufEnd) || (ptr < _readBuf) )
    {
       mybeep();
       return false;
    }
    if (ptr > _readBufPtr)
        for (; _readBufPtr<ptr; _readBufPtr++)
            cout << *_readBufPtr;
    else if (ptr < _readBufPtr)
        for (; _readBufPtr>ptr;_readBufPtr--)
             cout << (char)BACK_SPACE_CHAR; 
    return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
    // TODO...
    //   Modify the string(fill the end with space) 
    // ->Print the modified string
    // ->Set the cursor back
    // ->Change the end with 0
    if (_readBufPtr == _readBufEnd)
    { mybeep(); return false; }
    int count_of_char=0;
    for (char* ptr=_readBufPtr;ptr<=_readBufEnd-2;ptr++)
    {
        *ptr = *(ptr+1);
        cout << *ptr;
        count_of_char++;
    }
    _readBufEnd--;   *_readBufEnd = 0;
    cout << ' ';     count_of_char++;
    for (int i=0;i<count_of_char;i++)cout << (char)BACK_SPACE_CHAR;
    return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//              ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
    // TODO...
    assert(repeat >= 1);
    char* ptr;
    for (ptr=_readBufEnd; ptr>=_readBufPtr; ptr--)
        *(ptr+repeat) = *ptr; 
    for (int i=0;i<repeat;i++)
        *(_readBufPtr+i) = ch;
    _readBufEnd+=repeat;
    for (ptr=_readBufPtr;ptr<_readBufEnd;ptr++)
         cout << *ptr;
    _readBufPtr+=repeat;
    for (ptr=_readBufEnd;ptr>_readBufPtr;ptr--)
         cout << (char)BACK_SPACE_CHAR;
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
    // TODO...
    moveBufPtr(_readBufEnd);
    int count=0;
    for (; _readBufEnd>_readBuf; _readBufEnd--){
        *_readBufEnd = 0;
        cout << (char)BACK_SPACE_CHAR;
        count++;
    }
    *_readBuf = 0;
    for (int i=0;i<count;i++) cout << ' ';
    for (int i=0;i<count;i++) cout << (char)BACK_SPACE_CHAR;
    _readBufPtr =  _readBuf;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
    // TODO...
    assert(index != _historyIdx);
    if (index<_historyIdx){
        if (index<0) 
            index=0;
        if (_historyIdx==0)
            { mybeep();  return;}
        if (_historyIdx == (int)_history.size()){
            assert(_tempCmdStored==false);
            _tempCmdStored = true;
            _history.push_back(string(_readBuf,_readBufEnd));
        }
        _historyIdx = index;
        retrieveHistory();
        return;
    }
    if (index>_historyIdx){
        if (_tempCmdStored){
            if (index > (int)_history.size()-1)
                index = (int)_history.size()-1;
            else{}
            assert(_historyIdx < (int)_history.size()-1);
                _historyIdx = index;
                retrieveHistory();
            if (index == (int)_history.size()-1){
                _tempCmdStored = false;
                _history.pop_back();
            }
            else{}
            return;
        }
        else{
            if (index > (int)_history.size())
                index = (int)_history.size();
            if (_historyIdx == (int)_history.size())
                {mybeep(); return;}
            cout << "Error!" ;
            return;
        }
    }
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
    // TODO...
    char *_BufBeg = _readBuf, *_BufEnd = _readBufEnd;
    while (*_BufBeg == ' ') _BufBeg++;
    while (*(_BufEnd-1) == ' ' && _BufBeg<_BufEnd) _BufEnd--;
    if (_tempCmdStored) {_history.pop_back(); _tempCmdStored = false;}
    if (_BufBeg != _BufEnd) _history.push_back(string(_BufBeg,_BufEnd));
    _historyIdx = _history.size();    
    
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
