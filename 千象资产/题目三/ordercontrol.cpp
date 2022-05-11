#include "ordercontrol.hpp"

/*
 * 可以做到与N和M无关，时间复杂度:O(n)
 */

OrderController::OrderController(int timeWindow, int upperLimit)
	: m_iTimeWindow(timeWindow)
	, m_iUpperLimit(upperLimit)
{
}

bool
OrderController::allow(const OrderRequest &order)
{
	if (m_qOrder.empty()) {
		m_qOrder.push(order);
		return true;
	}

	while (!m_qOrder.empty()) {
		auto frontOrder = m_qOrder.front();
		if (order.reqTimestamp - frontOrder.reqTimestamp > m_iTimeWindow) {
			m_qOrder.pop();
			continue;
		}
		if (m_qOrder.size() < m_iUpperLimit) {
			m_qOrder.push(order);
			return true;
		} else {
			return false;
		}
	}

	return true;
}
