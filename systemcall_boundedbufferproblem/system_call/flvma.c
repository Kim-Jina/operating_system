#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <linux/slab.h>

#define __NR_ftrace 351

void **syscall_table=(void**)SYSCALL_TABLE;	// syscall table

asmlinkage pid_t (*real_ftrace)(pid_t);
asmlinkage pid_t ftrace(pid_t pid){		// ftrace pointer function
	struct mm_struct* mm=get_task_mm(current);	// mm_struct
	struct vm_area_struct *cur_vm=mm->mmap;		// virtual memory area
	struct file* c_file;				// file on virtual memory
	char buf[1024];					// buf 1D array
	char* path;					// path
	
	printk(" ######### Loaded files of a process '%s(%d)' in VM #########\n",current->comm,pid);
	while(cur_vm){
		c_file=cur_vm->vm_file;	// get file
		if(c_file){		// if file exists
			path=d_path(&c_file->f_path,buf,1024);					// get absolute path of file
			printk("(0x%lx~0x%lx) %s\n",cur_vm->vm_start,cur_vm->vm_end,path);	// print information
		}
		cur_vm=cur_vm->vm_next; // move to next VM
	}
	printk("##############################$$##############################\n");
	return pid;
}
void make_rw(void* address){	// make the page writable
	unsigned int level;
	pte_t *pte=lookup_address((unsigned long)address,&level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
}
void make_ro(void *address){	// make the page write protected
	unsigned int level;
	pte_t *pte=lookup_address((unsigned long)address,&level);
	pte->pte=pte->pte &~ _PAGE_RW;
}
int hooking_init(void){
	make_rw(syscall_table);
	real_ftrace=syscall_table[__NR_ftrace];
	syscall_table[__NR_ftrace] = ftrace;
	return 0;
}
void hooking_exit(void){
	syscall_table[__NR_ftrace] = real_ftrace;
	make_ro(syscall_table);
}
module_init(hooking_init);
module_exit(hooking_exit);
MODULE_LICENSE("GPL");
