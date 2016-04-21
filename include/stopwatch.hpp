#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>

struct Stopwatch
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;

	Stopwatch() {
		start();
	}

	void start() {
		start_ = std::chrono::high_resolution_clock::now();
	}

	int64_t ms() const {
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
	}

	float ms_float() const {
		auto end = std::chrono::high_resolution_clock::now();
		auto us_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
    return ((float)us_count)/1000.0f;
	}
};

#endif
