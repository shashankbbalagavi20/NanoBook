#include <iostream>
#include "LOB/OrderBook.h"

int main() {
    LOB::OrderBook book;

    std::cout << "--- MATCHING ENGINE TEST ---\n";

    // 1. Bob places a SELL order (Passive)
    // "I want to sell 100 shares at $105."
    std::cout << "[1] Adding Sell Order #1 (100 @ $105)...\n";
    book.addOrder(1, 105, 100, LOB::Side::Sell);

    // 2. Alice places a BUY order (Aggressive)
    // "I want to buy 50 shares at $105."
    // EXPECTATION: Match! Alice buys 50 from Bob.
    // Alice should disappear (filled). Bob should have 50 left.
    std::cout << "[2] Adding Buy Order #2 (50 @ $105)...\n";
    book.addOrder(2, 105, 50, LOB::Side::Buy);

    // 3. Charlie places a huge BUY order
    // "I want to buy 200 shares at $106."
    // EXPECTATION: Match! Charlie buys Bob's remaining 50.
    // Bob disappears. Charlie has 150 left waiting.
    std::cout << "[3] Adding Buy Order #3 (200 @ $106)...\n";
    book.addOrder(3, 106, 200, LOB::Side::Buy);

    book.printBook();

    return 0;
}