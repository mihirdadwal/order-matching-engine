# Order Matching Engine (C++17)

A simple stock exchange **order matching engine** written in modern C++.
It maintains an order book of buy and sell orders and matches them using **price-time priority**.

> 🚧 **Work in progress**

## Features (planned)
- Limit and market orders
- Partial fills
- Order cancellation
- Price-time priority (best price first, then first-come-first-served)
- Multiple stock symbols (one order book per symbol)
- Market statistics: last price, volume, spread, VWAP
- Live colored order book in the terminal
- Replay orders from CSV files
- Unit tests with GoogleTest
- Throughput benchmark

## Tech
C++17 · STL · CMake · GoogleTest

## Docs
- [Project Guide](docs/PROJECT_GUIDE.md) – how it works & architecture
- [Learning Roadmap](docs/LEARNING_ROADMAP.md)
- [Setup](docs/SETUP.md)
