#include "async.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

typedef void*(*thread_handler)(void*);

struct thread_t
{
    pthread_t object;
    thread_handler handler;
    void* arg;
};

struct mutex_t
{
    pthread_mutex_t lock;
};

void mutex_t_create(struct mutex_t** mut)
{
    (*mut) = (struct mutex_t*) malloc(sizeof(struct mutex_t));
    pthread_mutex_init(&(*mut)->lock, NULL);
}

void mutex_t_destroy(struct mutex_t* mut)
{
    pthread_mutex_destroy(&mut->lock);
    free(mut);
}

void mutex_t_lock(struct mutex_t* mut)
{
    pthread_mutex_lock(&mut->lock);
}

void mutex_t_unlock(struct mutex_t* mut)
{
    pthread_mutex_unlock(&mut->lock);
}

void thread_t_create(struct thread_t** thread, thread_handler handler, void* arg) {
    *thread = (struct thread_t*)malloc(sizeof(struct thread_t));
    struct thread_t* t = *thread;


    t->handler = handler;
    t->arg = arg;

    pthread_create(&t->object, NULL, t->handler, t->arg);
}

void thread_t_destroy(struct thread_t* thread)
{
    free(thread);
}

void thread_t_join(struct thread_t* thread) {
    pthread_join(thread->object, NULL);
}

void thread_t_cancel(struct thread_t* thread)
{
    pthread_kill(thread->object, SIGKILL);
}

int thread_t_is_current_thread(struct thread_t* thread) {
    return pthread_self() == thread->object;
}

void thread_t_exit()
{
    pthread_exit(NULL);
}
