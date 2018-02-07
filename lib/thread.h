// thread.h

#pragma once

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

typedef void *(*thread_func)(void *);

class Thread {
private:
    pthread_t	thread;
public:
    Thread() {}

    void start(thread_func func, void *arg) {
	int rc;
	PTHREAD_CHECK(pthread_create(&thread, NULL, func, arg));
    }

    void join(void **result) {
	int rc;
	PTHREAD_CHECK(pthread_join(thread, result));
    }
};
