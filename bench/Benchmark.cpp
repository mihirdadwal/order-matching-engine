// ============================================================================
// Benchmark.cpp – High-Throughput Performance Benchmarking Tool
//
// LEARNING NOTES (Benchmarking & Profiling):
// 1. std::chrono::high_resolution_clock: Platform's highest-resolution clock.
// 2. Throughput = Total Operations / Total Time (seconds) -> Orders / sec.
// 3. Latency = Total Time / Total Operations -> Time per single order.
// ============================================================================

#include "Exchange.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>

int main() {
    std::cout << "========================================================\n"
              << "       ORDER MATCHING ENGINE - PERFORMANCE BENCHMARK    \n"
              << "========================================================\n\n";

    Exchange exchange;
    const std::string symbol = "AAPL";
    exchange.getOrCreateBook(symbol);

    const size_t NUM_ORDERS = 500'000; // 500,000 orders
    std::cout << "Generating and processing " << NUM_ORDERS << " orders...\n";

    // Pre-generate order specs to benchmark pure engine execution
    // (excluding random number generation overhead during the timed loop)
    struct OrderSpec {
        Side side;
        OrderType type;
        Price price;
        Quantity qty;
    };

    std::vector<OrderSpec> specs;
    specs.reserve(NUM_ORDERS);

    Price midPrice = 15000; // $150.00
    for (size_t i = 0; i < NUM_ORDERS; ++i) {
        Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
        OrderType type = (i % 10 == 9) ? OrderType::Market : OrderType::Limit;

        // Spread prices tightly around midPrice so orders frequently match and cross
        Price offset = static_cast<Price>((i % 50) - 25); // -25 to +24 cents
        Price price = (type == OrderType::Market) ? 0 : (midPrice + offset);
        Quantity qty = static_cast<Quantity>(10 + (i % 100)); // 10 to 109 shares

        specs.push_back({side, type, price, qty});
    }

    std::cout << "Starting timed benchmark...\n";

    size_t totalTrades = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < NUM_ORDERS; ++i) {
        const auto& spec = specs[i];
        Order order(i + 1, symbol, spec.side, spec.type, spec.price, spec.qty);
        auto trades = exchange.submitOrder(order);
        totalTrades += trades.size();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double totalSeconds = std::chrono::duration<double>(endTime - startTime).count();
    double totalMs = totalSeconds * 1000.0;
    double ordersPerSec = NUM_ORDERS / totalSeconds;
    double avgLatencyNs = (totalSeconds * 1e9) / NUM_ORDERS;
    double avgLatencyMicroSec = avgLatencyNs / 1000.0;

    const MarketStats* stats = exchange.getStats(symbol);

    std::cout << "\n================== BENCHMARK RESULTS ==================\n"
              << "  Total Orders Processed : " << NUM_ORDERS << "\n"
              << "  Total Trades Executed  : " << totalTrades << "\n"
              << "  Total Traded Volume    : " << (stats ? stats->getTotalVolume() : 0) << " shares\n"
              << "  Last Traded Price      : " << (stats ? stats->lastPriceString() : "--") << "\n"
              << "  VWAP Execution Price   : " << (stats ? stats->vwapString() : "--") << "\n"
              << "--------------------------------------------------------\n"
              << "  Total Time Elapsed     : " << std::fixed << std::setprecision(2)
              << totalMs << " ms (" << totalSeconds << " s)\n"
              << "  Throughput             : " << std::fixed << std::setprecision(0)
              << ordersPerSec << " orders / second\n"
              << "  Average Latency        : " << std::fixed << std::setprecision(2)
              << avgLatencyMicroSec << " microseconds (" << avgLatencyNs << " ns/order)\n"
              << "========================================================\n\n";

    return 0;
}
