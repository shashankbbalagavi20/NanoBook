# NanoBook: Low-Latency Limit Order Book (HFT)

A high-performance, single-threaded Limit Order Book (LOB) engine written in modern C++20. Designed for High-Frequency Trading (HFT) simulations, achieving **4.25 nanosecond** order allocation latency via custom memory architecture.

## 🚀 Key Features
* **Extreme Performance:** Utilizing a custom **Object Pool** (Slab Allocator) to bypass OS syscalls (`new`/`delete`), achieving a **4.3x speedup** over standard heap allocation.
* **O(1) Execution:**
    * **Add Order:** O(1) via Hash Map + Intrusive Doubly Linked List.
    * **Cancel Order:** O(1) via direct pointer access (no iteration required).
    * **Match:** O(1) relative to the number of matches (best-price priority).
* **Modern C++20:** Uses concepts, smart pointers, and template metaprogramming.

## 📊 Benchmarks
Comparison of Order Lifecycle (Allocate -> Initialize -> Deallocate):

| Method | Latency (ns) | Iterations/Sec | Speedup |
| :--- | :--- | :--- | :--- |
| **Standard (`new`/`delete`)** | 18.5 ns | 38 Million | 1.0x |
| **NanoBook Pool** | **4.25 ns** | **165 Million** | **4.3x** |

![Benchmark Screenshot](/src/benchmark.jpg)

## 🛠 Tech Stack
* **Language:** C++20
* **Build System:** CMake
* **Architecture:** Intrusive Linked Lists, Slab Allocation, Price-Time Priority Matching.

## 📂 Project Structure
* `LOB/LimitLevel`: The price queue (Doubly Linked List).
* `LOB/OrderBook`: The matching engine logic.
* `LOB/ObjectPool`: The custom memory manager.

## 💻 How to Run
```bash
# 1. Build the Project
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build

# 2. Run the Benchmark
./build/NanoBenchmark

# 3. Run the Engine Demo
./build/NanoBook