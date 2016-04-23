#include <stdio.h>
#include <sys/resource.h>
int main()
{
   struct rlimit limit;
   //get resource limit
   getrlimit(RLIMIT_NOFILE, &limit);
   
   printf("Maximum number of open files per process:\n");
   printf("Current value: %d\nMax available value: %d\n",
           limit.rlim_cur, limit.rlim_max);
}
