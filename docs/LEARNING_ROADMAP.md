# Learning Roadmap – C++ for the Order Matching Engine

You already know **Java** and **TypeScript**, so you don't need to learn programming from zero —
you need to learn **what's different in C++**. This roadmap only covers what this project needs.

**Total time: ~2–3 weeks** (1–2 hours/day), then ~1–2 weeks building.

**Main resource:** [learncpp.com](https://www.learncpp.com) – free, the best C++ tutorial.
**Reference:** [cppreference.com](https://en.cppreference.com) – look things up (like MDN for C++).

Tick the boxes as you go ✅

---

## Phase 0 – Setup (Day 1)
Follow [SETUP.md](SETUP.md).
- [ ] Install compiler (g++) and CMake
- [ ] Install VS Code extensions: **C/C++** and **CMake Tools**
- [ ] Compile and run a "Hello World"

---

## Phase 1 – C++ Basics (Days 2–4)
Mostly the same as Java – skim quickly.
- [ ] `#include`, `main()`, `std::cout` / `std::cin`
- [ ] Variables & types: `int`, `double`, `bool`, `char`, `std::string`
- [ ] Fixed-width ints: `int64_t`, `uint64_t` (we use these for price and IDs)
- [ ] `if`, `for`, `while`, `switch`
- [ ] Functions, `namespace`, `using`
- [ ] `std::getline` and `std::istringstream` (reading and splitting a line of input)

**Practice:** A program that reads `ADD 5 7` from the user and prints `12`.

---

## Phase 2 – What's Different from Java ⚠️ (Days 5–8) – most important phase
- [ ] **Compilation model:** header (`.h`) vs source (`.cpp`), `#pragma once`, linking
- [ ] **Values vs references:** in C++ objects are **copied** by default (unlike Java!)
- [ ] **References** (`int&`) and **const references** (`const Order&`) – avoid copies
- [ ] **Pointers** (`*`, `&`, `nullptr`) – just the basics, we barely use them
- [ ] **`const`** everywhere: const variables, const parameters, const member functions
- [ ] **Stack vs heap**, no garbage collector, what **RAII** means (objects clean themselves up)
- [ ] `auto` keyword (like `var` in Java)

**Practice:** Write a function `void addOne(int x)` and `void addOne(int& x)`. See the difference.

### Java → C++ cheat sheet
| Java | C++ |
|---|---|
| `String` | `std::string` |
| `ArrayList<T>` | `std::vector<T>` |
| `HashMap<K,V>` | `std::unordered_map<K,V>` |
| `TreeMap<K,V>` | `std::map<K,V>` |
| `ArrayDeque<T>` | `std::deque<T>` |
| `System.out.println(x)` | `std::cout << x << '\n';` |
| `final` | `const` |
| `var` | `auto` |
| `null` | `nullptr` |
| objects passed by reference | objects **copied** unless you use `&` |
| `enum` | `enum class` |
| JUnit | GoogleTest |
| Maven / Gradle | CMake |

---

## Phase 3 – Classes & OOP (Days 9–11)
- [ ] `struct` vs `class` (only difference: default public vs private)
- [ ] Constructors, member initializer lists
- [ ] `public` / `private`, member functions
- [ ] Splitting a class into `.h` (declaration) and `.cpp` (definition)
- [ ] `enum class` (we use `Side::Buy`, `OrderType::Limit`)
- [ ] Operator `<<` overloading (to print objects) – optional

**Practice:** Create a `BankAccount` class in `BankAccount.h` / `BankAccount.cpp` with deposit/withdraw.

---

## Phase 4 – STL Containers ⭐ (Days 12–15) – the heart of this project
- [ ] `std::vector` – push_back, loop, size
- [ ] `std::deque` – push_back, front, pop_front (our FIFO queue)
- [ ] `std::map` – insert, find, erase, **sorted** iteration, `begin()` = smallest key
- [ ] `std::map` with `std::greater<>` – sorted **largest first**
- [ ] `std::unordered_map` – fast lookup by key
- [ ] Iterators – `begin()`, `end()`, `it->first`, `it->second`, erasing while iterating
- [ ] Range-based for loop and structured bindings: `for (auto& [price, orders] : bids)`
- [ ] `std::min`, `std::optional` (a value that may or may not exist)
- [ ] Big-O of each container (interview question!)

**Practice:** Build a mini "price ladder": insert prices into a `std::map<int, int>`, print lowest→highest,
then do the same with `std::greater<int>` and print highest→lowest.

---

## Phase 5 – Tooling (Days 16–18)
- [ ] **CMake basics:** `cmake_minimum_required`, `project`, `add_executable`, `add_library`,
      `target_include_directories`, `set(CMAKE_CXX_STANDARD 17)`
- [ ] Build commands: `cmake -S . -B build` and `cmake --build build`
- [ ] **GoogleTest:** `TEST()`, `EXPECT_EQ`, `ASSERT_TRUE`, pulling it in with `FetchContent`
- [ ] `<chrono>` – `std::chrono::steady_clock::now()` to time code
- [ ] Debugging in VS Code (breakpoints)

**Needed for the upgrades:**
- [ ] Reading files with `std::ifstream` + `std::getline` (for the CSV loader)
- [ ] Splitting a CSV line by `,` using `std::getline(stream, field, ',')`
- [ ] Formatted output with `<iomanip>`: `std::setw`, `std::fixed`, `std::setprecision`
- [ ] ANSI color codes in the terminal (e.g. `"\033[32m"` = green, `"\033[0m"` = reset)

Resources: [CMake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html),
[GoogleTest primer](https://google.github.io/googletest/primer.html)

---

## Phase 6 – Domain Knowledge (Day 19, ~1 hour)
- [ ] Read [PROJECT_GUIDE.md](PROJECT_GUIDE.md) sections 1–4 again
- [ ] Search "order book explained" on YouTube and watch a short video
- [ ] Understand: bid, ask, spread, limit vs market, partial fill, price-time priority
- [ ] Do the Section 4 example from PROJECT_GUIDE.md **on paper**

---

## ✅ Ready-to-build checklist
You're ready to start coding when you can answer these:
1. What's the difference between `void f(Order o)` and `void f(const Order& o)`?
2. Why do we put declarations in `.h` and code in `.cpp`?
3. How do you get the smallest key in a `std::map`? The largest?
4. Why `std::deque` for orders at one price, and not `std::vector`?
5. What's the time complexity of insert/find in `std::map` vs `std::unordered_map`?
6. Why store prices as `int64_t` cents instead of `double`?
7. If a BUY LIMIT @ $150 arrives and the best ask is $149, at what price does the trade happen?

---

## Then: Build (Week 4–5)
Follow the milestones in [PROJECT_GUIDE.md – Section 9](PROJECT_GUIDE.md#9-build-milestones-after-learning).
We'll build them one at a time together, and you'll write/understand every line.
