#ifndef ASYNC_H
#define ASYNC_H

typedef void*(*thread_handler)(void*);

/**
 * The main thread_t structure.
 * This structure holds all data needed to handle threads.
 */
struct thread_t;

/**
 * The main mutex_t structure.
 * This structure holds all data needed to handle mutexes.
 */
struct mutex_t;

/**
 * Alocate and initialize a mutex_t structure.
 */
void mutex_t_create(struct mutex_t** mut);

/**
 * Destroy and dealocate a mutex_t structure.
 */
void mutex_t_destroy(struct mutex_t* mut);

/**
 * Lock a mutex_t structure.
 */
void mutex_t_lock(struct mutex_t* mut);

/**
 * Unlock a mutex_t structure.
 */
void mutex_t_unlock(struct mutex_t* mut);

/**
 * Alocate and inidialide a thread_t structure.
 */
void thread_t_create(struct thread_t** thread, thread_handler handler, void* arg);

/**
 * Destroy and dealocate a thread_t structure.
 */
void thread_t_destroy(struct thread_t* thread);

/**
 * Wait for a thread_t structure finish its execution.
 */
void thread_t_join(struct thread_t* thread);

/**
 * Cancel a thread_t structure execution.
 */
void thread_t_cancel(struct thread_t* thread);

/**
 * Returns 1 if the current thread_t in execution is the thread param,
 * and 0 otherwise.
 */
int thread_t_is_current_thread(struct thread_t* thread);

/**
 * Exit the current thread in execution.
 */
void thread_t_exit();

#endif