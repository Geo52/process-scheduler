# Process Scheduler Simulation

The program simulates multiple CPU scheduling algorithms including First-In-First-Out (FIFO), Round Robin (RR), and Shortest Process Next (SPN).

## Components

The simulation includes:

1. **Long-term Scheduler**: Reads process information from a file and creates process threads at appropriate arrival times.
2. **Short-term Scheduler**: Selects processes from the ready queue and allocates CPU time based on the selected scheduling algorithm.
3. **Global Timer**: Maintains a system-wide clock for scheduling decisions.
4. **Process Threads**: Simulate CPU execution and track statistics.
5. **Ready Queue**: Manages processes waiting for CPU time.
6. **Scheduler Framework**: Supports multiple scheduling algorithms through function pointers.

## How It Works

- Each process is represented by a thread with a Process Control Block (PCB)
- The long-term scheduler creates processes according to their arrival times
- Processes are placed in a ready queue when they enter the system
- The short-term scheduler allocates CPU time to processes from the ready queue based on the selected algorithm
- Mutex locks and semaphores ensure proper synchronization
- Statistics (turnaround time, waiting time) are collected for each process

## Scheduling Algorithms

The simulation supports the following scheduling algorithms:

1. **FIFO (First-In-First-Out)**: Processes are executed in the order they arrive.
2. **RR (Round Robin)**: Each process gets a fixed time quantum, then is preempted and placed back in the ready queue.
3. **SPN (Shortest Process Next)**: The process with the shortest service time is selected next.

## Input File Format

The input file should contain lines with two integers per line:
```
<arrival_time> <service_time>
```

Example:
```
0 8
2 4
4 9
```

## Compilation

```bash
make
```

## Execution

```bash
./simulation_scheduler [-fifo | -rr | -spn] processes.txt
```

Where:
- `-fifo`: Use First-In-First-Out scheduling
- `-rr`: Use Round Robin scheduling
- `-spn`: Use Shortest Process Next scheduling
- `processes.txt`: Input file containing process arrival and service times

## Cleanup

```bash
make clean
```
