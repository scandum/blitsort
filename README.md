Origin
------
Blitsort is an in-place rotate quick/merge sort based on the stable out-of-place merge sort [quadsort](https://github.com/scandum/quadsort), stable out-of-place quicksort [fluxsort](https://github.com/scandum/fluxsort), and unstable in-place [crumsort](https://github.com/scandum/crumsort). This page primarily focuses on the in-place stable aspects of blitsort, the quadsort and fluxsort pages go into greater detail about mergesort and stable quicksort respectively.

Visualization
-------------
In the visualization below the upper half shows the swap memory (32 elements) and the bottom half shows the main memory (512 elements). Colors are used to differentiate between various operations. Partitioning is in light and dark blue, rotations are in yellow and violet.

[![blitsort benchmark](/images/blitsort.gif)](https://www.youtube.com/watch?v=WaqkBO_nV7k)

The [blitsort visualization](https://www.youtube.com/watch?v=WaqkBO_nV7k) only shows the sorting of random data, the sorting of ordered data is identical to the [quadsort visualization](https://www.youtube.com/watch?v=GJjH_99BS70) on the quadsort github page.

Analyzer
--------
Blitsort uses the same analyzer as fluxsort. The distribution is scanned to determine how much of it is already sorted. Distributions that are fully in order or in reverse-order are handled immediately. Segments that are mostly sorted are sorted with rotate quadsort, otherwise they're sorted with rotate fluxsort.

Rotate merge sort
-----------------
A rotate merge sort uses rotations to partition two sorted arrays until they're small enough to be merged using auxiliary memory. Blitsort does so by taking the center element of the first array, using a binary search to find all elements smaller than the center element in the second array, and performing an array rotation. It does so recursively until a partition becomes small enough to be merged.

Rotate stable quicksort
-----------------------
A stable rotate quicksort partitions the array in segments using auxiliary memory, the left and right partition of each segment is then moved to the proper location recursively using rotations. On random data it's faster than a rotate mergesort since no binary search is required and because the branchless optimizations are slightly more efficient. 

Monobound binary search
-----------------------
Blitsort uses a [monobound binary search](https://github.com/scandum/binary_search), which is up to two times faster than the binary search in general use.

Trinity rotation
----------------
Blitsort uses a [trinity rotation](https://github.com/scandum/rotate), a new and significantly faster array rotation algorithm.

Memory
------
By default blitsort uses 512 elements worth of stack memory.

The minimum memory requirement for blitsort is 32 elements of stack memory, it can be configured to use sqrt(n) memory.

Blitsort partitions recursively, requiring an additional log(n) memory. It's possible to make this O(1) through the implementation of a stack, which makes the rotate quick/merge sort algorithm in-place from a theoretical perspective.

There is currently no clear consensus on what constitutes as an in-place sort, it boils down to what someone considers a small enough memory footprint to be considered negligable. This typically ranges from the size of a cache line to the size of the L1 cache.

Performance
-----------
Blitsort has exceptional performance due to the use of trinity / bridge rotations. It is likely the fastest in-place stable sort written so far and is about 15-50% faster than [octosort](https://github.com/scandum/octosort), which is a block merge sort.

Blitsort's performance is similar to that of fluxsort as long as it has sqrt(n) auxiliary memory. Performance on larger arrays degrades steadily but will still beat most traditional sorts at 10 million elements.

To take full advantage of branchless operations the `cmp` macro needs to be uncommented in bench.c, which will double the performance on primitive types. The blitsort_prim function can be used to access primitive comparisons directly for 32 and 64 bit integers.

Data Types
----------
Blitsort supports long doubles and 8, 16, 32, and 64 bit data types. By using 32 or 64 bit pointers it's possible to sort any other data type, like strings. Custom data sizes can be added in blitsort.h. For any practical use where stability matters you'll likely be using pointers however.

Interface
---------
The interface is the same one as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Big O
-----
```
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│blitsort       ││n      │n log n│n log n││1      │1      │1      ││yes   ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│crumsort       ││n      │n log n│n log n││1      │1      │1      ││no    ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│fluxsort       ││n      │n log n│n log n││1      │n      │n      ││yes   ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```

Benchmark: blitsort vs std::stable_sort vs gfx::timsort
-------------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort). The source code
was compiled using `g++ -O3 -w -fpermissive bench.c`. Stablesort is g++'s std:stable_sort function.

The graph shows the relative performance on 100,000 32 bit integers. A table with the best and
average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |  128 | 0.010913 | 0.011047 |         0 |     100 |     random order |
|  blitsort |   100000 |  128 | 0.010880 | 0.011423 |         0 |     100 |     random order |
|   timsort |   100000 |  128 | 0.012791 | 0.012901 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   64 | 0.006125 | 0.006207 |         0 |     100 |     random order |
|  blitsort |   100000 |   64 | 0.002397 | 0.002518 |         0 |     100 |     random order |
|   timsort |   100000 |   64 | 0.007787 | 0.007855 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.005991 | 0.006025 |         0 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.002245 | 0.002341 |         0 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007620 | 0.007656 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003815 | 0.003838 |         0 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.001017 | 0.001085 |         0 |     100 |     random % 100 |
|   timsort |   100000 |   32 | 0.005641 | 0.005674 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000673 | 0.000703 |         0 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000044 | 0.000044 |         0 |     100 |  ascending order |
|   timsort |   100000 |   32 | 0.000045 | 0.000045 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001363 | 0.001399 |         0 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.000628 | 0.000636 |         0 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000844 | 0.000849 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000810 | 0.000840 |         0 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000358 | 0.000363 |         0 |     100 |       pipe organ |
|   timsort |   100000 |   32 | 0.000169 | 0.000172 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000905 | 0.000921 |         0 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000055 | 0.000055 |         0 |     100 | descending order |
|   timsort |   100000 |   32 | 0.000088 | 0.000092 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001361 | 0.001402 |         0 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000637 | 0.000641 |         0 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000842 | 0.000850 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002033 | 0.002090 |         0 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.000956 | 0.000961 |         0 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.001999 | 0.002019 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003493 | 0.003541 |         0 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.001396 | 0.001404 |         0 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004033 | 0.004059 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000921 | 0.000948 |         0 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.001383 | 0.001397 |         0 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000836 | 0.000880 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001171 | 0.001355 |         0 |     100 |     bit reversal |
|  blitsort |   100000 |   32 | 0.002052 | 0.002160 |         0 |     100 |     bit reversal |
|   timsort |   100000 |   32 | 0.002250 | 0.002832 |         0 |     100 |     bit reversal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. It measures the performance
on random data with array sizes ranging from 10 to 10,000,000. It's generated by running the benchmark
using 1024 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions halves
each time the number of items doubles. A table with the best and average time in seconds can be
uncollapsed below the bar graph.

![Graph](/images/graph2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |       10 |   32 | 0.126263 | 0.126498 |       0.0 |      10 |        random 10 |
|  blitsort |       10 |   32 | 0.050169 | 0.050964 |       0.0 |      10 |        random 10 |
|   timsort |       10 |   32 | 0.140403 | 0.142217 |       0.0 |      10 |        random 10 |
|           |          |      |          |          |           |         |                  |
|stablesort |      100 |   32 | 0.245318 | 0.245592 |       0.0 |      10 |       random 100 |
|  blitsort |      100 |   32 | 0.113215 | 0.113717 |       0.0 |      10 |       random 100 |
|   timsort |      100 |   32 | 0.344290 | 0.344927 |       0.0 |      10 |       random 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |     1000 |   32 | 0.362981 | 0.363514 |       0.0 |      10 |      random 1000 |
|  blitsort |     1000 |   32 | 0.141162 | 0.141663 |       0.0 |      10 |      random 1000 |
|   timsort |     1000 |   32 | 0.497717 | 0.497871 |       0.0 |      10 |      random 1000 |
|           |          |      |          |          |           |         |                  |
|stablesort |    10000 |   32 | 0.476746 | 0.477406 |       0.0 |      10 |     random 10000 |
|  blitsort |    10000 |   32 | 0.175833 | 0.176154 |       0.0 |      10 |     random 10000 |
|   timsort |    10000 |   32 | 0.639670 | 0.640221 |       0.0 |      10 |     random 10000 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.601740 | 0.602295 |       0.0 |      10 |    random 100000 |
|  blitsort |   100000 |   32 | 0.233671 | 0.234503 |       0.0 |      10 |    random 100000 |
|   timsort |   100000 |   32 | 0.765775 | 0.766347 |       0.0 |      10 |    random 100000 |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.730429 | 0.732132 |       0.0 |      10 |   random 1000000 |
|  blitsort |  1000000 |   32 | 0.313603 | 0.316055 |       0.0 |      10 |   random 1000000 |
|   timsort |  1000000 |   32 | 0.898629 | 0.900761 |       0.0 |      10 |   random 1000000 |
|           |          |      |          |          |           |         |                  |
|stablesort | 10000000 |   32 | 0.889812 | 0.894025 |         0 |      10 |  random 10000000 |
|  blitsort | 10000000 |   32 | 0.425343 | 0.433828 |         0 |      10 |  random 10000000 |
|   timsort | 10000000 |   32 | 1.083189 | 1.089713 |         0 |      10 |  random 10000000 |

</details>

Benchmark: blitsort vs qsort (mergesort) vs quadsort
----------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. The graph shows the relative performance on
100,000 32 bit integers. A table with the best and average time in seconds can be uncollapsed
below the bar graph.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.016679 | 0.016874 |   1536381 |     100 |    random string |
|  blitsort |   100000 |   64 | 0.010601 | 0.010905 |   1782500 |     100 |    random string |
|  quadsort |   100000 |   64 | 0.010759 | 0.010926 |   1684673 |     100 |    random string |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   64 | 0.014511 | 0.014668 |   1536491 |     100 |    random double |
|  blitsort |   100000 |   64 | 0.008066 | 0.008297 |   1781674 |     100 |    random double |
|  quadsort |   100000 |   64 | 0.008569 | 0.008713 |   1684633 |     100 |    random double |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   64 | 0.011017 | 0.011179 |   1536491 |     100 |      random long |
|  blitsort |   100000 |   64 | 0.005432 | 0.005603 |   1781674 |     100 |      random long |
|  quadsort |   100000 |   64 | 0.006017 | 0.006109 |   1684633 |     100 |      random long |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   64 | 0.010713 | 0.010870 |   1536634 |     100 |       random int |
|  blitsort |   100000 |   64 | 0.005227 | 0.005355 |   1790104 |     100 |       random int |
|  quadsort |   100000 |   64 | 0.005368 | 0.005409 |   1684734 |     100 |       random int |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.019064 | 0.019920 |   1536491 |     100 |     random order |
|  blitsort |   100000 |  128 | 0.014491 | 0.014905 |   1781674 |     100 |     random order |
|  quadsort |   100000 |  128 | 0.011836 | 0.011920 |   1684633 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.009403 | 0.009521 |   1536491 |     100 |     random order |
|  blitsort |   100000 |   64 | 0.004597 | 0.004787 |   1781674 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.004097 | 0.004135 |   1684633 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008948 | 0.009054 |   1536634 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.003867 | 0.004006 |   1790104 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.003365 | 0.003412 |   1684734 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006890 | 0.007088 |   1532465 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.001746 | 0.001816 |    897246 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.002744 | 0.002825 |   1415417 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002610 | 0.002730 |    815024 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000165 | 0.000166 |     99999 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000159 | 0.000161 |     99999 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003396 | 0.003577 |    915020 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.000886 | 0.000896 |    322647 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000891 | 0.000905 |    379624 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002663 | 0.002709 |    884462 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000550 | 0.000555 |    234795 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000461 | 0.000464 |    277113 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002456 | 0.002586 |    853904 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000176 | 0.000179 |     99999 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000163 | 0.000180 |     99999 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003287 | 0.003400 |    953892 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000908 | 0.000922 |    322881 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000913 | 0.000924 |    391547 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004232 | 0.004340 |   1012073 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.001514 | 0.001528 |    613378 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.001190 | 0.001209 |    592061 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005963 | 0.006163 |   1200713 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.002331 | 0.002363 |   1040201 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.002046 | 0.002066 |   1006728 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004359 | 0.004624 |   1209200 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.001330 | 0.001353 |    371638 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.002179 | 0.002243 |    671244 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005982 | 0.006376 |   1553378 |     100 |     bit reversal |
|  blitsort |   100000 |   32 | 0.003649 | 0.003825 |   1795037 |     100 |     bit reversal |
|  quadsort |   100000 |   32 | 0.003180 | 0.003253 |   1727134 |     100 |     bit reversal |

</details>

Benchmark: blitsort vs pdqsort vs crumsort
------------------------------------------
The following benchmark was on WSL gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04) using the [wolfsort](https://github.com/scandum/wolfsort) benchmark.
The source code was compiled using g++ -O3 -w -fpermissive bench.c. The bar graph shows the best run out of 100 on 100,000 32 bit integers. Comparisons for pdqsort, blitsort, and crumsort are inlined.

![Graph](/images/graph5.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |  128 | 0.005953 | 0.006105 |         0 |     100 |     random order |
|  blitsort |   100000 |  128 | 0.010987 | 0.011649 |         0 |     100 |     random order |
|  crumsort |   100000 |  128 | 0.008277 | 0.008499 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |   64 | 0.002646 | 0.002694 |         0 |     100 |     random order |
|  blitsort |   100000 |   64 | 0.002382 | 0.002515 |         0 |     100 |     random order |
|  crumsort |   100000 |   64 | 0.001894 | 0.001904 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |   32 | 0.002683 | 0.002720 |         0 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.002249 | 0.002377 |         0 |     100 |     random order |
|  crumsort |   100000 |   32 | 0.001795 | 0.001825 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000802 | 0.000807 |         0 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.001018 | 0.001078 |         0 |     100 |     random % 100 |
|  crumsort |   100000 |   32 | 0.000560 | 0.000563 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000099 | 0.000099 |         0 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000044 | 0.000044 |         0 |     100 |  ascending order |
|  crumsort |   100000 |   32 | 0.000044 | 0.000044 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.003459 | 0.003501 |         0 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.000627 | 0.000632 |         0 |     100 |    ascending saw |
|  crumsort |   100000 |   32 | 0.000628 | 0.000633 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002827 | 0.002863 |         0 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000358 | 0.000362 |         0 |     100 |       pipe organ |
|  crumsort |   100000 |   32 | 0.000358 | 0.000360 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000203 | 0.000204 |         0 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000055 | 0.000055 |         0 |     100 | descending order |
|  crumsort |   100000 |   32 | 0.000056 | 0.000056 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.003232 | 0.003262 |         0 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000637 | 0.000641 |         0 |     100 |   descending saw |
|  crumsort |   100000 |   32 | 0.000637 | 0.000646 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002559 | 0.002579 |         0 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.000955 | 0.000959 |         0 |     100 |      random tail |
|  crumsort |   100000 |   32 | 0.000880 | 0.000886 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002663 | 0.002679 |         0 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.001396 | 0.001407 |         0 |     100 |      random half |
|  crumsort |   100000 |   32 | 0.001199 | 0.001211 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002309 | 0.002344 |         0 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.001384 | 0.001403 |         0 |     100 |  ascending tiles |
|  crumsort |   100000 |   32 | 0.001517 | 0.001531 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002662 | 0.002685 |         0 |     100 |     bit reversal |
|  blitsort |   100000 |   32 | 0.002052 | 0.002175 |         0 |     100 |     bit reversal |
|  crumsort |   100000 |   32 | 0.001787 | 0.001805 |         0 |     100 |     bit reversal |

</details>
