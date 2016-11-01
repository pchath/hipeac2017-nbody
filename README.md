# hipeac2017-nbody
Initial commit including various optimizations of nbody (O(n*2)) algorithm on x86 machine.

Various optimizations on nbody (O(n*2)) algorithm on x86 machine. These
optimizations include 1) Alignment, 2) Vectorization, 3) Tiling, 4)
Parallelization. Most of this code is inspired from
"https://github.com/harrism/mini-nbody/tree/master/mic". 
The following are the changes done on the existing code to improve the performance

- Updated the body force method to use less number of synchronization points and increased the coarse granularity using the array expansion techniques. (Look at file: nbody-par-opt.cpp)

- Allocation and Alignment done using mkl_malloc and mkl_free for better performance

- Vectorized the inner most loop in the main iterator loop.

- 
