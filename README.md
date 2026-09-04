# OS Simulator — Dynamic Partitioning Memory Manager

A simulated operating system written in C that loads and executes programs on a virtual CPU while managing physical memory through contiguous allocation with dynamic partitioning. Each process receives an isolated memory space, and every read and write is bounds-checked against that space at runtime — a process that reaches outside its own allocation is terminated and its memory reclaimed, without bringing down the rest of the system.

## What it does

The simulator models the pieces of a small OS that have to cooperate to run multiple processes in a shared physical address space:

- A **memory manager** hands out non-overlapping partitions from a single flat memory array, tracking free space in an address-ordered hole list and reclaiming it on process termination.
- A **round-robin scheduler** interleaves execution across processes with a fixed time quantum.
- A **CPU** fetches, decodes, and executes instructions against the memory array, routing every access through the memory manager's permission check.
- A **disk** loads program images from disk into whatever partition the memory manager assigned.

The interesting part is what happens when things go wrong: an allocation that can't be satisfied and an access that crosses a partition boundary are both handled as recoverable errors, and the system keeps running.

## Architecture

| Component | Responsibility |
|---|---|
| `main.c` | Initializes the memory pool and hole list, loads the program list, drives the simulation loop, prints final statistics |
| `smm.c` / `smm.h` | Memory manager — first-fit allocation, deallocation, hole list maintenance, coalescing, permission checks |
| `memory.h` | Physical RAM model; `mem_read` / `mem_write` consult the memory manager before every access |
| `cpu.h` | Register file and fetch-decode-execute cycle |
| `disk.h` | Reads the program list, requests allocations, loads program images into memory |
| `scheduler.h` | Ready queue, round-robin dispatch, process removal, current-PID lookup |

## Memory management

**Allocation — first fit.** The manager walks an address-ordered singly linked list of holes and takes the first one large enough to satisfy the request. The process is placed at the hole's base address; the hole shrinks to reflect the space consumed, or is dropped from the list entirely if it was consumed exactly. A request that no hole can satisfy is rejected — the process is never created, an error is reported, and the system continues.

**Bookkeeping — allocation table.** A 256×3 table stores the PID, base address, and size of each live process. Size zero marks a free row, so placement is a scan for the first zero-size entry. This table is also what the permission check reads from.

**Deallocation and coalescing.** When a process terminates, a hole matching its base and size is inserted into the list at the correct address-ordered position, and the list is then swept for adjacent holes, which are merged into one. Without this, repeated allocate/free cycles would fragment memory into a long list of unusably small gaps. The simulator counts every hole created over its lifetime and reports the total at exit as a rough fragmentation metric.

**Protection.** Before any read or write, the CPU passes the target address and the running process's PID to `is_allowed_address()`, which looks up that PID's row and confirms the address falls within `[base, base + size)`. A violation terminates the offending process: its memory is deallocated and it is pulled from the ready queue. Execution then continues with the remaining processes.

### Memory manager interface

```c
int  allocate(int pid, int size);          /* 1 on success, 0 if rejected */
void deallocate(int pid);
void add_hole(int base, int size);         /* inserts in address order, then merges */
void remove_hole(int base);
void merge_holes(void);
int  find_hole(int size);                  /* first-fit; base address, or -1 */
int  get_base_address(int pid);
int  find_empty_row(void);
int  is_allowed_address(int pid, int addr);
```

## Scheduling

Round-robin with a 10-cycle time quantum. Process state is saved and restored across context switches. The simulation runs until the ready queue drains.

## Building and running

```bash
gcc main.c smm.c -o simulator
./simulator
```

Requires only a C compiler and the standard library. Developed and tested on Linux.

All `.c`, `.h`, and `.txt` files must sit in the same directory — the program list and input file paths are configured in `main.c`.

## Input format

The program list assigns each process a memory size and an instruction file. The requested size may exceed what the instructions actually occupy, which is what makes over-allocation and rejection possible:

```
64 program_add.txt
80 program_if.txt
40 program_add.txt
```

## Test scenarios

Three program lists exercise the interesting paths:

| File | Exercises |
|---|---|
| `program_list_valid.txt` | Three processes with sizes that all fit — the normal path |
| `program_list_invalid_allocation.txt` | Two processes request more memory than any hole can provide — rejection and recovery |
| `program_list_invalid_access.txt` | One process writes outside its partition — protection fault, termination, and cleanup |

The instruction programs (`loop50.txt`, `loop100.txt`, `loop200_valid.txt`, `loop200_invalid.txt`) loop a fixed number of times writing a marker value to a fixed absolute address, so the final state of memory shows which processes ran to completion and which were killed.

## Output

On exit the simulator reports the total number of holes created during the run, then dumps memory at addresses 30, 150, and 230 — the three locations the test programs write to.

## Limitations

Deliberately scoped to contiguous allocation, so it inherits that scheme's constraints: external fragmentation is possible even with coalescing, a process's memory can't grow after allocation, and there's no swapping or paging. First-fit was chosen for its low search cost; best-fit or a buddy allocator would trade lookup time for tighter packing. The allocation table is a fixed 256 rows and is searched linearly.
