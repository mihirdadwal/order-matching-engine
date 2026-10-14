# High-Performance Order Matching Engine (C++17)

A stock exchange **in-memory order matching engine** and multi-stock trading simulator written in modern C++17.
Maintains limit order books across multiple equity tickers with **price-time priority (FIFO)** and achieves **1.4M+ orders/sec** matching throughput.

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Build](https://img.shields.io/badge/Build-CMake%20%7C%20Ninja-brightgreen.svg)]()
[![Tests](https://img.shields.io/badge/Tests-GoogleTest%20(9%2F9%20Passing)-success.svg)]()
[![Throughput](https://img.shields.io/badge/Throughput-1.4M%2B%20orders%2Fsec-orange.svg)]()

---

## ⚡ Highlights & Key Features

- **Price-Time Priority (FIFO):** Bids sorted highest-first (`std::greater`), asks sorted lowest-first. Orders at identical price levels are matched strictly on arrival order using `std::deque`.
- **Order Types:** Supports both **Limit Orders** (with partial fills and resting order storage) and **Market Orders** (which aggressively sweep across price levels).
- **Multi-Symbol Exchange:** Concurrently manages multiple active order books (`AAPL`, `TSLA`, `MSFT`) with on-demand symbol creation.
- **Fast Order Cancellation:** $O(1)$ hash table lookup (`std::unordered_map`) for instant order location and deque removal.
- **Real-Time Market Statistics:** Tracks Last Traded Price, Total Volume, Bid-Ask Spread, and **VWAP** (Volume-Weighted Average Price).
- **Interactive ANSI Terminal UI:** High-contrast color-coded price ladder (Green bids, Red asks) and interactive REPL.
- **CSV Replay Engine:** Batch-loads and simulates thousands of orders from CSV files.
- **Comprehensive Unit Testing:** GoogleTest suite verifying price priority, FIFO execution, sweeps, and cancellations.
- **Throughput Benchmark:** Built-in performance test using `<chrono>`.

---

## 📊 Benchmark Performance

Benchmarked on **500,000 orders** (alternating Limit and Market orders across a tight spread):

| Metric | Result |
|---|---|
| **Total Orders Processed** | **500,000** |
| **Trades Executed** | **450,000** |
| **Execution Time** | **354.39 ms** (0.35 s) |
| **Throughput** | **1,410,875 orders / second** |
| **Average Latency** | **0.71 microseconds** (708 ns / order) |

---

## 🏗 Architecture & Core Data Structures

```
                 Incoming Order (CLI / CSV)
                            |
                            v
                   +------------------+
                   |  CommandParser   |
                   +------------------+
                            |
                            v
                   +------------------+
                   |     Exchange     |  (Manages multiple stocks & order routing)
                   +------------------+
                            |
              +-------------+-------------+
              v                           v
     +-----------------+         +-----------------+
     | OrderBook(AAPL) |         | OrderBook(TSLA) | ...
     +-----------------+         +-----------------+
        |           |
        v           v
    [ Bids ]     [ Asks ]
   (std::map)   (std::map)
        |           |
        v           v
   [std::deque] [std::deque] -> FIFO Time Priority at each price level
```

### Data Structure Selection Rationale

| Component | C++ Data Structure | Complexity | Reason |
|---|---|---|---|
| **Bids Book** | `std::map<Price, std::deque<Order>, std::greater<Price>>` | $O(\log P)$ | Self-balancing BST (Red-Black tree) maintaining highest buyer at `begin()` |
| **Asks Book** | `std::map<Price, std::deque<Order>>` | $O(\log P)$ | Maintains lowest seller at `begin()` |
| **Time Priority** | `std::deque<Order>` | $O(1)$ push / pop | Constant-time queue operations for oldest orders at a price level |
| **Order Cancel Index** | `std::unordered_map<OrderId, OrderLocation>` | $O(1)$ | Instant lookup to locate an order's book and price tier for cancellation |
| **Price Storage** | `int64_t` (cents/ticks) | Exact | Avoids floating-point rounding errors (`$150.25` = `15025` cents) |

---

## 💻 Terminal Commands & Usage

```bash
# Launch the interactive exchange
./build/engine.exe
```

```text
========================================================
        MULTI-STOCK ORDER MATCHING EXCHANGE             
   Supporting AAPL, TSLA, MSFT and any custom ticker.   
   Type HELP for commands, or QUIT to exit.             
========================================================

> BUY AAPL LIMIT 100 150.00
Order #1 (AAPL BUY 100 shares) placed in book.

> SELL AAPL LIMIT 50 151.00
Order #2 (AAPL SELL 50 shares) placed in book.

> BUY TSLA LIMIT 20 250.00
Order #3 (TSLA BUY 20 shares) placed in book.

> BOOK AAPL
========================== AAPL MARKET ==========================
  Last: --   Volume: 0   VWAP: --   Spread: $1.00
------------------------------------------------------------
  ASKS (Sellers)
    $  151.00  |      50 shares (1 orders)
------------------------------------------------------------
  BIDS (Buyers)
    $  150.00  |     100 shares (1 orders)
============================================================

> SELL AAPL MARKET 40
TRADE [AAPL]: 40 shares @ $150.00 (Buyer #1, Seller #4)

> LOAD data/sample_orders.csv
SUCCESS: Loaded 21 orders -> 8 trades executed in 0.35 ms.

> CANCEL 2
SUCCESS: Order #2 cancelled.

> SYMBOLS
Active Traded Symbols (3): MSFT TSLA AAPL 

> QUIT
Exiting Exchange. Goodbye!
```

---

## 🛠 Building & Running

### Prerequisites
- Modern C++ compiler supporting **C++17** (`g++`, `clang++`, or `MSVC`)
- **CMake** (3.16+) & **Ninja** or Make

### Build
```bash
# Configure build
cmake -S . -B build -G Ninja

# Compile all targets (engine, benchmark, tests)
cmake --build build
```

### Run Tests
```bash
./build/order_tests.exe
```

### Run Benchmark
```bash
./build/benchmark.exe
```

---

## 📚 Documentation
- [Project Architecture & Domain Guide](docs/PROJECT_GUIDE.md)
- [C++ Learning Roadmap for Java/TS Developers](docs/LEARNING_ROADMAP.md)
- [Toolchain Setup Guide](docs/SETUP.md)
