# Contention aware counter

Run tests:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./test

Run genmc (can take a long time):

    $ ./run_genmc.sh


## Proof of linearizability

The counter has two operations: `inc()` and `get()`.
Consider a concurrent history of invocations and responses.
Since operation `inc()` has a linearization point at the fetch-and-add instruction,
we can reorder this history so that every invocation of `inc()` is immediately followed by
the corresponding response.

A `get()` operation will always return a value that is no less than the number
of `inc()` operations completed before the invocation of the `get()` operation.
Also this value cannot be greater than the number of `inc()` operations completed
before the response of the `get()` operation is received. This can be easily seen
from the implementation of these operations.

This means that a `get()` operation that eventually returns `n` has a linearization point
after `n`th `inc()` operation, but before `n+1`th. Linearizing the history like this
results in a valid linearization, since a `get()` operation that started after another
`get()` operation completed cannot return a lesser value.

## Proof of non-linearizability in the presence of `inc(delta)` instruction

Consider the following order of execution:

```
thread 1       | thread 2      | counter
---------------+---------------+---------------
               | get() {       | [0, 0]        
               |   sum += c[0] | [0, 0]        
inc(1)         |               | [1, 0]        
inc(2)         |               | [1, 2]        
               |   sum += c[1] | [1, 2]        
               |   return sum  | [1, 2]        
               | } // sum == 2 | [1, 2]        
```

There is no linearization for this execution history
since in linear order `get()` must return either 0, 1 or 3, but not 2.
