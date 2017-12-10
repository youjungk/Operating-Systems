#include "array_stats.h"
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define _ARRAY_STATS_ 331

//function def
asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size);

//function implementation
asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size) {
    struct array_stats *temp_res = NULL;
    long *copied_data = NULL;
    //variables for loops
    long i = 0;
    long j = 0;
    long l =0;
    long temp;
    long sum = 0;

    if (size <= 0) {
        return -EINVAL;
    }

//If the kmalloc() call succeeds, ptr now points to a block of memory that is at least the requested size. 
//The GFP_KERNEL flag specifies the behavior of the memory allocator while trying to obtain the memory to return to the caller of kmalloc().
    copied_data = kmalloc(sizeof (long) * size, GFP_KERNEL);
    while (i<size){
        if (copy_from_user(&copied_data[i], &data[i], sizeof (long))) {
            kfree(copied_data);
            return -EFAULT;
        }
        i++;
    }

//Sorting the data using insertion sort
    for (int k = 0; k < size; ++k) {
        j = k;
        while (j > 0 && copied_data[j] < copied_data[j - 1]) {
            temp = copied_data[j];
            copied_data[j] = copied_data[j - 1];
            copied_data[j - 1] = temp;
            --j;
        }
    }

//Calculate the sum
    while(l<size){
        sum += copied_data[l];
        l++;
    }

//Assigning sorted value into the temporary result //first value is the smallest value //last element is the largest value
    temp_res = kmalloc(sizeof (struct array_stats), GFP_KERNEL);
    temp_res->min = copied_data[0];
    temp_res->max = copied_data[size - 1];
    temp_res->sum = sum;

    if (copy_to_user(stats, temp_res, sizeof (struct array_stats))) {
        kfree(copied_data);
        kfree(temp_res);
        return -EFAULT;
    }

//Finally free up the memory that was used
    kfree(copied_data);
    kfree(temp_res);

    return 0;
}
