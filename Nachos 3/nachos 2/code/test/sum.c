#include "syscall.h"

int main() {

   int  arr[10] = {1,2,3,4,5,6,7,8,9,10};
   int i,sum=0;
   for (i=0;i<10;i++) {
       sum+=arr[i];
   }
   printf("Summation of array = %d\n",sum);
   Exit(-1);
}
