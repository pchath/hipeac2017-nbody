# hipeac2017-nbody
Initial commit including various optimizations of nbody (O(n*2)) algorithm on x86 machine.

Various optimizations on nbody (O(n*2)) algorithm on x86 machine. These
optimizations include 1) Alignment, 2) Vectorization, 3) Tiling, 4)
Parallelization. Most of this code is inspired from
"https://github.com/harrism/mini-nbody/tree/master/mic". 

- I have done some changes on top of the existing implementation such as increasing the coarse granularity of openmp threads and reducing number of synchronization points by keeping "omp parallel for" at tile iterator instead of on an inner loop. 

- Allocation and Alignment done using mkl_malloc and mkl_free for better performance

- Vectorized the inner most loop in the main iterator loop.
