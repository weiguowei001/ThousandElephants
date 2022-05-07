#include "first/s1.hpp"
#include <random>
#include <chrono>
#include <iostream>

struct Data {
	int i; // data source   5000
	int j; // sequece number 100000~500000
		   // data
	bool
	operator>(const Data &other) const
	{
		return j > other.j;
	}
};

typedef Data X;
typedef Data Y;

int
main()
{
	std::vector<X> vl;
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 500000; ++j) {
			X x;
			x.i = i;
			x.j = j;
			vl.emplace_back(x);
		}
	}
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vl.begin(), vl.end(), g);

	std::queue<X> ql;
	for (const auto &x : vl) {
		ql.push(std::move(x));
	}
	std::queue<Y> qr;


	S<X, Y>::F f = [](const Y &y, const X &x) -> Y {
		for (int i = 0; i < 10000; ++i)
			;
		Y r = y;
		r.j++;
		return r;
	};
	S<X, Y> s(f);


	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	while (!ql.empty()) {
		s.pushl(ql.front());
		ql.pop();
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[mill]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[micro]" << std::endl;

	s.emptyr();
	return 0;
}