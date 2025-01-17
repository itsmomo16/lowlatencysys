#include "common.hpp"

// Base class for all order types
class BaseOrder {
public:
    virtual ~BaseOrder() = default;
    virtual bool should_trigger(const Quote& quote) = 0;
    virtual Order generate_order() = 0;
    
    std::string order_id;
    std::string symbol;
    bool is_buy;
    double quantity;
    std::chrono::nanoseconds timestamp;
};

// Limit order
class LimitOrder : public BaseOrder {
public:
    double limit_price;

    bool should_trigger(const Quote& quote) override {
        if (is_buy) {
            return quote.ask <= limit_price;
        } else {
            return quote.bid >= limit_price;
        }
    }

    Order generate_order() override {
        Order order;
        order.order_id = order_id;
        order.symbol = symbol;
        order.is_buy = is_buy;
        order.quantity = quantity;
        order.price = limit_price;
        order.timestamp = timestamp;
        return order;
    }
};

// Stop order
class StopOrder : public BaseOrder {
public:
    double stop_price;

    bool should_trigger(const Quote& quote) override {
        if (is_buy) {
            return quote.ask >= stop_price;
        } else {
            return quote.bid <= stop_price;
        }
    }

    Order generate_order() override {
        Order order;
        order.order_id = order_id;
        order.symbol = symbol;
        order.is_buy = is_buy;
        order.quantity = quantity;
        order.price = 0.0;  // Market order when triggered
        order.timestamp = timestamp;
        return order;
    }
};

// Stop-limit order
class StopLimitOrder : public BaseOrder {
public:
    double stop_price;
    double limit_price;
    bool stop_triggered = false;

    bool should_trigger(const Quote& quote) override {
        if (!stop_triggered) {
            if (is_buy) {
                stop_triggered = quote.ask >= stop_price;
            } else {
                stop_triggered = quote.bid <= stop_price;
            }
            return false;
        }
        
        if (is_buy) {
            return quote.ask <= limit_price;
        } else {
            return quote.bid >= limit_price;
        }
    }

    Order generate_order() override {
        Order order;
        order.order_id = order_id;
        order.symbol = symbol;
