# Roadmap

## MLIN-0: Build a minimal tensor library

<details>
- Practical:
    - Implement a C++ Tensor<T> class.
    - Support arbitrary rank and dynamic shapes.
    - Store shape, strides, number of elements and data pointer.
    - Implement indexing and basic contiguous allocation.
    - Implement reshape() without copying when possible.
    - Add basic tests for 1D/2D/3D/4D tensors.
    - Add a small Python/NumPy reference script for checking results.
- Theoretical:
    - What is a tensor from a computational perspective?
    - Shape vs stride vs storage size.
    - Row-major memory layout.
    - What does contiguous mean?
    - Why can reshape sometimes be done without copying?
    - How does NCHW differ from NHWC?
    - How do strides translate multidimensional indexing into a linear address?
    - What are the implications of contiguous vs strided memory access?
- Hardware:
    - linux pc nvidia gpu
</details>

## MLIN-1: Implement the basic neural network operator layer

<details>
- Practical:
    - Implement a small operator library using your tensor class:
        - ReLU
        - elementwise add/multiply
        - matrix multiplication
        - numerically stable softmax
        - 2D max pooling
    - For every operator:
        - define input/output shapes
        - validate inputs
        - implement a straightforward reference version
        - test against NumPy/PyTorch results
        - measure execution time
    - Finish by composing operators into:
        input -> matmul -> ReLU -> matmul -> softmax
- Theoretical:
    - What are activations, weights and biases?
    - What does a linear layer actually compute?
    - Why is softmax numerically unstable if implemented naively?
    - Why are ReLU and elementwise operations so cheap?
    - What determines the computational complexity of each operator?
    - What does it mean for an operator to be compute-bound or memory-bound?
    - Why are GEMM and convolution so important in neural-network inference?
- Hardware:
    - linux pc nvidia gpu
</details>

## MLIN-2: Implement convolution and run your first complete CNN

<details>
- Practical:
    - Implement naive 2D convolution from scratch.
    - Support configurable:
        - channels
        - kernel size
        - stride
        - padding
    - Add max/average pooling.
    - Implement enough operators to execute a small CNN.
    - Create a tiny fixed model with randomly generated weights.
    - Compare every layer against a Python reference.
    - Produce an end-to-end inference result.
- Theoretical:
    - What exactly does convolution compute?
    - Input channels vs output channels.
    - Kernel, feature map and receptive field.
    - Stride and padding.
    - Why does convolution have so much computation?
    - How can convolution be represented as matrix multiplication?
    - What is the difference between a neural-network layer and an operator/kernel?
- Hardware:
    - linux pc nvidia gpu
</details>

## MLIN-3: Build the correctness + benchmarking infrastructure

<details>
- Practical:
    - Implement:
        - random tensor generation
        - reference implementation using NumPy
        - numerical comparison
        - relative/absolute error checking
        - warm-up iterations
        - repeated timing
        - median/min/p95 latency
        - throughput measurement
        - operator-level benchmarks
        - end-to-end benchmarks
- Theoretical:
    - Floating-point error and tolerances.
    - Absolute vs relative error.
    - Why benchmarking one iteration is misleading.
    - Warm-up effects.
    - Latency vs throughput.
    - FLOPs vs FLOP/s.
    - Memory bandwidth vs computational throughput.
    - Arithmetic intensity.
    - Why benchmark methodology matters.
- Hardware:
    - linux pc nvidia gpu
</details>

## MLIN-4: Optimize GEMM and understand CPU performance

<details>
- Practical:
    - Take your naive matrix multiplication and progressively optimize it
    - Measure after every optimization:
        - latency
        - GFLOP/s
        - scaling with matrix size
        - single-thread vs multi-thread
        - cache effects
- Theoretical:
    - Why does loop ordering matter?
    - What happens in the cache hierarchy?
    - What is cache blocking/tiling?
    - What is spatial/temporal locality?
    - What is SIMD/vectorization?
    - What is the difference between instruction-level and thread-level parallelism?
    - Why doesn't adding threads produce linear speedup?
    - What are Amdahl's and Gustafson's laws?
    - What makes GEMM particularly well suited to optimization?
- Hardware:
    - linux pc nvidia gpu
</details>

## MLIN-5: Build a Tiny Inference Runtime

### Practical
Refactor your existing implementation into:

```text
Tensor
   ↓
Operator
   ↓
Node
   ↓
Graph
   ↓
Executor
```

- [ ] Create an `Operator` abstraction.
- [ ] Create a graph representation.
- [ ] Represent inputs/outputs of each node.
- [ ] Implement graph construction.
- [ ] Implement topological execution.
- [ ] Implement operator dispatch.
- [ ] Implement basic shape inference.
- [ ] Manage tensor lifetimes.
- [ ] Add simple model serialization.
- [ ] Execute your CNN through the graph rather than manually calling operators.
- [ ] Add execution timing per node.

Example:

```text
Input
  ↓
Conv
  ↓
ReLU
  ↓
Conv
  ↓
ReLU
  ↓
Pool
  ↓
GEMM
  ↓
Softmax
```

### Theoretical
- [ ] What is a computational graph?
- [ ] What is a node?
- [ ] What is an operator?
- [ ] What is topological ordering?
- [ ] Static vs dynamic graphs.
- [ ] What is shape inference?
- [ ] Why does tensor lifetime matter?
- [ ] Why can tensor memory be reused?
- [ ] What information does an inference runtime need from a model?
- [ ] Which graph transformations can be performed before execution?

### Hardware
- Linux x86-64 CPU

---

## MLIN-6: Implement Your First CUDA Operators

