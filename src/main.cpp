#include <iostream>
#include "LOB/OrderBook.h"

int main() {
    LOB::OrderBook book;

    std::cout << "Task 2 Test: Storage...\n";

    // Add 1 Buy Order and 1 Sell Order
    book.addOrder(1, 100, 50, LOB::Side::Buy);
    book.addOrder(2, 105, 10, LOB::Side::Sell);

    // Should print them out
    book.printBook();

    return 0;
}