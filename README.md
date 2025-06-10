# 🧠🍝 Filosofos: The Dining Philosophers Problem

![Project Banner](https://media.giphy.com/media/26ufdipQqU2lhNA4g/giphy.gif)

**Filosofos** is a C++ console application developed with **Visual Studio Code**, aiming to demonstrate the classic **Dining Philosophers Problem** — a fundamental example of synchronization issues in concurrent processing. This project provides both educational insight and a practical implementation of thread management and mutual exclusion in operating systems.

---

## 📚 Problem Statement

The **Dining Philosophers Problem** is a classic synchronization problem involving five philosophers who alternate between thinking and eating. Each philosopher needs two forks (one on the left and one on the right) to eat. However, forks are limited, leading to potential **deadlocks**, **starvation**, or **resource contention**.

This project simulates these behaviors and explores various strategies to avoid synchronization issues.

---

## 🎯 Project Objectives

- ✅ Model the philosophers and forks using threads and mutexes.
- ✅ Avoid deadlocks using proper resource allocation strategies.
- ✅ Ensure fair access to resources, avoiding starvation.
- ✅ Provide a clear, console-based visualization of the states: **Thinking**, **Hungry**, **Eating**.
- ✅ Use gifs and diagrams for better understanding (see below).

---

## 🛠️ Tech Stack

| Component        | Technology         |
|------------------|--------------------|
| Language          | C++                |
| IDE               | Visual Studio Code |
| Threads           | `std::thread`      |
| Synchronization   | `std::mutex`, `std::unique_lock` |
| Build System      | CMake / g++        |

---

## 📦 How to Build and Run

### 🔧 Prerequisites

- C++17 or newer
- g++ or any C++ compiler
- CMake (optional)
- Terminal (Linux/Mac) or PowerShell (Windows)

### 🚀 Build and Run (with g++)

```bash
g++ -std=c++17 -pthread -o filosofos main.cpp
./filosofos
