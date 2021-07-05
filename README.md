Origin
------
Blitsort is based on [quadsort](https://github.com/scandum/quadsort). This document primarily lists notable differences and some benchmarks.

Quad swap
---------
Blitsort uses the same quad swap as quadsort. It is a routine that creates blocks of 32 sorted elements using 32 comparisons for in-order and reverse-order data, and aproximately 160 comparisons for random data.

A block of 4 elements is created with a decision tree, a block of 4 is turned into a block of 8 with a binary decision tree, and four blocks of 8 are turned into a block of 32 using a parity merge.

Rotate merge sort
-----------------
A rotate merge sort uses rotations to partition two sorted arrays until they're small enough to be merged using auxiliary memory. Blitsort does so by taking the center element of the first array, using a binary search to find all elements smaller than the center element in the second array, and performing a rotation. It does so recursively until a partition can be merged.

Monobound binary search
-----------------------
Blitsort uses a [monobound binary search](https://github.com/scandum/binary_search), which is up to two times faster than the binary search in general use.

Trinity rotation
----------------
Blitsort uses a [trinity rotation](https://github.com/scandum/rotate), which is significantly faster than rotations in general use.

Performance
-----------
Due to the monobound binary search and trinity rotation blitsort has exceptional performance.

Data Types
----------
Blitsort supports long doubles and 8, 16, 32, and 64 bit data types. By using 32 or 64 bit pointers it's possible to sort any other data type.

Interface
---------
The interface is the same one as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Memory
------
By default blitsort uses 512 elements worth of stack memory. The minimimum stack size on modern systems should be around 8192 KB.

The minimum memory requirement for blitsort is 32 elements of stack memory.

Performance is optimal while the auxiliary memory equals the square root of the array being sorted, which comes out at 262,144 elements with the default stack of 512 elements.

Big O
-----
```cobol
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│blitsort       ││n      │n log n│n log n││1      │1      │1      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```
