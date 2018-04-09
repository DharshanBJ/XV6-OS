#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{

  /* Fork a new process to play with */
  /* We don't have a good way to list all pids in the system
     so forking a new process works for testing */ 
  int pid = fork();

  if (pid == 0) {
    /* child spins and yields */
    while (1) {
       sleep(5);
    };
  }
 /* parent dumps memory of the child */
	int size=(int)sbrk(0);
	char *buffer=malloc(size);
	char *addr=0;
	int guardPage=dump(pid,addr,buffer,size);
	int *ptr=(int*)buffer;
	int i=0;
	for(i=0;i<size;i+=16)
	{
		if(i==guardPage){
			printf(1,"******************Guard page starts here************************");
			printf(1,"\n");
		}
		if(i==guardPage+4096){
			printf(1,"******************Stack starts here*****************************");
			printf(1,"\n");
		}
		if(i==guardPage+8192){
			printf(1,"******************Heap starts here******************************");
			printf(1,"\n");
		}
		printf(1,"0x%x\t",i);
		printf(1,"0x%x\t",*ptr);
		ptr++;
		printf(1,"0x%x\t",*ptr);
		ptr++;
		 printf(1,"0x%x\t",*ptr);
		ptr++;
 		 printf(1,"0x%x\t",*ptr);
		ptr++;
	       	printf(1,"\n");
		
	}

  exit();
}

