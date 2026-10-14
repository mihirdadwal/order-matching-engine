#pragma once

// ============================================================================
// CsvLoader.h – Reads and replays order streams from a CSV file
//
// LEARNING NOTES (File I/O & Parsing in C++):
// 1. std::ifstream: Input File Stream. Opens and reads from a file on disk.
// 2. std::getline(stream, line): Reads an entire line from the file.
// 3. std::getline(stringstream, cell, ','): Splits a line by comma delimiter.
// ============================================================================

#include "Exchange.h"
#include <string>
#include <cstdint>

struct LoadResult {
    size_t ordersLoaded{0};
    size_t tradesExecuted{0};
    double elapsedMs{0.0};
    bool success{false};
    std::string errorMessage;
};

class CsvLoader {
public:
    // Reads a CSV file and submits all parsed orders directly into the Exchange
    static LoadResult loadAndExecute(const std::string& filePath,
                                    Exchange& exchange,
                                    OrderId& nextOrderId);
};
