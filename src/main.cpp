#include <iostream>
#include "LOB/OrderBook.h"

int main() {
    LOB::OrderBook book;

    std::cout << "--- CANCELLATION TEST ---\n";

    // 1. Add an order
    std::cout << "[1] Adding Order #1 (Buy 100 @ $100)...\n";
    book.addOrder(1, 100, 100, LOB::Side::Buy);

    // 2. See it in the book
    book.printBook();

    // 3. Decide to Cancel
    std::cout << "[2] Cancelling Order #1...\n";
    book.cancelOrder(1);

    // 4. Check if it's gone (Book should be empty)
    book.printBook();

    return 0;
}