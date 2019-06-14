#ifndef RNG_H
#define RNG_H

#include <random>

// Pseudolosowy generator liczb całkowitych (np. indeksów wierzchołków)
// oraz rzeczywistych z zakresu [0, 1)

class RNG {
	private:
		const int max_int;
		std::default_random_engine generator;
		std::uniform_int_distribution<int>     unif_dist_integers;
		std::uniform_real_distribution<double> unif_dist_doubles;
	public:
		RNG(int max=1) : max_int(max), unif_dist_integers(0, max), unif_dist_doubles(0.0, 1.0) { }
		int  getMaxInt(void) const { return max_int; }
		int  randomInt(void)    { return unif_dist_integers(generator); }
		int  randomFrom0To1(void) { return unif_dist_doubles(generator); }
};

#endif // RNG_H
