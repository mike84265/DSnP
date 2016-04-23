#include <stdio.h>
int main()
{
   FILE* f;
   f = fopen("do12","w");
   fprintf(f,"cirr ../tests.fraig/sim12.aag\n");
   fprintf(f,"usage\n");
   fprintf(f,"cirstr\n");
   fprintf(f,"ciropt\n");
   fprintf(f,"usage\n");
   fprintf(f,"cirsim -f ../tests.fraig/pattern.12\n");
   fprintf(f,"usage\n");
//   fprintf(f,"cirp -pi\n");
//   fprintf(f,"cirp -po\n");
//   fprintf(f,"cirp -fl\n");
//   fprintf(f,"cirp -s\n");
//   fprintf(f,"cirp -n\n");
   for (int i=0;i<=9642;i++){
      fprintf(f,"cirg %d\n",i);
      fprintf(f,"cirg %d -fanin 2\n",i);
      fprintf(f,"cirg %d -fanout 2\n",i);
   }
   fprintf(f,"usage\n");
   for (int i=0;i<5;i++) fprintf(f,"q -f\n");
   fclose(f);
}
