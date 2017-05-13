#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <time.h>
#include <stdlib.h>

#define BILLION 1000000000

int loop_count;		// loop count
int thread_count;	// thread count

void* thread_func(void *arg);	// thread function

int main(void){			// main function
	struct timespec ts, tp, tc;	// time
	pthread_t *tid;			// process ID
	double p_time=0, c_time=0;	// time
	int i;				// index
	
	// enter ther loop count and thread count
	printf("Loop count:");
	scanf("%d",&loop_count);
	printf("Thread count: ");
	scanf("%d",&thread_count);

	tid=(pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	
	// create thread and finish thread
	for(i=0;i<thread_count;i++){	
		clock_gettime(CLOCK_REALTIME,&ts); // calculate starting time		
		pthread_create(&tid[i],NULL,thread_func,NULL);
		clock_gettime(CLOCK_REALTIME,&tp); // calculate ending time
		p_time+=(tp.tv_sec-ts.tv_sec)+(double)(tp.tv_nsec-ts.tv_nsec)/BILLION;	// calculate process time
		pthread_join(tid[i],NULL);
		clock_gettime(CLOCK_REALTIME,&tc); // calculate ending time
		c_time+=(tc.tv_sec-ts.tv_sec)+(double)(tc.tv_nsec-ts.tv_nsec)/BILLION;	// calculate completed time		
	}
	
	// print result
	printf("Elapsed time after making processes: %f sec\n",p_time);
	printf("Elapsed time to completion: %f sec\n",c_time);

	free(tid);
	return 0;
}
void* thread_func(void *arg){	// thread function
	int sum=0;	// sum	
	int i;		// index
	
	for(i=0;i<loop_count;i++)
		sum+=i;
	
	return NULL;
}
