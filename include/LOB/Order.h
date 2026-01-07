#pragma once
#include <cstdint>

namespace LOB {

    using OrderId = uint64_t;
    using Price = uint64_t;
    using Quantity = uint64_t;

    enum class Side : uint8_t { Buy, Sell };

    struct Order {
        OrderId id;
        Price price;
        Quantity quantity;
        Side side;

        Order* next = nullptr;
        Order* prev = nullptr;

        Order() = default;
        Order(OrderId id, Price price, Quantity qty, Side side) : id(id), price(price), quantity(qty), side(side) {}
    };
}