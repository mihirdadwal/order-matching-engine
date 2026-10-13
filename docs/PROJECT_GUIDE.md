# Project Guide – Order Matching Engine

This document explains **what** we are building, **how** it works, and **what tools/tech** we use.
Read this first, then follow [LEARNING_ROADMAP.md](LEARNING_ROADMAP.md).

---

## 1. What is an Order Matching Engine?

Every stock exchange (NYSE, NASDAQ) has a program at its core that takes **buy** and **sell** orders
from people and **matches** them into trades. That program is the matching engine.

We are building a small, simple version of it that runs in the terminal.

### Real-life analogy
Think of a vegetable market:
- Buyers shout: "I'll pay **at most** ₹50 for 1kg tomatoes"
- Sellers shout: "I'll sell for **at least** ₹48"
- Since the buyer's max (50) ≥ seller's min (48), a deal happens.

The matching engine does exactly this, automatically, for thousands of orders.

---

## 2. Key Terms (Glossary)

| Term | Meaning | Example |
|---|---|---|
| **Order** | A request to buy or sell | "BUY 100 shares at $150" |
| **Side** | BUY or SELL | BUY |
| **Bid** | A buy order | BUY 100 @ $150 |
| **Ask** | A sell order | SELL 50 @ $151 |
| **Limit order** | Buy/sell only at this price **or better** | "BUY at most $150" |
| **Market order** | Buy/sell **right now** at whatever the best price is | "BUY 100, any price" |
| **Order book** | The list of all waiting (unmatched) orders | see below |
| **Best bid** | Highest price any buyer is willing to pay | $150 |
| **Best ask** | Lowest price any seller is willing to accept | $151 |
| **Spread** | Best ask − best bid | $1 |
| **Trade / Fill** | A completed match between a buyer and seller | 50 shares @ $150 |
| **Partial fill** | Only part of an order got matched; the rest keeps waiting | 50 of 100 filled |
| **Cancel** | Remove a waiting order from the book | CANCEL order #3 |
| **Price-time priority** | Best price goes first; if same price, whoever came first goes first | FIFO at each price |

---

## 3. What the Order Book Looks Like

```
        ASKS (sellers)  – lowest price on top is the "best ask"
        ----------------------------------
        $152.00   |  200 shares
        $151.00   |   50 shares   <-- best ask
        ==================================   spread = $1.00
        $150.00   |  100 shares   <-- best bid
        $149.50   |  300 shares
        ----------------------------------
        BIDS (buyers)   – highest price on top is the "best bid"
```

No trade happens here because the best buyer ($150) pays less than the best seller ($151).

---

## 4. How Matching Works (step by step)

**Example:**
1. Alice: `BUY LIMIT 100 @ $150` → no sellers yet → goes into the book as a bid.
2. Bob: `SELL LIMIT 30 @ $149` → best bid is $150 ≥ $149 → **match!**
   - Trade: 30 shares @ **$150** (the price of the order that was already waiting)
   - Bob's order is fully filled. Alice still has 70 shares waiting.
3. Carol: `SELL LIMIT 100 @ $150` → matches Alice's remaining 70 @ $150.
   - Alice fully filled. Carol has 30 left → goes into the book as an ask at $150.
4. Dave: `BUY MARKET 10` → takes the best ask → buys 10 @ $150 from Carol.

### Matching rules (the entire "brain" of the project)
```
When a new BUY order arrives:
    while order still has quantity AND there are sellers:
        bestAsk = lowest-priced sell order (oldest one first at that price)
        if LIMIT order and bestAsk.price > order.price: stop (too expensive)
        tradeQty = min(order.qty, bestAsk.qty)
        record trade at bestAsk.price
        reduce both quantities by tradeQty
        remove bestAsk from book if its qty becomes 0
    if order still has quantity:
        LIMIT  -> add the rest to the book
        MARKET -> discard the rest (nobody left to trade with)

A new SELL order is the mirror image (match against highest bids).
```

That is honestly the hardest part of the project. Everything else is plumbing.

---

## 5. Project Architecture

```
  User types a command
          |
          v
  +------------------+       +------------------+       +-------------+
  |  CommandParser   | ----> |    OrderBook     | ----> |   Trades    |
  | "BUY LIMIT 100   |       | - bids (map)     |       | printed on  |
  |  150.00" -> Order|       | - asks (map)     |       | screen      |
  +------------------+       | - match()        |       +-------------+
                             | - cancel()       |
                             +------------------+
```

### Files
```
order-matching-engine/
├── CMakeLists.txt          # build instructions (like package.json / pom.xml)
├── include/                # header files (.h) = "declarations" (like a Java interface)
│   ├── Order.h             # Order struct, Side & OrderType enums
│   ├── Trade.h             # Trade struct
│   ├── OrderBook.h         # OrderBook class – one stock's book + matching
│   ├── MarketStats.h       # last price, volume, VWAP           (upgrade 4)
│   ├── Exchange.h          # holds one OrderBook per symbol     (upgrade 1)
│   ├── CommandParser.h     # turns text into commands
│   ├── CsvLoader.h         # reads orders from a CSV file       (upgrade 2)
│   └── Display.h           # colored live order book screen     (upgrade 3)
├── src/                    # source files (.cpp) = "implementations"
│   ├── OrderBook.cpp       # matching logic lives here
│   ├── MarketStats.cpp
│   ├── Exchange.cpp
│   ├── CommandParser.cpp
│   ├── CsvLoader.cpp
│   ├── Display.cpp
│   └── main.cpp            # program entry point (the terminal loop)
├── tests/
│   └── OrderBookTests.cpp  # GoogleTest unit tests
├── bench/
│   └── Benchmark.cpp       # measures orders/second
├── data/
│   └── sample_orders.csv   # sample orders to replay
└── docs/                   # these documents
```