### Practical
Port selected operators from your CPU implementation to CUDA:

- [ ] ReLU
- [ ] Elementwise operation
- [ ] Reduction
- [ ] Softmax

For every operator:

```text
CPU reference
      ↓
CUDA implementation
      ↓
Correctness test
      ↓
Benchmark
      ↓
Profile
```

- [ ] Implement a basic CUDA kernel.
- [ ] Validate against your CPU implementation.
- [ ] Measure kernel execution time.
- [ ] Measure CPU -> GPU transfer time.
- [ ] Measure GPU -> CPU transfer time.
- [ ] Compare end-to-end CPU vs GPU execution.
- [ ] Profile at least one kernel.

### Theoretical
- [ ] CUDA execution model.
- [ ] Threads.
- [ ] Blocks.
- [ ] Grids.
- [ ] Warps.
- [ ] Global memory.
- [ ] Shared memory.
- [ ] Registers.
- [ ] Memory coalescing.
- [ ] Synchronization.
- [ ] Parallel reduction.
- [ ] Occupancy.
- [ ] Why can a GPU implementation be slower than a CPU implementation?
- [ ] What role does CPU↔GPU transfer play in inference latency?

### Hardware
- NVIDIA GPU on your Linux PC

---

## MLIN-7: Optimize CUDA GEMM

### Practical
Implement CUDA GEMM progressively:

```text
Naive CUDA GEMM
       ↓
Coalesced memory access
       ↓
Shared-memory tiling
       ↓
Register-level optimization
```

- [ ] Implement naive CUDA matrix multiplication.
- [ ] Benchmark it.
- [ ] Investigate global-memory access patterns.
- [ ] Improve memory coalescing.
- [ ] Implement shared-memory tiling.
- [ ] Experiment with tile sizes.
- [ ] Investigate register usage.
- [ ] Benchmark every version.
- [ ] Compare against cuBLAS.
- [ ] Measure latency, GFLOP/s, and scaling with matrix size.
- [ ] Profile the final implementation.

### Theoretical
- [ ] Why does GPU GEMM benefit from tiling?
- [ ] Global vs shared vs register memory.
- [ ] What is memory coalescing?
- [ ] What is warp execution?
- [ ] What is occupancy?
- [ ] How does shared memory reduce global-memory traffic?
- [ ] What is register tiling?
- [ ] Roofline model.
- [ ] Arithmetic intensity on GPUs.
- [ ] Kernel launch overhead.
- [ ] Why is highly optimized GEMM difficult to implement?
- [ ] Why is cuBLAS so difficult to beat?

### Hardware
- NVIDIA GPU on your Linux PC

---

## MLIN-8: Implement Basic Quantization

### Practical
Extend your tensor/operator infrastructure to support:

```text
FP32
FP16
INT8
```

- [ ] Implement FP32 -> INT8 quantization.
- [ ] Implement INT8 -> FP32 dequantization.
- [ ] Implement symmetric quantization.
- [ ] Implement asymmetric quantization.
- [ ] Implement per-tensor quantization.
- [ ] Implement INT8 matrix multiplication.
- [ ] Measure numerical error against FP32.
- [ ] Run a small model using quantized weights/activations.
- [ ] Compare accuracy/error, latency, and memory consumption.
- [ ] Investigate where quantization error is introduced.

### Theoretical
- [ ] Why does lower precision help inference?
- [ ] What are scale and zero-point?
- [ ] Symmetric vs asymmetric quantization.
- [ ] Per-tensor vs per-channel quantization.
- [ ] Weight vs activation quantization.
- [ ] Quantization error.
- [ ] Dynamic range.
- [ ] Accumulation precision.
- [ ] Why can INT8 be significantly more efficient than FP32?
- [ ] Why is hardware support important for quantization?

### Hardware
- Linux x86-64 CPU
- NVIDIA GPU for FP16 experiments where supported

---

## MLIN-9: Compare Against Production Inference Runtimes

### Practical
Take one or two real pretrained models.

Run them through:

```text
                 ┌── Your runtime
Model ───────────┼── ONNX Runtime
                 └── TensorRT
```

- [ ] Obtain an ONNX model.
- [ ] Understand its graph structure.
- [ ] Run the model with your runtime where supported.
- [ ] Run it with ONNX Runtime.
- [ ] Run it with TensorRT.
- [ ] Compare model loading time.
- [ ] Compare initialization time.
- [ ] Compare memory usage.
- [ ] Compare end-to-end latency.
- [ ] Compare throughput.
- [ ] Compare FP32 performance.
- [ ] Compare FP16 performance.
- [ ] Compare INT8 performance where available.
- [ ] Inspect the execution graph where tooling allows it.
- [ ] Identify the largest performance gap between your runtime and the production runtimes.
- [ ] Pick one bottleneck and investigate why it exists.

### Theoretical
- [ ] What is ONNX?
- [ ] What is an inference runtime?
- [ ] Model representation vs execution engine.
- [ ] What is graph optimization?
- [ ] What is operator fusion?
- [ ] What is constant folding?
- [ ] What is kernel selection?
- [ ] What is memory planning?
- [ ] What happens between loading a model and executing a kernel?
- [ ] Which responsibilities belong to the model, runtime, compiler, and hardware backend?
- [ ] Why do production inference engines outperform a simple operator-by-operator implementation?

### Hardware
- Primary: NVIDIA GPU + Linux
- Secondary: Qualcomm/Radxa platform after the NVIDIA implementation is working

## MLIN-10+: Remaining work 

### Midterm

- Vulkan

- Hexagon DSP

### Longterm

- ?
