Papageno - Algorithmic Performance
==================================

To test the performance of Papageno the following experiment was conducted.

A large number of single note lines was defined. Hereby is

```
n: The number of different inputs
t: The length of the pattern
```

All possible permutations of note lines were defined,

```
n_lines = n^t
```

in total.

The number of nodes in the resulting tree can be computed as

```
n_nodes = n*(n^t - 1)/(n - 1)
```

which results from the definition of the finite geometric series.

We conducted two types of experiments, that pass or fail. The reason is that we want to determine the best case and the worse case behavior of the search engine.

Successful Search
-----------------

To test the performance of passing test, i.e. event series that lead to a match, numerical experiments were conducted as follows. An event series was defined that activates the highest possible input t times. From an algorithmic point of view, this is still a partial worse case as due to the definition order of patterns, the note that corresponds to the last input is always checked last. This leads to n-1 failing tests on each level of the tree during pattern matching.

For every experiment, the number of tokens that were checked against an event was counted. The results are as follows:

```
            Number of Different Inputs
Tree Depth |    2 |   3 |   4 |   5 |   6 |   7 |   8 |
-------------------------------------------------------
         1 |    2 |   3 |   4 |   5 |   6 |   7 |   8 |   
         2 |    4 |   6 |   8 |  10 |  12 |  14 |  16 |   
         3 |    6 |   9 |  12 |  15 |  18 |  21 |  24 |   
         4 |    8 |  12 |  16 |  20 |  24 |  28 |  32 |  
         5 |   10 |  15 |  20 |  25 |  30 |  35 |  40 |  
         6 |   12 |  18 |  24 |  30 |  36 |  42 |  48 |  
         7 |   14 |  21 |  28 |  35 |  42 |  49 |  56 |  
         8 |   16 |  24 |  32 |  40 |  48 |  56 |  64 |
```

The formula to compute the amount of node checks is

```
n_checks = n*t .
```

This is because we used note-tokens that only expect an input activation. If we would have used standard notes that expect activation and deactivation and passed a event sequence where an activation of an input is always followed by a deactivation, the amount of checks would be

```
n_checks_s = 2*n*t - (n - 1) .
```

As we can see, the complexity is linear with respect to both, the number of inputs and the length of the node lines.

Failing Search
--------------

A similar test was conducted for tests that must fail. The only difference to the event series that was used before is that the last input in the event series is selected in a way that it is not part of the input set that is used to construct the patterns. Thus, every test must fail.

If a search fails the search engine drops the first event and starts over with an event queue that is by one event shorter than before. This is done repeatedly if the consecutive pattern searches fail, until the event queue is empty.

Again the number of token checks per experiment is listed.

```
            Number of Different Inputs
Tree Depth |    2 |   3 |   4 |   5 |   6 |   7 |   8 |
----------------------------------------
         1 |    2 |   3 |   4 |   5 |   6 |   7 |   8 |  
         2 |    6 |   9 |  12 |  15 |  18 |  21 |  24 |  
         3 |   12 |  18 |  24 |  30 |  36 |  42 |  48 |  
         4 |   20 |  30 |  40 |  50 |  60 |  70 |  80 |  
         5 |   30 |  45 |  60 |  75 |  90 | 105 | 120 |
         6 |   42 |  63 |  84 | 105 | 126 | 147 | 168 |
         7 |   56 |  84 | 112 | 140 | 168 | 196 | 224 |
         8 |   72 | 108 | 144 | 180 | 216 | 252 | 288 |
```

The resulting complexity is

```
n_checks = n*t*(t + 1)/2 ,
```

which is still linear with the number of inputs but quadratic with the pattern length.

Thus, the overall complexity with respect to pattern length is linear in the optimal case and quadratic as worse case.
