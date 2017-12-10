#include "process_ancestors.h"
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

#define __PROCESS_ANCESTORS_ 332
#define ANCESTOR_NAME_LEN 16

//function def
asmlinkage long sys_process_ancestors(struct process_info info_array[], long size, long *num_filled);
struct process_info process_info_getter(struct task_struct* curr_task);

struct process_info process_info_getter(struct task_struct* curr_task){
	struct process_info result;
	int num_children, num_sibling;

	struct list_head* ptr;

	result.pid = curr_task->pid;
	result.state = curr_task->state;
	result.uid = curr_task->cred->uid.val;
	result.nvcsw = curr_task->nvcsw;
	result.nivcsw = curr_task->nivcsw;
	// name is in .comm and can be copied using memcpy function
	memcpy(result.name, curr_task->comm, ANCESTOR_NAME_LEN);

//list_for_each used to traverse
//list_for_each(p, list) {
        /* p points to an entry in the list */
//}
	list_for_each(ptr, &(curr_task->children)){
		++num_children;
	}
	list_for_each(ptr, &(curr_task->sibling)){
		++num_sibling;
	}

	result.num_children = num_children;
	result.num_sibling = num_sibling;

	return result;
}

asmlinkage long sys_process_ancestors(struct process_info info_array[], long size, long *num_filled) {
	long curr_num_filled = 0;
	struct process_info* temp_info_array;
	struct task_struct* curr_process, prev_process;

	if (size <=0){return -EINVAL;}
	temp_info_array = kmalloc(sizeof(struct process_info)*size, GFP_KERNEL);

	for (int i=0;i<size;i++){
		if(copy_from_user(&temp_info_array[i],&info_array[i], sizeof(struct process_info))){
			kfree(temp_info_array);
			return -EFAULT;
		}
	}

	//latest children
	temp_info_array[0] = get_process_info(current);
	curr_num_filled++;
	int j=1;
	while (j<size && curr_process != prev_process){
		temp_info_array[j] = process_info_getter(curr_process);
		curr_num_filled++;
		prev_process = curr_process;
		curr_process = curr_process->parent;
		j++
	}

	for (int k=0; k<num_filled;k++){
		if (copy_to_user(&info_array[k], &temp_info_array[k], sizeof(struct process_info))){
			kfree(temp_info_array);
			return -EFAULT;
		}
	}

	if (copy_to_user(num_filled, &curr_num_filled, sizeof(long))){
		return -EFAULT;
	}

	kfree(temp_info_array);
	return 0;
}