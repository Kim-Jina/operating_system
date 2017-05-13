#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PROCESSES 5000

int main(void){	// main function
	FILE *f_write;				// file
	char file_name[1024]={};	// file name	
	int i;					// index

	chdir("./temp");	// change directory
	for(i=0;i<MAX_PROCESSES;i++){
		sprintf(file_name,"%d",i);	// make file name
		f_write=fopen(file_name,"w");	// open file
		fprintf(f_write,"%d",(int)1E9+(int)rand()%(int)1E9);	// write date
		fclose(f_write);	// close file
	}
	return 0;
}
