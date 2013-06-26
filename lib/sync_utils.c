#include <pthread.h>
#include <stdlib.h>
#include "io_utils.h"
#include "sync_utils.h"

void cond_destroy(pthread_cond_t *cond) {
	if (pthread_cond_destroy(cond) != 0)
			write_to_fd(2, "Failed to destroy condition variable\n");
}

void conds_init(pthread_cond_t *conds, int n_conds) {
	int i;
	
	for (i = 0; i < n_conds; ++i) {
		if(pthread_cond_init(&conds[i], NULL) != 0) {
			write_with_int(2, "Failed to initialize condition variable ", i);
			exit(1);
		}
	}
}

void cond_signal(pthread_cond_t *cond) {
	if (pthread_cond_signal(cond) != 0) {
		write_to_fd(2, "\tFailed to signal condition\n");
		exit(1);
	}
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (pthread_cond_wait(cond, mutex) != 0) {
		write_to_fd(2, "Failed to wait on condition\n");
		exit(1);
	}
}

void mutex_destroy(pthread_mutex_t *mutex) {
	if (pthread_mutex_destroy(mutex) != 0)
			write_to_fd(2, "Failed to destroy mutex\n");
}

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

void mutex_lock(pthread_mutex_t *mutex) {
	if (pthread_mutex_lock(mutex) != 0) {
		write_to_fd(2, "Failed to lock mutex\n");
		exit(1);
	}
}

void mutex_unlock(pthread_mutex_t *mutex) {
	if (pthread_mutex_unlock(mutex) != 0) {
		write_to_fd(2, "Failed to unlock mutex\n");
		exit(1);
	}
}
