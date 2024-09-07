# Dining Philosophers Problem

This project is a C implementation of the classic **Dining Philosophers Problem**, where multiple philosophers alternate between thinking and eating. The philosophers must pick up two forks (one on the left and one on the right) to eat, but only one fork is available between each pair of philosophers. The solution uses POSIX threads, mutexes, condition variables, and semaphores to prevent deadlocks and simulate the process.

## Problem Overview

The Dining Philosophers Problem involves five philosophers sitting at a round table with a fork placed between each adjacent pair of philosophers. Philosophers can only eat if they have both the left and right forks, but they must share these forks with their neighbors. They spend their time alternating between thinking, getting hungry, and eating.

### Objective

- Ensure that no two philosophers can use the same fork at the same time.
- Avoid deadlock, where all philosophers are holding one fork and waiting for the other.
- Prevent starvation, allowing all philosophers to eventually eat.

## Solution Approach

- **Threads**: Each philosopher is represented by a thread.
- **Mutexes**: Each fork is protected by a mutex to ensure only one philosopher can hold it at a time.
- **Semaphores**: A semaphore limits the number of philosophers who can sit at the table and request forks to avoid deadlock.
- **Condition Variables**: Used to signal when forks are available for a philosopher to pick up and eat.

## Prerequisites

To compile and run the program, you need:
- A C compiler (like `gcc`)
- POSIX-compliant libraries (available on most UNIX-like operating systems)

## Files

- `dining_philosophers.c`: Contains the main implementation of the Dining Philosophers Problem.
- `README.md`: Project documentation.

## How to Compile

To compile the code, run the following command in your terminal:

```bash
gcc -pthread -o main.c main

