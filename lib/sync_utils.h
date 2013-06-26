#ifndef MUTEX_UTILS_H
#define MUTEX_UTILS_H

void cond_destroy(pthread_cond_t *cond);
void conds_init(pthread_cond_t *conds, int n_conds);
void cond_signal(pthread_cond_t *cond);
void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
void mutex_destroy(pthread_mutex_t *mutex);
void mutexes_init(pthread_mutex_t *mutexes, int n_mutexes);
void mutex_lock(pthread_mutex_t *mutex);
void mutex_unlock(pthread_mutex_t *mutex);

#endif
