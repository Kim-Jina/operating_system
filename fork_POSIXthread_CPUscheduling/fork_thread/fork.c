#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#define BILLION 1000000000

int loop_count;	// loop count
int fork_count;	// fork count

void child();	// child function

int main(void){ // main function
	struct timespec ts, tp, tc;	// time
	pid_t pid;			// process ID: 9771
	double p_time=0, c_time=0;	// time
	int i;				// index
	
	// enter ther loop count and thread count
	printf("Loop count: ");
	scanf("%d",&loop_count);
	printf("Fork count: ");
	scanf("%d",&fork_count);
	
	// make process
	for(i=0;i<fork_count;i++){
		clock_gettime(CLOCK_REALTIME,&ts); // calculate starting time
		if((pid=fork())<0)	// fail
			return 1;
		else if(pid==0){	// child process
			child();
			return 0;	
		}
		else{			// parent process
			clock_gettime(CLOCK_REALTIME,&tp); // calculate ending time
			p_time+=(tp.tv_sec-ts.tv_sec)+(double)(tp.tv_nsec-ts.tv_nsec)/BILLION;	// calculate process time
			wait(NULL);
			clock_gettime(CLOCK_REALTIME,&tc); // calculate ending time
			c_time+=(tc.tv_sec-ts.tv_sec)+(double)(tc.tv_nsec-ts.tv_nsec)/BILLION;	// calculate completed time
		}
	}
	
	// print result
	printf("Elapsed time after making processes: %f sec\n",p_time);
	printf("Elapsed time to completion: %f sec\n",c_time);

	return 0;
}
void child(){	// child function
	int sum=0;	// sum
	int i;		// index
	for(i=0;i<loop_count;i++)
		sum+=i;
}
