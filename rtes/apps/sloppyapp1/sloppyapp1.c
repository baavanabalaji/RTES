#include <stdio.h>
#include <unistd.h>

#define syscall_number 1
void main()
{
   FILE *fptr;
   fptr = fopen("program.txt","w");

   if(fptr == NULL)
   {
      printf("Error!");   
     syscall(syscall_number,1);
   //   sys_exit(1);             
   }
    printf("pid: %ld\n",getpid());

   fprintf(fptr,"sloppy hello");
   //fclose(fptr);
    
   syscall(syscall_number,0);
   //sys_exit(0);
}


