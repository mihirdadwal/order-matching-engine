// ============================================================================
// OrderBookTests.cpp – GoogleTest test suite for Order Matching Engine
//
// LEARNING NOTES:
// 1. TEST(TestSuiteName, TestName): GoogleTest macro defining a unit test.
// 2. EXPECT_EQ(val1, val2): Verifies equality (like JUnit assertEquals).
// 3. EXPECT_TRUE(cond): Verifies boolean is true.
// ============================================================================

#include <gtest/gtest.h>
#include "OrderBook.h"
#include "CommandParser.h"

class OrderBookTest : public ::testing::Test {
protected:
    OrderBook book{"AAPL"};
};

// 1. Basic insertion and best bid/ask detection
TEST_F(OrderBookTest, BestBidAndAskTracking) {
    EXPECT_FALSE(book.getBestBid().has_value());
    EXPECT_FALSE(book.getBestAsk().has_value());

    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 15000, 100));
    book.addOrder(Order(2, "AAPL", Side::Buy, OrderType::Limit, 15050, 100)); // Higher bid
    book.addOrder(Order(3, "AAPL", Side::Sell, OrderType::Limit, 15100, 50));
    book.addOrder(Order(4, "AAPL", Side::Sell, OrderType::Limit, 15200, 50)); // Higher ask

    EXPECT_EQ(*book.getBestBid(), 15050); // Highest buyer
    EXPECT_EQ(*book.getBestAsk(), 15100); // Lowest seller
}

// 2. Exact match (full fill for both parties)
TEST_F(OrderBookTest, ExactFullMatch) {
    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 15000, 100));
    auto trades = book.addOrder(Order(2, "AAPL", Side::Sell, OrderType::Limit, 15000, 100));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 100);
    EXPECT_EQ(trades[0].price, 15000);
    EXPECT_EQ(trades[0].buyOrderId, 1);
    EXPECT_EQ(trades[0].sellOrderId, 2);

    // Book should now be empty
    EXPECT_FALSE(book.getBestBid().has_value());
    EXPECT_FALSE(book.getBestAsk().has_value());
}

// 3. Partial fill on resting order
TEST_F(OrderBookTest, PartialFillRestingOrder) {
    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 15000, 100));
    auto trades = book.addOrder(Order(2, "AAPL", Side::Sell, OrderType::Limit, 15000, 40));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 40);

    // 60 shares should still remain at $150.00
    EXPECT_TRUE(book.getBestBid().has_value());
    EXPECT_EQ(*book.getBestBid(), 15000);
}

// 4. Partial fill on incoming order (rests remaining shares)
TEST_F(OrderBookTest, PartialFillIncomingOrderRestsRemaining) {
    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 15000, 40));
    // Seller wants to sell 100 @ 150.00: 40 match, 60 should rest as Ask
    auto trades = book.addOrder(Order(2, "AAPL", Side::Sell, OrderType::Limit, 15000, 100));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 40);

    // No more bids
    EXPECT_FALSE(book.getBestBid().has_value());
    // 60 shares resting on Ask side @ 150.00
    EXPECT_TRUE(book.getBestAsk().has_value());
    EXPECT_EQ(*book.getBestAsk(), 15000);
}

// 5. Price Priority: Seller matches highest bid first
TEST_F(OrderBookTest, PricePriority) {
    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 14900, 50));
    book.addOrder(Order(2, "AAPL", Side::Buy, OrderType::Limit, 15100, 50)); // Higher price

    // Seller offers 50 @ 149.00 -> should match with Order #2 at $151.00 first!
    auto trades = book.addOrder(Order(3, "AAPL", Side::Sell, OrderType::Limit, 14900, 50));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].buyOrderId, 2);
    EXPECT_EQ(trades[0].price, 15100); // Resting price priority
}

// 6. Time Priority (FIFO): Same price matches oldest order first
TEST_F(OrderBookTest, TimePriorityFIFO) {
    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 15000, 50)); // First
    book.addOrder(Order(2, "AAPL", Side::Buy, OrderType::Limit, 15000, 50)); // Second

    // Seller offers 50 @ 150.00 -> should match Order #1 first
    auto trades = book.addOrder(Order(3, "AAPL", Side::Sell, OrderType::Limit, 15000, 50));

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].buyOrderId, 1);
    EXPECT_EQ(trades[0].quantity, 50);
}

// 7. Market Orders sweep multiple price levels
TEST_F(OrderBookTest, MarketOrderSweepingLevels) {
    book.addOrder(Order(1, "AAPL", Side::Sell, OrderType::Limit, 15100, 50));
    book.addOrder(Order(2, "AAPL", Side::Sell, OrderType::Limit, 15200, 50));

    // Market Buy for 80 shares
    auto trades = book.addOrder(Order(3, "AAPL", Side::Buy, OrderType::Market, 0, 80));

    ASSERT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[0].quantity, 50);
    EXPECT_EQ(trades[0].price, 15100);

    EXPECT_EQ(trades[1].quantity, 30);
    EXPECT_EQ(trades[1].price, 15200);

    // Remaining ask has 20 shares at 15200
    EXPECT_EQ(*book.getBestAsk(), 15200);
}

// 8. Order Cancellation
TEST_F(OrderBookTest, CancelOrder) {
    book.addOrder(Order(1, "AAPL", Side::Buy, OrderType::Limit, 15000, 100));
    EXPECT_TRUE(book.getBestBid().has_value());

    EXPECT_TRUE(book.cancelOrder(1));
    EXPECT_FALSE(book.getBestBid().has_value());

    // Cancelling again should return false
    EXPECT_FALSE(book.cancelOrder(1));
}

// 9. CommandParser Unit Test
TEST(CommandParserTest, ParsingValidAndInvalidCommands) {
    CommandParser parser;

    // Valid limit buy
    auto cmd = parser.parse("BUY LIMIT 100 150.25");
    EXPECT_EQ(cmd.type, CommandType::AddOrder);
    ASSERT_TRUE(cmd.order.has_value());
    EXPECT_EQ(cmd.order->price, 15025);
    EXPECT_EQ(cmd.order->remainingQuantity, 100);
    EXPECT_EQ(cmd.order->side, Side::Buy);

    // Valid market sell with custom symbol
    cmd = parser.parse("SELL TSLA MARKET 50");
    EXPECT_EQ(cmd.type, CommandType::AddOrder);
    ASSERT_TRUE(cmd.order.has_value());
    EXPECT_EQ(cmd.order->symbol, "TSLA");
    EXPECT_EQ(cmd.order->type, OrderType::Market);

    // Cancel command
    cmd = parser.parse("CANCEL 42");
    EXPECT_EQ(cmd.type, CommandType::CancelOrder);
    ASSERT_TRUE(cmd.orderIdToCancel.has_value());
    EXPECT_EQ(*cmd.orderIdToCancel, 42);

    // Invalid command
    cmd = parser.parse("BUY LIMIT invalid 150.00");
    EXPECT_EQ(cmd.type, CommandType::Unknown);
}
