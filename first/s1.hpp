#pragma once

// data was inserted quickly and periodicly in two hours, which has 'flood
// peak', then slowly.
// Y[i, j] = F(Y[i, j - i], X[i, j])
// Y[i, 0] = X[i, 0]

#include <queue>
#include <functional>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <map>
#include <vector>

template <class Tx, class Ty> class S {
public:
	using F = std::function<Ty(const Ty &, const Tx &)>;

private:
	using umap_dta = std::unordered_map<int,
		std::priority_queue<Tx, std::vector<Tx>, std::greater<Tx> > >;
	using umap_y = std::unordered_map<int, Ty>;

public:
	S(F f)
		: f(f)
	{
	}

	~S() {}

	void
	pushl(const Tx &x)
	{
		// push
		if (0 == hash_pl.count(x.i)) {
			hash_pl.insert(std::make_pair(x.i, umap_dta::mapped_type()));
		}
		auto &pl = hash_pl.at(x.i);
		pl.push((x));

		if (0 == x.j) {
			Ty y;
			y.i = x.i;
			y.j = -1;
			hash_ymax.insert(std::make_pair(y.i, y));
		}
		if (0 == hash_ymax.count(x.i)) {
			return;
		}
		while (!pl.empty()) {
			auto &y = hash_ymax.at(x.i);
			auto x = pl.top();
			if (x.j == y.j + 1) {
				y = f(y, x);
				qr.push(y);
				pl.pop();
			} else {
				break;
			}
		}
	}


	bool
	emptyr()
	{
		return qr.empty();
	}

	Ty &
	frontr()
	{
		return qr.front();
	}

	void
	popr()
	{
		qr.pop();
	}

private:
	void dealdata();

private:
	umap_dta hash_pl;
	std::queue<Ty> qr;
	F f;
	umap_y hash_ymax;
	umap_dta hash_plt; // temp l

	std::mutex m; // mutex
};
