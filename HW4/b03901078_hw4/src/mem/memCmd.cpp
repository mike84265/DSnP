/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

static string to_string(int n)
{
   string ret;
   char str[10];
   int i;
   for (i=0; n!=0; i++){
      str[i] = (n%10) + '0' ;
      n/=10;
   }
   for (i--; i>=0; i--)
      ret.push_back(str[i]);
   return ret;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj))) ) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> tok;
   if (!CmdExec::lexOptions(option,tok))
      return CMD_EXEC_ERROR;
   if (tok.empty())
      return errorOption(CMD_OPT_MISSING,"");
   int arrSize=0, numObj=0, dummy;
   bool newArr = false;
   for (size_t i=0; i<tok.size(); i++){
      if (!myStr2Int(tok[i],dummy)){
         if (myStrNCmp("-Array",tok[i],2) == 0){
            if (newArr)
               return errorOption(CMD_OPT_EXTRA,tok[i]);
            i++;
            if (i >= tok.size() )
               return errorOption(CMD_OPT_MISSING,tok[i-1]);
            newArr = true;
            if (!myStr2Int(tok[i],arrSize))
               return errorOption(CMD_OPT_ILLEGAL,tok[i]);
            if (arrSize <= 0)
               return errorOption(CMD_OPT_ILLEGAL,tok[i]);
            if (numObj!=0){
               if (i != tok.size()-1)
                  return errorOption(CMD_OPT_EXTRA,tok[i+1]);
               else
                  break;
            }
         }
         else
            return errorOption(CMD_OPT_ILLEGAL, tok[i]);
      }
      else if (dummy <= 0)
         return errorOption(CMD_OPT_ILLEGAL,tok[i]);
      else if (numObj != 0)
         return errorOption(CMD_OPT_EXTRA,tok[i]);
      else
         numObj = dummy;
   }
   if (numObj == INT_MAX || numObj == 0)
      return errorOption(CMD_OPT_MISSING, "");
   if (newArr){
      if (arrSize == INT_MAX)
         return errorOption(CMD_OPT_MISSING, "");
      mtest.newArrs((size_t)numObj,(size_t)arrSize);
   }
   else
      mtest.newObjs((size_t)numObj);
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> tok;
   CmdExec::lexOptions(option,tok);
   int objId = INT_MIN;
   int randNum = INT_MIN;
   if (tok.empty()) return errorOption(CMD_OPT_MISSING,"");
   bool delArr = false;
   char optionRead = 0;
   for (size_t i=0; i<tok.size(); i++){
      if (myStrNCmp("-Array",tok[i],2) == 0){
         if (delArr)
            return errorOption(CMD_OPT_EXTRA,tok[i]);
         delArr = true;
      }
      else if (myStrNCmp("-Index",tok[i],2) == 0){
         if (optionRead)
            return errorOption(CMD_OPT_EXTRA,tok[i]);
         optionRead = 'i';
         i++;
         if (i >= tok.size())
            return errorOption(CMD_OPT_MISSING,tok[i-1]);
         if (!myStr2Int(tok[i],objId))
            return errorOption(CMD_OPT_ILLEGAL,tok[i]);
         if (objId < 0)
            return errorOption(CMD_OPT_ILLEGAL,tok[i]);
      }
      else if (myStrNCmp("-Random",tok[i],2) == 0){
         if (optionRead)
            return errorOption(CMD_OPT_EXTRA,tok[i]);
         optionRead = 'r';
         i++;
         if (!myStr2Int(tok[i],randNum))
            return errorOption(CMD_OPT_ILLEGAL,tok[i]);
         if (randNum <= 0)
            return errorOption(CMD_OPT_ILLEGAL,tok[i]);
      }
      else
         return errorOption(CMD_OPT_ILLEGAL,tok[i]);
   }//end for

   if (optionRead == 'i'){
      if (delArr){
         if (objId >= (int)mtest.getArrListSize()){
            cerr << "Size of array list (" << 
               mtest.getArrListSize() << ") is <= " << objId
               << "!!" << endl;
            return errorOption(CMD_OPT_ILLEGAL,to_string(objId));
         }
         mtest.deleteArr(objId);
      }
      else{
         if (objId >= (int)mtest.getObjListSize()){
            cerr << "Size of object list (" <<
               mtest.getObjListSize() << ") is <= " << objId
               << "!!" << endl;
            return errorOption(CMD_OPT_ILLEGAL,to_string(objId));
         }
         mtest.deleteObj(objId);
      }
   }
   else if (optionRead == 'r'){
      if (delArr){
         if (mtest.getArrListSize() == 0){
            cerr << "Size of array list is 0!!" << endl; 
            for (size_t i=0; i<tok.size(); i++){
               if (myStrNCmp("-Random",tok[i],2) == 0)
                  return errorOption(CMD_OPT_ILLEGAL,tok[i]);
            }
         }
         for (size_t i=0; i<(size_t)randNum; i++)
            mtest.deleteArr(rnGen(mtest.getArrListSize()));
      }
      else{
         if (mtest.getObjListSize() == 0){
            cerr << "Size of object list is 0!!" << endl;
            for (size_t i=0; i<tok.size(); i++){
               if (myStrNCmp("-Random",tok[i],2) == 0)
                  return errorOption(CMD_OPT_ILLEGAL,tok[i]);
            }
         }
         for (size_t i=0; i<(size_t)randNum; i++)
            mtest.deleteObj(rnGen(mtest.getObjListSize()));
      }
   }
   else
      return errorOption(CMD_OPT_MISSING,"");
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


