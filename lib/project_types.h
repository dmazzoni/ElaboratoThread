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


typedef struct thread_args {
	int processor_id;
	
	pthread_mutex_t *mutex1;

	pthread_mutex_t *mutex2;

	pthread_mutex_t *cond_mutex;

	operation *oper;

	int *state;

	int *free_count;

	pthread_cond_t *cond;
} thread_args;

#endif
