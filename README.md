# 🧠🍝 Filosofos: the perch route.

![Project Banner](https://media.giphy.com/media/26ufdipQqU2lhNA4g/giphy.gif)

**Filosofos** is a C++ console application developed with **Visual Code**, aiming to demonstrate the classic **Dining Philosophers Problem, the Barber and more.** — a fundamental example of synchronization issues in concurrent processing. This project provides both educational insight and a practical implementation of thread management and mutual exclusion in operating systems.

---

## 📚 Problem Statement

The **Dining Philosophers Problem** is a classic synchronization problem involving five philosophers who alternate between thinking and eating. Each philosopher needs two forks (one on the left and one on the right) to eat. However, forks are limited, leading to potential **deadlocks**, **starvation**, or **resource contention**.

This project simulates these behaviors and explores various strategies to avoid synchronization issues.

---

## 🎯 Project Objectives

- ✅ Model the philosophers and forks using threads and mutexes.
- ✅ Avoid deadlocks using proper resource allocation strategies.
- ✅ Ensure fair access to resources, avoiding starvation.
- ✅ Provide a clear, console-based visualization.

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

### 🔧 Prerequisites

- No character mode on the project implementation.
- Windows, C++ above 17.
- Win32 environment execution.
