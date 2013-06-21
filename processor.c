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
#include <sys/types.h>
#include "io_utils.h"
#include "operation.h"

static void compute(operation *oper);

/**
	Computes the operations while they are provided by 
	the main thread and returns the results in the
	corresponding memory locations, with synchronized access.
	@param argc The number of arguments
	@param argv The array of arguments
*/
int main(int argc, char *argv[]) {
	int proc_id, nsems;
	int *states;
	operation *operations;
		
	proc_id = atoi(argv[1]);
	write_with_int(1, "\tProcessor - Started as #", proc_id + 1);

	while(1) {
		// Lock M[proc_id]
		if (operations[proc_id].op == 'K')
			break;
		write_with_int(1, "\tOperation received - Processor ", proc_id + 1);
		compute(operations + proc_id);
		// Lock area stati
		states[proc_id] *= -1;
		// Unlock area stati
		write_with_int(1, "\tResult computed. Unblocking main - Processor ", proc_id + 1);
		// Unlock M[proc_id]
		// Incremento contatore (protetto da lock)
	}
	
	write_with_int(1, "\tExiting - Processor ", proc_id + 1);
	// pthread_exit
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
