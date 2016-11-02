# hipeac2017-nbody

N-body simulation:

- 1) Direct particle based approach O(n*2): 
-- The first project is to explicitly compute the complete set of all-pairs interactions as part of n-body simulations. This algorithm is of O(nlogn) complexity. The optimizations such as alignment, layout transformations for vectorization, tiling and basic OpenMP parallelization are taken from "https://github.com/harrism/mini-nbody/tree/master/mic". 

-- I have added another optimization based on array expansion that makes the outermost loop as parallel to reduce the number of barrier instances and to increase the coarse granularity of OpenMP threads. (Look at file: nbody-par-opt.cpp)

-- I have added hints to Intel compiler for vectorizations at other places. 

- 2) Hierarchical Grid Methods - Tree Codes (Barnes and Hut) - O(nlogn):
-- The second project is based on the work of Barnes and Hut refinement of  Appel’s algorithm further by utilizing a hierarchical spatial grid decomposition using uniform bisection to create either a quad tree (for 2D) or an oct-tree (for 3D) rather than a binary tree. 

-- This project handles only quad trees and hence supports only two dimensions. Code for quad trees is taken from "https://github.com/ninjin/barnes-hut-sne". 

-- I have updated the existing code to make it parallelizable using OpenMP parallel for with reduction clause
