// ============================================================================
// main.cpp – Interactive Command-Line Interface (REPL)
// ============================================================================

#include <iostream>
#include <string>
#include "OrderBook.h"
#include "CommandParser.h"

void printHelp() {
    std::cout << "\n=================== AVAILABLE COMMANDS ===================\n"
              << " 1. Limit Order:    BUY LIMIT <quantity> <price>\n"
              << "                    SELL LIMIT <quantity> <price>\n"
              << "                    e.g., BUY LIMIT 100 150.25\n"
              << " 2. Market Order:   BUY MARKET <quantity>\n"
              << "                    SELL MARKET <quantity>\n"
              << "                    e.g., SELL MARKET 50\n"
              << " 3. With Symbol:    BUY AAPL LIMIT 100 150.25\n"
              << " 4. View Book:      BOOK\n"
              << " 5. Cancel Order:   CANCEL <orderId>\n"
              << "                    e.g., CANCEL 1\n"
              << " 6. Help:           HELP\n"
              << " 7. Exit:           QUIT or EXIT\n"
              << "==========================================================\n\n";
}

int main() {
    std::cout << "========================================================\n"
              << "        STOCK EXCHANGE ORDER MATCHING ENGINE            \n"
              << "  Type HELP for commands, or QUIT to exit.              \n"
              << "========================================================\n\n";

    OrderBook book("AAPL");
    CommandParser parser;
    std::string line;

    std::cout << "> ";
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << "> ";
            continue;
        }

        ParsedCommand cmd = parser.parse(line);

        if (cmd.type == CommandType::Quit) {
            std::cout << "Exiting Order Matching Engine. Goodbye!\n";
            break;
        } else if (cmd.type == CommandType::Help) {
            printHelp();
        } else if (cmd.type == CommandType::PrintBook) {
            book.printBook();
        } else if (cmd.type == CommandType::CancelOrder) {
            if (cmd.orderIdToCancel) {
                OrderId id = *cmd.orderIdToCancel;
                if (book.cancelOrder(id)) {
                    std::cout << "SUCCESS: Order #" << id << " cancelled.\n";
                } else {
                    std::cout << "FAILED: Order #" << id << " not found in the book.\n";
                }
            }
        } else if (cmd.type == CommandType::AddOrder) {
            if (cmd.order) {
                Order order = *cmd.order;
                OrderId orderId = order.id;
                Quantity initialQty = order.initialQuantity;

                auto trades = book.addOrder(order);

                // Print any trades that took place
                for (const auto& trade : trades) {
                    std::cout << "TRADE: " << trade.quantity << " shares @ "
                              << trade.priceString() << " (Buyer #" << trade.buyOrderId
                              << ", Seller #" << trade.sellOrderId << ")\n";
                }

                // If partially filled or fully rested, let user know
                if (trades.empty()) {
                    std::cout << "Order #" << orderId << " (" << toString(order.side)
                              << " " << initialQty << " shares) placed in book.\n";
                } else {
                    // Check if there was remaining quantity placed in book
                    // If order was a limit order and not completely filled:
                    Quantity tradedQty = 0;
                    for (const auto& t : trades) tradedQty += t.quantity;
                    if (tradedQty < initialQty && order.type == OrderType::Limit) {
                        std::cout << "Remaining " << (initialQty - tradedQty)
                                  << " shares of Order #" << orderId << " placed in book.\n";
                    }
                }
            }
        } else {
            // Unknown or syntax error
            std::cout << "ERROR: " << cmd.errorMessage << "\n";
        }

        std::cout << "> ";
    }

    return 0;
}
