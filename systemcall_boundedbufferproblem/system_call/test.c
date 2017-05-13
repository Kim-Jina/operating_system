#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>

int main(void){
	syscall(__NR_ftrace,getpid());
	return 0;
}
