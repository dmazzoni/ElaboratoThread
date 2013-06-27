/** @file
	Contains functions for initialization and management of condition
	variables and mutexes.
*/

#include <pthread.h>
#include <stdlib.h>
#include "io_utils.h"
#include "sync_utils.h"

/**
	Destroys the specified condition variable.<br>
	Wraps the @c pthread_cond_destroy() function.
	@param cond The condition variable
*/
void cond_destroy(pthread_cond_t *cond) {
	if (pthread_cond_destroy(cond) != 0)
			write_to_fd(2, "Failed to destroy condition variable\n");
}

/**
	Initializes the condition variables in the specified array with the
	default settings.
	@param conds The condition variables array
	@param n_conds The number of condition variables
*/
void conds_init(pthread_cond_t *conds, int n_conds) {
	int i;
	
	for (i = 0; i < n_conds; ++i) {
		if(pthread_cond_init(&conds[i], NULL) != 0) {
			write_with_int(2, "Failed to initialize condition variable ", i);
			exit(1);
		}
	}
}

/**
	Signals the specified condition variable.<br>
	Wraps the @c pthread_cond_signal() function.
	@param cond The condition variable
*/
void cond_signal(pthread_cond_t *cond) {
	if (pthread_cond_signal(cond) != 0) {
		write_to_fd(2, "\tFailed to signal condition\n");
		exit(1);
	}
}

/**
	Waits on the specified condition variable.<br>
	Wraps the @c pthread_cond_wait() function.
	@param cond The condition variable
	@param mutex The mutex that protects the condition variable
*/
void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (pthread_cond_wait(cond, mutex) != 0) {
		write_to_fd(2, "Failed to wait on condition\n");
		exit(1);
	}
}

/**
	Destroys the specified mutex.<br>
	Wraps the @c pthread_mutex_destroy() function.
	@param mutex The mutex
*/
void mutex_destroy(pthread_mutex_t *mutex) {
	if (pthread_mutex_destroy(mutex) != 0)
			write_to_fd(2, "Failed to destroy mutex\n");
}

/**
	Initializes the mutexes in the specified array with the
	default settings.
	@param mutexes The mutexes array
	@param n_mutexes The number of mutexes
*/
void mutexes_init(pthread_mutex_t *mutexes, int n_mutexes) {
	int i;
	
	for (i = 0; i < n_mutexes; ++i) {
		if(pthread_mutex_init(&mutexes[i], NULL) != 0) {
			write_with_int(2, "Failed to initialize mutex ", i);
			exit(1);
		}
	}
	for (i = 1; i < n_mutexes - 1; i += 2) {
		mutex_lock(&mutexes[i]);
	} 
}

/**
	Locks the specified mutex.<br>
	Wraps the @c pthread_mutex_lock() function.
	@param mutex The mutex
*/
void mutex_lock(pthread_mutex_t *mutex) {
	if (pthread_mutex_lock(mutex) != 0) {
		write_to_fd(2, "Failed to lock mutex\n");
		exit(1);
	}
}

/**
	Unlocks the specified mutex.<br>
	Wraps the @c pthread_mutex_unlock() function.
	@param mutex The mutex
*/
void mutex_unlock(pthread_mutex_t *mutex) {
	if (pthread_mutex_unlock(mutex) != 0) {
		write_to_fd(2, "Failed to unlock mutex\n");
		exit(1);
	}
}
