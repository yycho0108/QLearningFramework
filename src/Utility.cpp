#include "Utility.h"
#include <functional>
#include <random>
#include <ctime>

auto _f = std::bind(std::uniform_real_distribution<double>(0.0,1.0),std::default_random_engine(time(0)));

double f(){
	return _f();
}
