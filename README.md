# Task Scheduler with Dependency Resolution

A C++ data structures and algorithms project that schedules tasks using graph-based dependency resolution.

## Features

- Models tasks as a directed graph.
- Detects invalid dependency cycles.
- Computes a valid execution order using Kahn's Algorithm.
- Computes a valid execution order using DFS-based topological sorting.
- Uses a priority queue / max heap to prioritize urgent ready tasks.
- Loads task data from a simple text file.

## Data Structures Used

- `unordered_map` for storing task metadata and adjacency lists.
- `vector` for dependency lists and final schedules.
- `queue` for Kahn's Algorithm.
- `priority_queue` for priority-based scheduling.
- DFS recursion state map for cycle detection.

## Input Format

Each task is written on one line:

```txt
task_id duration priority dependencies
```

Use `-` when a task has no dependencies. Use commas for multiple dependencies.

Example:

```txt
Frontend 6 7 BuildAPI,AuthModule
Deploy 2 10 Testing
```

## Build and Run

```bash
make
make run
```

Or manually:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o task_scheduler
./task_scheduler tasks.txt
```

To test cycle detection:

```bash
make cycle
```

## Algorithm Summary

### Kahn's Algorithm

1. Count incoming edges for every task.
2. Push all tasks with indegree `0` into a queue.
3. Repeatedly remove a ready task and reduce indegree of its dependent tasks.
4. If all tasks are processed, the order is valid.
5. If some tasks remain unprocessed, the graph contains a cycle.

Time complexity: `O(V + E)`

### DFS Topological Sort

1. Visit every unvisited task.
2. Mark a task as currently visiting.
3. Recursively visit all dependent tasks.
4. If a currently visiting task is reached again, a cycle exists.
5. Add completed tasks to the order and reverse the result.

Time complexity: `O(V + E)`

### Priority Queue Scheduling

This version still respects dependencies, but when multiple tasks are ready at the same time, it picks the task with the highest priority first.

Time complexity: `O((V + E) log V)`

## Resume Description

Task Scheduler with Dependency Resolution - Data Structures and Algorithms

- Developed a task scheduling system using graphs to model dependencies between tasks.
- Implemented Kahn's Algorithm and DFS-based topological sorting to compute valid execution orders.
- Enhanced scheduling efficiency using priority queues / heaps for time-sensitive task execution.
- Added cycle detection to reject invalid dependency graphs.
