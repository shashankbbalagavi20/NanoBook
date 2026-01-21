/**
 * @file OrderBookTests.cpp
 * @brief Unit Tests for the OrderBook logic using Google Test.
 * @details
 * Verified functionality:
 * 1. Order Submission (Smoke Test)
 * 2. Trade Execution (Matching Logic)
 * 3. Order Cancellation
 */
#include <gtest/gtest.h>
#include "../include/LOB/OrderBook.h"

/**
 * @class OrderBookTest
 * @brief Test Fixture class.
 * @details
 * Google Test will instantiate this class once for EACH test case.
 * This ensures that every test starts with a clean, empty OrderBook.
 */
class OrderBookTest : public ::testing::Test {
protected:
    LOB::OrderBook book;
};

// 1. Basic Add Verification
TEST_F(OrderBookTest, AddOrderNoCrash) {
    // "Smoke Test": Just ensure the basic API calls don't crash (segfault/throw).
    EXPECT_NO_THROW(book.addOrder(1, 100, 10, LOB::Side::Buy));
    EXPECT_NO_THROW(book.addOrder(2, 105, 10, LOB::Side::Sell));
}

// 2. Execution Verification
TEST_F(OrderBookTest, ExecutesTrade) {
    // Setup: Add a resting Buy order
    book.addOrder(1, 100, 10, LOB::Side::Buy);
    
    // Action: Add a matching Sell order (Aggressive)
    // We capture stdout because the engine prints "TRADE EXECUTE" to console.
    testing::internal::CaptureStdout();
    
    book.addOrder(2, 100, 10, LOB::Side::Sell);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    // Assertion: Verify the trade log appeared
    EXPECT_TRUE(output.find("TRADE EXECUTE") != std::string::npos);
}

// 3. Cancel Verification
TEST_F(OrderBookTest, CancelsOrder) {
    // Setup: Add an order
    book.addOrder(1, 100, 10, LOB::Side::Buy);
    
    // Action: Cancel it
    testing::internal::CaptureStdout();
    book.cancelOrder(1);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Assertion: Verify the cancellation log
    // Note: Matches the "Cancelled" string from OrderBook.cpp
    EXPECT_TRUE(output.find("Cancelled") != std::string::npos);
}