#include <stdio.h>
int main()
{
   FILE* f;
   f = fopen("do8","w");
   fprintf(f,"cirr ../tests.fraig/sim12.aag\n");
   fprintf(f,"usage\n");
   fprintf(f,"cirp -pi\n");
   fprintf(f,"cirp -po\n");
   fprintf(f,"cirp -fl\n");
   fprintf(f,"cirp -s\n");
   fprintf(f,"cirp -n\n");
   for (int i=0;i<=3588;i++){
      fprintf(f,"cirg %d\n",i);
      fprintf(f,"cirg %d -fanin 2\n",i);
      fprintf(f,"cirg %d -fanout 2\n",i);
   }
   fprintf(f,"usage\n");
   fprintf(f,"cirw\n");
   fprintf(f,"usage\n");
   fprintf(f,"cirw\n");
   fprintf(f,"usage\n");
   for (int i=0;i<5;i++) fprintf(f,"q -f\n");
   fclose(f);
}
