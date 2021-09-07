#pragma once

#define K 4
#define MEMORY_ORDER memory_order_relaxed

#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

static unsigned my_rand(unsigned *rng) {
    *rng = (214013 * *rng + 2531011);
    return (*rng >> 16) & 0x7FFF;
}

typedef struct {
    atomic_int a[K];
} counter_t;

static void counter_init(counter_t* counter) {
    for (int i = 0; i < K; ++i) {
        atomic_store_explicit(&counter->a[i], 0, MEMORY_ORDER);
    }
}

static void counter_inc_idx(counter_t* counter, int idx) {
    atomic_fetch_add_explicit(&counter->a[idx], 1, MEMORY_ORDER);
}

static void counter_inc(counter_t* counter, unsigned* rng) {
    counter_inc_idx(counter, my_rand(rng) % K);
}

static int counter_get(const counter_t* counter) {
    int sum = 0;
    for (int i = 0; i < K; ++i) {
        sum += atomic_load_explicit(&counter->a[i], MEMORY_ORDER);
    }
    return sum;
}
