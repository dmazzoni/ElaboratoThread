#ifndef MUTEX_UTILS_H
#define MUTEX_UTILS_H

void destroy(pthread_mutex_t *mutex);
void init_mutexes(pthread_mutex_t *mutexes, int n_mutexes);
void lock(pthread_mutex_t *mutex);
void unlock(pthread_mutex_t *mutex);

#endif
