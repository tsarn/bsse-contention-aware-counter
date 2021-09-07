#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ITERATIONS 8
#define N_THREADS 2

typedef struct {
    counter_t* counter;
    unsigned seed;
} context_t;

atomic_int expected_counter_atomic = 0;

void* worker(void* arg) {
    context_t* ctx = arg;
    unsigned rng = ctx->seed;

    for (int i = 0; i < ITERATIONS; ++i) {
        long action = my_rand(&rng);
        if (action & 1) {
            atomic_fetch_add_explicit(&expected_counter_atomic, 1, memory_order_seq_cst);
            counter_inc(ctx->counter, &rng);
        } else {
            int expected = atomic_load_explicit(&expected_counter_atomic, memory_order_seq_cst);
            int cur = counter_get(ctx->counter);
            if (cur != expected) {
                printf("FAIL, expected counter = %d, got %d\n", expected, cur);
                abort();
            }
        }
    }

    return NULL;
}

void run_test(int n_threads) {
    expected_counter_atomic = 0;
    counter_t counter;
    counter_init(&counter);

    pthread_t threads[n_threads];
    context_t ctx[n_threads];

    for (int i = 0; i < n_threads; ++i) {
        ctx[i].counter = &counter;
        ctx[i].seed = i + n_threads * N_THREADS;
        pthread_create(&threads[i], NULL, worker, &ctx[i]);
    }

    for (int i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    int real_counter = counter_get(&counter);
    int expected_counter = atomic_load(&expected_counter_atomic);

    if (real_counter != expected_counter) {
        printf("FAIL, expected final counter = %d, got %d\n", expected_counter, real_counter);
        abort();
    }
}

int main() {
    for (int i = 1; i <= N_THREADS; ++i) {
        run_test(i);
    }
    return 0;
}
