

## 🛠️ Project Deep Dives

### 🔹 Homework 1: Multiprocess Concept & Inter-Process Communication

* **Objective:** Demonstrate Inter-Process Communication (IPC) using bidirectional pipelines.
* **Mechanism:** * The parent process ($P_1$) acts as the orchestrator. It reads an integer $N$ from standard input and generates two distinct text files containing $N$ random numbers.
* The numbers are upper-bounded by a threshold derived dynamically via a customized ID offset system macro (`#define TC_LAST_3DIG 111`) to enforce absolute standard compliance.
* Two child processes ($P_2$ and $P_3$) are spawned using `fork()`. Communication routes are established using discrete UNIX `pipe()` file descriptors.
* $P_2$ parses `File1` to count **Prime Numbers**, while $P_3$ parses `File2` to count **Abundant Numbers**. Results are piped back to the parent to declare the mathematical winner.



### 🔹 Homework 2: Multi-Threaded Sync & Critical Section Boundary

* **Objective:** Manage CPU bound multi-threaded synchronization tasks under tight hardware constraint simulation.
* **Mechanism:**
* Uses the POSIX threads (`pthread`) library alongside standard UNIX counting semaphores (`sem_t`).
* Accepts a directory pathway containing target data files and a strict constraint integer mapping out the maximum concurrently active worker threads allowed inside the processing zone.
* Threads parallelly read the datasets to count **Deficient Numbers** while leveraging semaphores to ensure the worker boundary constraint is never breached.
* Real, user, and kernel system execution times are calculated via the UNIX `time` utility across incremental thread pool scalability configurations to observe core scheduling overheads.



### 🔹 Homework 3: Advanced Reader-Writer Paradigm with Authentication

* **Objective:** Implement a customized variant of the classic Reader-Writer synchronization problem with token/password security layer controls.
* **Mechanism:**
* Uses a robust layer of POSIX Mutexes (`pthread_mutex_t`) and binary semaphores to build a race-condition-free data structure.
* A master password verification routing table is auto-generated upon start. Only real reader/writer threads configured with authentic structural memory vectors can interact with the global shared database system (`BUFFER`).
* Equal streams of rogue dummy processes are concurrently instantiated with faulty structural passwords to rigorously test cross-thread validation and rejection boundaries.
* Execution flows are formatted as fully traceable transaction logs mapping operational roles, validation flags, and synchronized runtime snapshots.



---

## 🚀 Compilation and Execution Guide

A native GNU/Linux toolchain (`gcc`) is highly recommended for building these systems.

### ⚙️ Compilation

```bash
# Compile Homework 1 (Multiprocess & Pipes)
gcc -g HW1_Multiprocess/main.c -o HW1_Multiprocess/main

# Compile Homework 2 (Multi-threading & Semaphores)
gcc -g HW2_Synchronization/hw2_main.c -o HW2_Synchronization/hw2_main -lpthread

# Compile Homework 3 (Reader-Writer with Passwords)
gcc -g HW3_ReaderWriter/hw3_main.c -o HW3_ReaderWriter/hw3_main -lpthread

```

### 🏃 Running the Binaries

```bash
# Execution for Homework 1
./HW1_Multiprocess/main

# Execution for Homework 2 (Process 'myDir' using maximum 4 parallel threads)
chmod +x HW2_Synchronization/new.sh
./HW2_Synchronization/new.sh   # Generates sample data directory 'myDir'
time ./HW2_Synchronization/hw2_main myDir 4

# Execution for Homework 3
./HW3_ReaderWriter/hw3_main

```
