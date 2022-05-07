#pragma once

// data was inserted quickly and periodicly in two hours, which has 'flood
// peak', then slowly.
// Y[i, j] = F(Y[i, j - i], X[i, j])
// Y[i, 0] = X[i, 0]

#include <queue>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <iostream>

template <class Tx, class Ty> class SBase {
public:
	using F = std::function<Ty(const Ty &, const Tx &)>;

private:
	using umap_dta = std::unordered_map<int,
		std::priority_queue<Tx, std::vector<Tx>, std::greater<Tx> > >;
	using umap_y = std::unordered_map<int, Ty>;

public:
	SBase(F f)
		: f(f)
	{
	}

	~SBase() {}

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
		if (!qr.empty()) {
			return false;
		}
		auto it = std::find_if(hash_pl.begin(), hash_pl.end(),
			[](const umap_dta::mapped_type &pl) { return !pl.empty(); });
		return it == hash_pl.end();
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
	umap_dta hash_pl;
	std::queue<Ty> qr;
	F f;
	umap_y hash_ymax;
};


template <class Tx, class Ty> class S2 {
public:
	using F = typename SBase<Tx, Ty>::F;

public:
	S2(F f)
		: f(f)
		, n(std::thread::hardware_concurrency())
		, bstop(false)
	{
		for (unsigned int i = 0; i < n; ++i) {
			vthread.push_back(std::thread([this, i]() {
				int inum = i;
				SBase<Tx, Ty> sbase(this->f);
				while (!this->bstop.load(std::memory_order_acquire)) {
					Tx x;
					{

						std::unique_lock<std::mutex> ulk(this->mtx);
						cv.wait(ulk, [this, inum]() {
							return this->bstop.load(
									   std::memory_order_acquire) ||
								   (this->curx.i != -1 &&
									   this->curx.i % this->n == inum);
						});
					}
					if (this->bstop.load(std::memory_order_acquire))
						return;
					x = curx;
					curx.i = -1;
					cv.notify_all();
					sbase.pushl(x);
				}
			}));
		}
	}


	~S2()
	{
		bstop.store(true, std::memory_order_release);
		cv.notify_all();
		for (auto &t : vthread) {
			if (t.joinable()) {
				t.join();
			}
		}
	};

	void
	pushl(const Tx &x)
	{
		{
			std::unique_lock<std::mutex> ulk(mtx);
			curx = x;

		}

		cv.notify_all();
		{
			std::unique_lock<std::mutex> ulk(mtx);
			cv.wait(ulk, [this]() {
				return this->bstop.load(std::memory_order_acquire) ||
					   this->curx.i == -1;
			});

		};
	}


private:
	unsigned int n;
	F f;
	std::vector<std::thread> vthread;
	std::atomic_bool bstop;
	std::condition_variable cv;
	std::mutex mtx;

	Tx curx;
};