Origin
------
Blitsort is a rotate merge sort based on [quadsort](https://github.com/scandum/quadsort).

Visualization
-------------
In the visualization below nine tests are performed on 256 elements.

1. Random order
2. Ascending order
3. Ascending Saw
4. Generic random order
5. Descending order
6. Descending Saw
7. Random tail
8. Random half
9. Ascending tiles.

The upper half shows the swap memory of 32 elements, and the bottom half shows the main memory.
Colors are used to differentiate various operations. Rotations are in yellow and violet.

[![blitsort benchmark](/images/blitsort.gif)](https://www.youtube.com/watch?v=3ibxXQ1KCbI)

Quad swap
---------
Blitsort uses the same quad swap as quadsort. It is a routine that creates blocks of 32 sorted elements using 31 comparisons for in-order and reverse-order data, and aproximately 160 comparisons for random data.

A block of 4 elements is created with a decision tree, a block of 4 is turned into a block of 8 with a binary decision tree, and four blocks of 8 are turned into a block of 32 using a parity merge.

Rotate merge sort
-----------------
A rotate merge sort uses rotations to partition two sorted arrays until they're small enough to be merged using auxiliary memory. Blitsort does so by taking the center element of the first array, using a binary search to find all elements smaller than the center element in the second array, and performing an array rotation. It does so recursively until a partition becomes small enough to be merged.

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

Blitsort rotate merges recursively, requiring an additional log(n) memory.

There is currently no clear consensus on what constitutes as an in-place sort, it boils down to what someone considers a small enough memory footprint to be considered negligable. This typically ranges from the size of a cache line to the size of the L1 cache.

Performance
-----------
Blitsort has exceptional performance due to the quad swap, monobound binary search, and trinity rotation.

Blitsort's performance is similar to that of quadsort as long as the auxiliary memory is greater or equal to the square root of the array being sorted, which comes out at 262,144 elements with the default stack of 512 elements. Performance on larger arrays degrades marginally.

Data Types
----------
Blitsort supports long doubles and 8, 16, 32, and 64 bit data types. By using 32 or 64 bit pointers it's possible to sort any other data type.

Interface
---------
The interface is the same one as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Big O
-----
```cobol
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│blitsort       ││n      │n log n│n log n││1      │1      │1      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│timsort        ││n      │n log n│n log n││n      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```

Benchmark: blitsort vs std::stable_sort vs gfx::timsort
-------------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort). The source code
was compiled using `g++ -O3 -w -fpermissive bench.c`. Stablesort is g++'s std:stable_sort function.

The graph shows the relative performance on 100,000 32 bit integers.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.006048 | 0.006072 |         1 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.005839 | 0.005869 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007586 | 0.007613 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000658 | 0.000713 |         1 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000061 | 0.000063 |         1 |     100 |  ascending order |
|   timsort |   100000 |   32 | 0.000045 | 0.000045 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001345 | 0.001432 |         1 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.001046 | 0.001055 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000854 | 0.000860 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003905 | 0.003925 |         1 |     100 |    generic order |
|  blitsort |   100000 |   32 | 0.003664 | 0.003679 |         1 |     100 |    generic order |
|   timsort |   100000 |   32 | 0.005589 | 0.005616 |         1 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000895 | 0.000905 |         1 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000048 | 0.000048 |         1 |     100 | descending order |
|   timsort |   100000 |   32 | 0.000089 | 0.000092 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001040 | 0.001053 |         1 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000626 | 0.000634 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000455 | 0.000460 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002053 | 0.002113 |         1 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.001678 | 0.001688 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.001997 | 0.002018 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003523 | 0.003557 |         1 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.003212 | 0.003224 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004021 | 0.004041 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000971 | 0.000984 |         1 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.000575 | 0.000586 |         1 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000838 | 0.000864 |         1 |     100 |  ascending tiles |

</details>

Benchmark: blitsort vs qsort (quicksort)
----------------------------------------
The following benchmark was on CYGWIN_NT-10.0-WOW gcc version 10.2.0.
The source code was compiled using gcc -O3 bench.c.

The graph shows the relative performance on 100,000 32 bit integers.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.011868 | 0.011914 |   1732151 |      10 |     random order |
|  blitsort |   100000 |   32 | 0.010623 | 0.010844 |   1630132 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.000719 | 0.000733 |    300004 |      10 |  ascending order |
|  blitsort |   100000 |   32 | 0.000197 | 0.000197 |     99999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006692 | 0.006747 |   1723033 |      10 |    ascending saw |
|  blitsort |   100000 |   32 | 0.001893 | 0.001911 |    388188 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004165 | 0.004213 |    602517 |      10 |    generic order |
|  blitsort |   100000 |   32 | 0.007435 | 0.007485 |   1566304 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.001029 | 0.001048 |    400015 |      10 | descending order |
|  blitsort |   100000 |   32 | 0.000188 | 0.000192 |     99999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006883 | 0.007062 |   1780288 |      10 |   descending saw |
|  blitsort |   100000 |   32 | 0.001925 | 0.001953 |    399927 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.008802 | 0.008875 |   1695201 |      10 |      random tail |
|  blitsort |   100000 |   32 | 0.003056 | 0.003072 |    571457 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.010466 | 0.010562 |   1734815 |      10 |      random half |
|  blitsort |   100000 |   32 | 0.005813 | 0.005854 |    960136 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.000745 | 0.000753 |    184218 |      10 |         unstable |
|  blitsort |   100000 |   32 | 0.002868 | 0.002872 |    809499 |      10 |  ascending tiles |

</details>
