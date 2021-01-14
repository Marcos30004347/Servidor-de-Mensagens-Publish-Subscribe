#ifndef NETWORK_ASYNC_H
#define NETWORK_ASYNC_H

/**
 * @brief The handler that will run inside a thread_t object.
 */
typedef void*(*thread_handler)(void*);

/**
 * @brief The thread_t structure.
 * 
 */
struct thread_t;

/**
 * @brief The main mutex_t structure.
 */
struct mutex_t;

/**
 * @brief Alocate and initialize a mutex_t structure.
 * 
 * @param mut The mutex that will be initialized.
 */
void mutex_t_create(struct mutex_t** mut);

/**
 *  @brief Destroy and dealocate a mutex_t structure.
 * 
 *  @param mut The mutex that will be destroyed.
 */
void mutex_t_destroy(struct mutex_t* mut);

/**
 * @brief Locks the mutex. Other threads will block at this function
 * while the thread that had first locked the mutex dont call the
 * mutex_t_unlock function.
 * 
 * @param mut The mutex that should be locked.
 */
void mutex_t_lock(struct mutex_t* mut);

/**
 * @brief Unlocks a mutex allowing another thread to
 * continue its execution if it was blocked by the
 * mutex.
 * 
 * @param mut The mutex that should be unlocked.
 */
void mutex_t_unlock(struct mutex_t* mut);

/**
 * @brief Alocate and inidialide a thread_t structure.
 * 
 * @param thread The thread that should be allocated.
 * @param handler The thread handler.
 * @param arg The argument that should be fowarded to the handler.
 */
void thread_t_create(struct thread_t** thread, thread_handler handler, void* arg);

/**
 * @brief Destroy and dealocate a thread_t structure.
 * 
 * @param thread The thread that should be destroyed.
 */
void thread_t_destroy(struct thread_t* thread);

/**
 * @brief Waits for a thread to finish its execution.
 * 
 * @param thread The thread that should be waited.
 */
void thread_t_join(struct thread_t* thread);


#endif