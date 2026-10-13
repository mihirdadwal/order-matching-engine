// ============================================================================
// main.cpp – Multi-Stock Interactive Exchange Terminal (REPL)
// ============================================================================

#include <iostream>
#include <string>
#include "Exchange.h"
#include "CommandParser.h"
#include "Display.h"

void printHelp() {
    std::cout << "\n=================== AVAILABLE COMMANDS ===================\n"
              << " 1. Limit Order:    BUY [SYMBOL] LIMIT <quantity> <price>\n"
              << "                    SELL [SYMBOL] LIMIT <quantity> <price>\n"
              << "                    e.g., BUY AAPL LIMIT 100 150.25\n"
              << "                    e.g., SELL TSLA LIMIT 50 240.00\n"
              << " 2. Market Order:   BUY [SYMBOL] MARKET <quantity>\n"
              << "                    SELL [SYMBOL] MARKET <quantity>\n"
              << "                    e.g., BUY AAPL MARKET 30\n"
              << " 3. View Book:      BOOK [SYMBOL]\n"
              << "                    e.g., BOOK AAPL  or  BOOK\n"
              << " 4. Cancel Order:   CANCEL <orderId>\n"
              << "                    e.g., CANCEL 1\n"
              << " 5. Active Stocks:  SYMBOLS\n"
              << " 6. Help:           HELP\n"
              << " 7. Exit:           QUIT or EXIT\n"
              << "==========================================================\n\n";
}

int main() {
    std::cout << "\033[1;36m"
              << "========================================================\n"
              << "        MULTI-STOCK ORDER MATCHING EXCHANGE             \n"
              << "   Supporting AAPL, TSLA, MSFT and any custom ticker.   \n"
              << "   Type HELP for commands, or QUIT to exit.             \n"
              << "========================================================\n"
              << "\033[0m\n";

    Exchange exchange;
    // Pre-register AAPL so it's ready out of the box
    exchange.getOrCreateBook("AAPL");

    CommandParser parser;
    std::string line;

    std::cout << "> ";
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << "> ";
            continue;
        }

        // Special command: SYMBOLS
        if (line == "SYMBOLS" || line == "symbols") {
            auto syms = exchange.getActiveSymbols();
            std::cout << "Active Traded Symbols (" << syms.size() << "): ";
            for (const auto& s : syms) std::cout << s << " ";
            std::cout << "\n> ";
            continue;
        }

        ParsedCommand cmd = parser.parse(line);

        if (cmd.type == CommandType::Quit) {
            std::cout << "Exiting Exchange. Goodbye!\n";
            break;
        } else if (cmd.type == CommandType::Help) {
            printHelp();
        } else if (cmd.type == CommandType::PrintBook) {
            std::string sym = cmd.symbol.empty() ? "AAPL" : cmd.symbol;
            const OrderBook* book = exchange.getBook(sym);
            const MarketStats* stats = exchange.getStats(sym);
            if (book && stats) {
                Display::printOrderBook(*book, *stats);
            } else {
                std::cout << "No trading activity found for symbol '" << sym << "'.\n";
            }
        } else if (cmd.type == CommandType::CancelOrder) {
            if (cmd.orderIdToCancel) {
                OrderId id = *cmd.orderIdToCancel;
                if (exchange.cancelOrder(id)) {
                    std::cout << "\033[32mSUCCESS: Order #" << id << " cancelled.\033[0m\n";
                } else {
                    std::cout << "\033[31mFAILED: Order #" << id << " not found or already filled.\033[0m\n";
                }
            }
        } else if (cmd.type == CommandType::AddOrder) {
            if (cmd.order) {
                Order order = *cmd.order;
                OrderId orderId = order.id;
                Quantity initialQty = order.initialQuantity;
                std::string sym = order.symbol;

                auto trades = exchange.submitOrder(order);

                // Print any executions with ANSI color
                for (const auto& trade : trades) {
                    Display::printTrade(trade);
                }

                if (trades.empty()) {
                    std::cout << "Order #" << orderId << " (" << sym << " "
                              << toString(order.side) << " " << initialQty
                              << " shares) placed in book.\n";
                } else {
                    Quantity tradedQty = 0;
                    for (const auto& t : trades) tradedQty += t.quantity;
                    if (tradedQty < initialQty && order.type == OrderType::Limit) {
                        std::cout << "Remaining " << (initialQty - tradedQty)
                                  << " shares of Order #" << orderId
                                  << " placed in " << sym << " book.\n";
                    }
                }
            }
        } else {
            std::cout << "\033[31mERROR: " << cmd.errorMessage << "\033[0m\n";
        }

        std::cout << "> ";
    }

    return 0;
}
