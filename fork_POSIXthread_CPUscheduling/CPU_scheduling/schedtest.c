#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/sched.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define MAX_PROCESSES 5000
#define BILLION 1000000000

int main(void){	// main function
	struct sched_param param;		// sched_param
	struct timespec ts, tp, tc;		// time
	pid_t pid;				// process ID
	FILE* f_read;				// file	
	char file_name[1024]={0,};		// file name
	char file_context;			// context
	double p_time=0, c_time=0;		// time
	int i, j;				// index
	
	// SCHED_FIFO, SCHED_RR
	param.sched_priority=1;
	// SCHED_IDLE, SCHED_BATCH, SCHED_NORMAL
	//param.sched_priority=0;
	if(sched_setscheduler(0,SCHED_RR,&param)!=0){
		printf("sched_setscheduler failed!\n");
		return 0;
	}

	chdir("./temp");	// change directory
	
	// make process
	for(i=0;i<MAX_PROCESSES;i++){
		clock_gettime(CLOCK_REALTIME,&ts); // calculate starting time
		if((pid=fork())<0)	// fail
			return 1;
		else if(pid==0){	// child process
			for(j=0;j<MAX_PROCESSES;j++){
				memset(file_name,0,1024);
				sprintf(file_name,"%d",j);	// make file name
				if( (f_read=fopen(file_name,"r")) == NULL)	// open file
					printf("File can't open!\n");
				while((file_context=fgetc(f_read))!=EOF)	// read file
					printf("%c",file_context);
				printf("\n");
				fclose(f_read);	// close file
			}
			return 0;	
		}
		else{			// parent process
			clock_gettime(CLOCK_REALTIME,&tp); // calculate ending time
			p_time+=(tp.tv_sec-ts.tv_sec)+(double)(tp.tv_nsec-ts.tv_nsec)/BILLION;	// calculate process time
			wait(NULL);	// wait
			clock_gettime(CLOCK_REALTIME,&tc); // calculate ending time
			c_time+=(tc.tv_sec-ts.tv_sec)+(double)(tc.tv_nsec-ts.tv_nsec)/BILLION;	// calculate completed time
		}
	}
	// print result
	printf("Elapsed time after making processes: %lf sec\n",p_time);
	printf("Elapsed time to completion: %lf sec\n",c_time);
	return 0;
}
