/** @file
	Code for the child processor, which is
	started by the main process.<br>
	Each processor is launched passing its ID and the
	total number of semaphores: with this information the
	processor can synchronize on the correct semaphore.<br>
	After attaching the shared memory segments, it loops
	while there are operations to compute, and writes the 
	results in the operations array.
*/

#include <stdlib.h>
#include "io_utils.h"
#include "mutex_utils.h"
#include "project_types.h"

static void compute(operation *oper);

/**
	Computes the operations while they are provided by 
	the main thread and returns the results in the
	corresponding memory locations, with synchronized access.
	@param arguments The thread arguments 
	@see thread_args
*/
void* processor_routine(void *arguments) {
	thread_args *args;
	
	args = (thread_args *) arguments;
	write_with_int(1, "\tProcessor - Started as #", args->processor_id + 1);

	while(1) {
		lock(args->mutexB);
		unlock(args->mutexB);
		lock(args->mutexA);
		if (args->oper->op == 'K')
			break;
		write_with_int(1, "\tOperation received - Processor ", args->processor_id + 1);
		compute(args->oper);
		lock(args->cond_mutex);
		*(args->state) *= -1;
		*(args->free_count)++;
		write_with_int(1, "\tIncremento ", *(args->free_count));
		if(*args->free_count == 1)
			if(pthread_cond_signal(args->cond) != 0)
				write_with_int(2, "\tFailed to signal condition - Processor ", args->processor_id + 1);
		unlock(args->cond_mutex);
		write_with_int(1, "\tResult computed. Unblocking main - Processor ", args->processor_id + 1);
		unlock(args->mutexA);
	}
	
	write_with_int(1, "\tExiting - Processor ", args->processor_id + 1);
	pthread_exit(NULL);
}

/**
	Calculates the operation passed and stores the result in
	the first operand field.
	@param oper The operation to execute
*/
static void compute(operation *oper) {
	switch(oper->op) {
		case '+': oper->num1 = oper->num1 + oper->num2; break;
		case '-': oper->num1 = oper->num1 - oper->num2; break;
		case '*': oper->num1 = oper->num1 * oper->num2; break;
		case '/': if(oper->num2 != 0)
				oper->num1 = oper->num1 / oper->num2; 
			  else {
				write_to_fd(2, "\tDivision by 0\n"); 
				exit(1);
			  }; break;
		default: write_to_fd(2, "\tInvalid operator\n"); 
			 exit(1); 
	}
}