### Core data structures (the important design decision)

| What | C++ type | Why |
|---|---|---|
| Bids | `std::map<Price, std::deque<Order>, std::greater<Price>>` | `map` keeps prices **sorted**; `greater` puts the **highest** price first |
| Asks | `std::map<Price, std::deque<Order>>` | sorted **lowest** price first (default) |
| Orders at one price | `std::deque<Order>` | a queue: oldest order at the front = **time priority** (FIFO) |
| Find order for cancel | `std::unordered_map<OrderId, OrderLocation>` | O(1) lookup of an order's side & price by its ID |
| Trades | `std::vector<Trade>` | simple list of what happened |

**Java equivalents:** `std::map` ≈ `TreeMap`, `std::unordered_map` ≈ `HashMap`,
`std::deque` ≈ `ArrayDeque`, `std::vector` ≈ `ArrayList`.

### Why prices are stored as integers (cents)
`0.1 + 0.2 != 0.3` in floating point. Money must be exact, so `$150.25` is stored as `15025` cents
(`int64_t`). This is a classic interview talking point.

---

## 6. Terminal Commands (what the finished program will do)

| Command | Example |
|---|---|
| Limit order | `BUY AAPL LIMIT 100 150.00` |
| Market order | `SELL AAPL MARKET 20` |
| Cancel | `CANCEL 1` |
| Show a book | `BOOK AAPL` |
| Load a CSV file | `LOAD data/sample_orders.csv` |
| Quit | `QUIT` |

```
> BUY AAPL LIMIT 100 150.25
Order #1 added to book
> SELL AAPL LIMIT 30 149.00
TRADE AAPL: 30 @ 150.25 (buy #1, sell #2)
> BOOK AAPL
=================== AAPL ===================
   Last: 150.25   Volume: 30   Spread: --
--------------------------------------------
   ASKS   (none)
--------------------------------------------
   BIDS   150.25 |    70
============================================
> LOAD data/sample_orders.csv
Loaded 10,000 orders -> 6,214 trades in 12 ms
> QUIT
```
(Bids show in **green**, asks in **red** in the terminal.)

### CSV format (upgrade 2)
```
side,symbol,type,quantity,price
BUY,AAPL,LIMIT,100,150.25
SELL,TSLA,MARKET,50,
```

### Market stats (upgrade 4)
| Stat | Meaning |
|---|---|
| Last price | Price of the most recent trade |
| Volume | Total shares traded |
| Best bid / ask | Top of each side of the book |
| Spread | Best ask − best bid |
| VWAP | Volume-weighted average price = Σ(price × qty) / Σ(qty) |

---

## 7. What We Are NOT Building (keeping it simple)

- No networking / servers
- No multithreading
- No database
- No GUI (terminal only)
- No user accounts

These can be "future improvements" to mention in interviews.

---

## 8. Tech Stack Summary

| Tool | What it is | Why we use it |
|---|---|---|
| **C++17** | The language | Fast, used in real trading systems |
| **g++ (GCC)** | Compiler – turns `.cpp` into an `.exe` | Free, standard |
| **CMake** | Build system – tells the compiler what to build | Industry standard for C++ (like Maven/npm) |
| **STL** | C++ Standard Library (`map`, `vector`, `deque`...) | Ready-made data structures |
| **GoogleTest** | Unit testing framework | Like JUnit for C++ |
| **`<chrono>`** | Built-in timing library | To measure orders/sec in the benchmark |
| **VS Code** | Editor (you already have it) | With the C/C++ and CMake Tools extensions |
| **Git / GitHub** | Version control | Repo: https://github.com/mihirdadwal/order-matching-engine |

---

## 9. Build Milestones (after learning)

**Part A – Core engine (~600 lines)**

| # | Milestone | What you'll have |
|---|---|---|
| 1 | Setup + "Hello World" with CMake | Toolchain works |
| 2 | `Order` and `Trade` structs, enums | Data model |
| 3 | `OrderBook` – add limit orders + print book | Book displays correctly |
| 4 | Matching logic for limit orders (incl. partial fills) | Core engine works |
| 5 | Market orders + cancel | Feature complete |
| 6 | `CommandParser` + terminal loop in `main.cpp` | Usable program |
| 7 | GoogleTest unit tests | Proven correct |

**Part B – Upgrades (~+400 lines)**

| # | Milestone | What you'll have |
|---|---|---|
| 8 | `Exchange` – multiple stocks (`std::unordered_map<std::string, OrderBook>`) | AAPL, TSLA, ... |
| 9 | `MarketStats` – last price, volume, spread, VWAP | Real market data |
| 10 | `Display` – colored live order book | Looks like a trading screen |
| 11 | `CsvLoader` – replay orders from a file | Load thousands of orders |
| 12 | Benchmark (1M orders/sec) | A real number for your resume |
| 13 | README + push to GitHub | Done |

**Total: ~1,000 lines.**
