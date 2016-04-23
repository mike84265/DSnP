#include <ctime>
#include <iostream>
#include <cstdio>
using namespace std;
inline int inlineFunc(int x) { return x%10; }
int Func(int x) { return x%10; }
#define macro(x) x%10
int main()
{
   #define ULL unsigned long long
   const ULL TIMES = (ULL)1<<28;
   cout << TIMES << endl;
   cout << sizeof(ULL) << endl;
   cout << sizeof(TIMES) << endl;
   clock_t t = clock();
   int dummy;
   for (ULL i=0; i<TIMES; ++i)
      dummy = inlineFunc(i);
   t = clock() - t;
   printf("Inline function consumes %.5f seconds\n",(float)t/CLOCKS_PER_SEC);
   t = clock();
   for (ULL i=0; i<TIMES; ++i)
      dummy = Func(i);
   t = clock() - t;
   printf("Ordinary function consumes %.5f seconds\n",(float)t/CLOCKS_PER_SEC);
   t = clock();
   for (ULL i=0; i<TIMES; ++i)
      dummy = macro(i);
   t = clock() - t;
   printf("Macro function consumes %.5f seconds\n",(float)t/CLOCKS_PER_SEC);
   t = clock();
   for (ULL i=0; i<TIMES; ++i)
      dummy = i%10;
   t = clock() - t;
   printf("Direct calculation consumes %.5f seconds\n",(float)t/CLOCKS_PER_SEC);

}
