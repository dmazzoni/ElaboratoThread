/** @file
	Defines the types shared by the main thread and the processors.
*/

#ifndef PROJECT_TYPES_H
#define PROJECT_TYPES_H

#include <pthread.h>

/// Used by the main process to send operations to processors
typedef struct operation {
	/// The first operand, also used to store the result
	int num1;
	
	/// The operator, also used to pass the termination command
	char op;
	
	/// The second operand
	int num2;
} operation;

/// Used to pass arguments to processor threads
typedef struct thread_args {
	/// The identification number of the processor
	int processor_id;
	
	/// The first mutex for synchronization on operation @c oper
	pthread_mutex_t *mutexA;

	/// The second mutex for synchronization on operation @c oper
	pthread_mutex_t *mutexB;

	/// The mutex for the condition variable
	pthread_mutex_t *cond_mutex;

	/// The operation to compute
	operation *oper;

	/// The pointer to the processor state
	int *state;

	/// The pointer to the free threads counter
	int *free_count;

	/// The condition variable
	pthread_cond_t *cond;
} thread_args;

#endif
