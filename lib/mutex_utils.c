#include <pthread.h>
#include <stdlib.h>
#include "io_utils.h"
#include "mutex_utils.h"

void destroy(pthread_mutex_t *mutex) {
	if (pthread_mutex_destroy(mutex) != 0)
			write_to_fd(2, "Failed to destroy mutex\n");
}

void init_mutexes(pthread_mutex_t *mutexes, int n_mutexes) {
	int i;
	
	for (i = 0; i < n_mutexes; ++i) {
		if(pthread_mutex_init(&mutexes[i], NULL) != 0) {
			write_with_int(2, "Failed to initialize mutex ", i);
			exit(1);
		}
	}
	for (i = 1; i < n_mutexes - 1; i += 2) {
		lock(&mutexes[i]);
	} 
}

void lock(pthread_mutex_t *mutex) {
	if (pthread_mutex_lock(mutex) != 0) {
		write_to_fd(2, "Failed to lock mutex\n");
		exit(1);
	}
}

void unlock(pthread_mutex_t *mutex) {
	if (pthread_mutex_unlock(mutex) != 0) {
		write_to_fd(2, "Failed to unlock mutex\n");
		exit(1);
	}
}
