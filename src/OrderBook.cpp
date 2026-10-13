// ============================================================================
// OrderBook.cpp – Implementation of Order Book and Matching Logic
// ============================================================================

#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

OrderBook::OrderBook(std::string symbol)
    : symbol_(std::move(symbol)) {}

std::optional<Price> OrderBook::getBestBid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

std::optional<Price> OrderBook::getBestAsk() const {
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

// ----------------------------------------------------------------------------
// addOrder – Processes an incoming order:
// 1. Attempts to match against resting orders on the opposite side.
// 2. If it's a Limit order and has unfilled quantity remaining,
//    places the remaining quantity into the book.
// ----------------------------------------------------------------------------
std::vector<Trade> OrderBook::addOrder(Order order) {
    std::vector<Trade> trades;

    if (order.side == Side::Buy) {
        trades = matchBuyOrder(order);
        // If remaining shares exist and it's a Limit order, rest in the book
        if (order.type == OrderType::Limit && !order.isFilled()) {
            bids_[order.price].push_back(order);
            orderLookup_[order.id] = {Side::Buy, order.price};
        }
    } else {
        trades = matchSellOrder(order);
        // If remaining shares exist and it's a Limit order, rest in the book
        if (order.type == OrderType::Limit && !order.isFilled()) {
            asks_[order.price].push_back(order);
            orderLookup_[order.id] = {Side::Sell, order.price};
        }
    }

    // Append newly executed trades to the overall book history
    tradeHistory_.insert(tradeHistory_.end(), trades.begin(), trades.end());
    return trades;
}

// ----------------------------------------------------------------------------
// matchBuyOrder – Matches an incoming BUY order against resting ASKS
// Price Priority: asks_.begin() is always the lowest (best) selling price.
// Time Priority:  queue.front() is always the oldest order at that price.
// ----------------------------------------------------------------------------
std::vector<Trade> OrderBook::matchBuyOrder(Order& buyOrder) {
    std::vector<Trade> trades;

    while (!buyOrder.isFilled() && !asks_.empty()) {
        auto bestAskIt = asks_.begin();
        Price askPrice = bestAskIt->first;

        // If it's a Limit order and the lowest seller wants MORE than our max price, STOP.
        if (buyOrder.type == OrderType::Limit && askPrice > buyOrder.price) {
            break;
        }

        auto& queue = bestAskIt->second;
        Order& restingSell = queue.front();

        // The matched quantity is the minimum of what buyer wants vs seller has
        Quantity matchedQty = std::min(buyOrder.remainingQuantity, restingSell.remainingQuantity);

        // Trade price is the price of the order that was already resting in the book
        Trade trade(symbol_, buyOrder.id, restingSell.id, askPrice, matchedQty);
        trades.push_back(trade);

        buyOrder.remainingQuantity -= matchedQty;
        restingSell.remainingQuantity -= matchedQty;

        // If resting order is fully filled, remove it from the book and lookup table
        if (restingSell.isFilled()) {
            orderLookup_.erase(restingSell.id);
            queue.pop_front();

            // If no more orders remain at this price level, erase the price tier
            if (queue.empty()) {
                asks_.erase(bestAskIt);
            }
        }
    }

    return trades;
}

// ----------------------------------------------------------------------------
// matchSellOrder – Matches an incoming SELL order against resting BIDS
// Price Priority: bids_.begin() is always the highest (best) buying price.
// Time Priority:  queue.front() is always the oldest order at that price.
// ----------------------------------------------------------------------------
std::vector<Trade> OrderBook::matchSellOrder(Order& sellOrder) {
    std::vector<Trade> trades;

    while (!sellOrder.isFilled() && !bids_.empty()) {
        auto bestBidIt = bids_.begin();
        Price bidPrice = bestBidIt->first;

        // If it's a Limit order and the highest buyer offers LESS than our min price, STOP.
        if (sellOrder.type == OrderType::Limit && bidPrice < sellOrder.price) {
            break;
        }

        auto& queue = bestBidIt->second;
        Order& restingBuy = queue.front();

        Quantity matchedQty = std::min(sellOrder.remainingQuantity, restingBuy.remainingQuantity);

        // Trade price is the price of the resting order
        Trade trade(symbol_, restingBuy.id, sellOrder.id, bidPrice, matchedQty);
        trades.push_back(trade);

        sellOrder.remainingQuantity -= matchedQty;
        restingBuy.remainingQuantity -= matchedQty;

        if (restingBuy.isFilled()) {
            orderLookup_.erase(restingBuy.id);
            queue.pop_front();

            if (queue.empty()) {
                bids_.erase(bestBidIt);
            }
        }
    }

    return trades;
}

// ----------------------------------------------------------------------------
// cancelOrder – O(1) location lookup + removal from deque
// ----------------------------------------------------------------------------
bool OrderBook::cancelOrder(OrderId orderId) {
    auto lookupIt = orderLookup_.find(orderId);
    if (lookupIt == orderLookup_.end()) {
        return false; // Order not in book (already filled or never existed)
    }

    Side side = lookupIt->second.side;
    Price price = lookupIt->second.price;
    orderLookup_.erase(lookupIt);

    if (side == Side::Buy) {
        auto mapIt = bids_.find(price);
        if (mapIt != bids_.end()) {
            auto& queue = mapIt->second;
            queue.erase(
                std::remove_if(queue.begin(), queue.end(),
                               [orderId](const Order& o) { return o.id == orderId; }),
                queue.end()
            );
            if (queue.empty()) {
                bids_.erase(mapIt);
            }
            return true;
        }
    } else {
        auto mapIt = asks_.find(price);
        if (mapIt != asks_.end()) {
            auto& queue = mapIt->second;
            queue.erase(
                std::remove_if(queue.begin(), queue.end(),
                               [orderId](const Order& o) { return o.id == orderId; }),
                queue.end()
            );
            if (queue.empty()) {
                asks_.erase(mapIt);
            }
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
// printBook – Visual representation of the order book ladder
// ----------------------------------------------------------------------------
void OrderBook::printBook() const {
    std::cout << "\n=================== " << symbol_ << " ORDER BOOK ===================\n";

    std::cout << "--- ASKS (Sellers) ---\n";
    if (asks_.empty()) {
        std::cout << "  (no asks)\n";
    } else {
        // Asks printed from highest price down to lowest (so best ask sits right above bids)
        for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
            Price price = it->first;
            Quantity totalQty = 0;
            for (const auto& o : it->second) totalQty += o.remainingQuantity;

            double dollarPrice = price / 100.0;
            std::cout << "  $" << std::fixed << std::setprecision(2) << std::setw(8) << dollarPrice
                      << "  |  " << std::setw(6) << totalQty << " shares (" << it->second.size() << " orders)\n";
        }
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "--- BIDS (Buyers) ---\n";
    if (bids_.empty()) {
        std::cout << "  (no bids)\n";
    } else {
        // Bids printed from highest (best bid) down to lowest
        for (const auto& [price, queue] : bids_) {
            Quantity totalQty = 0;
            for (const auto& o : queue) totalQty += o.remainingQuantity;

            double dollarPrice = price / 100.0;
            std::cout << "  $" << std::fixed << std::setprecision(2) << std::setw(8) << dollarPrice
                      << "  |  " << std::setw(6) << totalQty << " shares (" << queue.size() << " orders)\n";
        }
    }

    std::cout << "========================================================\n\n";
}
