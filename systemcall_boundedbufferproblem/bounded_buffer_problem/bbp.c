#include <stdio.h>
#include <stdlib.h>
#include <linux/sem.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

struct BUFDATA{
	int producer_tid;	// producer's thread ID
	int data_id;		// an integer that is incremented by 1 when produced
	struct BUFDATA *prev;	// previous element in buffer
	struct BUFDATA *next;	// next element in buffer
};

struct BUFDATA* root=NULL;	// root element in buffer
pthread_mutex_t mutex;		// mutex
int g_semid=0;			// semaphore
int P_NUM=0;			// the number of producer
int C_NUM=0;			// the number of consumer
int B_SIZE=0;			// buffer size
int P_COUNT=0;			// count of producer
int C_COUNT=0;			// count of consumer

void* producer(void* arg);	// producer function
void* consumer(void* arg);	// consumer function

int main(int argc, char* argv[]){
	// get the number of producer and consumer, buffer size
	P_NUM=atoi(argv[1]);
	C_NUM=atoi(argv[2]);
	B_SIZE=atoi(argv[3]);
	
	// variables
	int i=0;
	int state;
	union semun sem_union;
	pthread_t p_thread[P_NUM];
	pthread_t c_thread[C_NUM];
	
	// create semaphore
	g_semid=semget((key_t)0x1234,1,0666|IPC_CREAT);
	if(g_semid<0){
		printf("semget failed\n");
		return 1;
	}

	// change the number of semaphore
	sem_union.val=B_SIZE;
	if(semctl(g_semid,0,SETVAL,sem_union)==-1){
		printf("semctl failed\n");
		return 1;
	}
	
	// init mutex
	state=pthread_mutex_init(&mutex,NULL);
	if(state){
		printf("mutex init error\n");
		return 1;
	}
	
	// create thread
	for(i=0;i<P_NUM;++i)
		pthread_create(&p_thread[i],NULL,producer,NULL);
	for(i=0;i<C_NUM;++i)
		pthread_create(&c_thread[i],NULL,consumer,NULL);
	
	// remove thread
	for(i=0;i<P_NUM;++i)
		pthread_join(p_thread[i],NULL);
	for(i=0;i<C_NUM;++i)
		pthread_join(c_thread[i],NULL);
	
	// remove mutex
	pthread_mutex_destroy(&mutex);

	// remove semaphore
	if(semctl(g_semid,0,IPC_RMID,0)==-1){
		printf("semctl failed\n");
		return 1;
	}
	
	return 0;
}
void insert(struct BUFDATA* B){	// insert function
	struct BUFDATA* pB;	// previous BUFDATA

	if(root==NULL){	// empty ( root = B)
		root=B;
		B->next=NULL;
		B->prev=NULL;
	}
	else{		// not empty
		pB=root;
		while(pB->next!=NULL)
			pB=pB->next;
		pB->next=B;
		B->next=NULL;
		B->prev=pB;
	}
}
struct BUFDATA* delete(){		// delete function
	struct BUFDATA* pD=root;	// delete BUFDATA

	if(pD->next==NULL)		// remain root
		root=NULL;
	else{				// make new root
		root=pD->next;
		root->prev=NULL;
	}
	return pD; 
}
int check_full(){		// check whether buffer is full or not
	struct BUFDATA* B=root;	// BUFDATA
	int count=0;		// count

	if(root==NULL)		// empty
		return 1;

	while(B->next!=NULL){	// count producer
		B=B->next;
		count++;
	}

	if(count==B_SIZE)	// full
		return 0;
	else			// not full
		return 1;
}
int check_empty(){		// check whether buffer is empty or not
	if(P_COUNT-C_COUNT==0)	// empty
		return 0;
	else			// not empty
		return 1;	
}
int count_producer(){		// find last buffer in double linked list
	struct BUFDATA* B=root;	// BUFDATA

	if(root==NULL)	// empty
		return 0;
	else{		// not empty (count the number of producer)
		while(B->next!=NULL)
			B=B->next;
		return B->data_id;
	}
}
void* producer(void* arg){	// producer
	unsigned int tid=syscall(SYS_gettid);	// thread ID
	struct BUFDATA* B;			// BUFDATA
	struct sembuf s;			// sembuf
	s.sem_num=0;				// sem_num
	s.sem_flg=SEM_UNDO;			// sem_flg
	
	while(1){
		// P				
		s.sem_op=-1;
		if(semop(g_semid,&s,1)==-1)
			printf("semop fail\n");
		
		// critical section (produce buffer)
		pthread_mutex_lock(&mutex);
		if(check_full()==1){
			P_COUNT++;
			B=(struct BUFDATA*)malloc(sizeof(struct BUFDATA));
			B->producer_tid=tid;
			B->data_id=P_COUNT;
			insert(B);
			printf("[Producer %u] produced %d (buffer state : %d/%d)\n",tid,count_producer(),P_COUNT-C_COUNT,B_SIZE);
		}
		pthread_mutex_unlock(&mutex);
		sleep(2);

		// V
		s.sem_op=1;
		if(semop(g_semid,&s,1)==-1)
			printf("semop fail\n");
	}
	return NULL;
}
void* consumer(void* arg){	// consumer
	unsigned int tid=syscall(SYS_gettid);	// thread ID
	struct BUFDATA* B;			// BUfDATA
	struct sembuf s;			// sembuf
	s.sem_num=0;				// sem_num
	s.sem_flg=SEM_UNDO;			// sem_flg
	
	while(1){
		// P				
		s.sem_op=-1;
		if(semop(g_semid,&s,1)==-1)
			printf("semop fail\n");

		// critical section (consume buffer)
		pthread_mutex_lock(&mutex);
		if(check_empty()==1){
			C_COUNT++;
			B=delete();
			printf("[Consumer %u] consumed %d, produced from %u (buffer state : %d/%d)\n",tid,C_COUNT,B->producer_tid,P_COUNT-C_COUNT,B_SIZE);
			free(B);
		}
		pthread_mutex_unlock(&mutex);
		sleep(2);

		// V
		s.sem_op=1;
		if(semop(g_semid,&s,1)==-1)
			printf("semop fail\n");
	}
	return NULL;
}
