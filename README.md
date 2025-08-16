# Parking Lot Management System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A C++ implementation of a parking lot system using:
- **Custom queue** (Linked list)
- **Hash tables** for O(1) lookups
- **Merge sort** for record sorting

## Features
- Real-time slot allocation
- Fee calculation ($2/hour)
- Persistent data storage

## How to Run
```bash
g++ -std=c++11 src/*.cpp -o parking_system
./parking_system
