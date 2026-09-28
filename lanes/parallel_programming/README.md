# Roadmap

## PRPR-0: Set up CUDA and compile hello world 

<details>
- Practical:
    - set up cuda
    - detect gpu
    - print gpu name
    - print gpu properties
    - launch trivial kernel
    - use nvcc and nvidia-smi
- Theoretical:
    - what nvidia gpu do I have?
    - what cuda version is installed?
    - how do I compile a .cu file?
    - how do I debug a basic cuda error?
    - where does the cuda toolkit live?
    - what other tools exsit for profiling and debugging?
- Hardware:
    - linux pc nvidia gpu
</details>

## PRPR-1: Vector addition

<details>
- Practical:
    - implement vector addition kernel
    - vary array size, block size, measure kernel runtime...
    - try to optimize it
    - see if there is some interesting thing you can use vector kernel for
- Theoretical:
    - explain each term:
        - kernel
        - thread
        - block
        - grid
        - threadIdx
        - blockIdx
        - blockDim
        - cudaMalloc
        - cudaMemcpy
        - kernel launch syntax
        - cudaFree
- Hardware:
    - linux pc nvidia gpu
</details>

## PRPR-2: Multidimensional threads: Image blur 

<details>
- Practical:
    - implement image blur kernel
    - try to optimize it
    - see if you can build something cool with it
    - implement same thing in cpu and compare
- Theoretical:
    - 2D grids
    - 2D blocks
    - mapping to image coords
    - boundary checks
    - basic image memory layout
- Hardware:
    - linux pc nvidia gpu
</details>

## PRPR-3: Matrix multiplication and memory hierarchy

<details>
- Practical:
    - implement naive matrix kernel
    - implement tiled version of the kernel
    - investigate memory, coalescing, tile sizes, boundary checks, register usage, occupancy
    - optimize it further
    - find a fun use case
- Theoretical:
    - How does the memory hierarchy affect kernel performance?
- Hardware:
    - linux pc nvidia gpu
</details>

## PRPR-4+: Remaining work 

### Midterm

- Convolution

- Stencil

- Parallel histogram

- Reduction

- Prefix sum (scan)

- Merge

### Longterm

- Advanced kernels in Programming massively parallel processors

- Vulkan compute
