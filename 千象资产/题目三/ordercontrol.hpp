#pragma once

#include <queue>

class OrderController {
public:
	struct OrderRequest {
		unsigned int reqTimestamp;
	};

public:
	OrderController(int timeWindow, int upperLimit);
	bool allow(const OrderRequest &order);
    
private:
    int m_iTimeWindow;
    int m_iUpperLimit;
    std::queue<OrderRequest> m_qOrder;
};