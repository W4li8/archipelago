using uint = unsigned int;
#ifndef UTILITIES_H
#define UTILITIES_H

#include <algorithm>
using uint = unsigned int;


#undef MIN
#define MIN(a, b) ( ((a) < (b)) ? (a) : (b) )
#undef MAX
#define MAX(a, b) ( ((a) > (b)) ? (a) : (b) )

#undef CLAMP
#define CLAMP(x, a, b) ( MAX(a, MIN(x, b)) )

#undef ABS
#define ABS(x)	( ((x) < 0) ? -(x) : (x))
#undef SIGN
#define SIGN(x) ( ((x) < 0) ? -1 : ((x) > 0) ? 1 : 0 )

#define BOUNDED(x, a, b) ( (a) <= (x) && (x) <= (b)) )

#define watch(x) (#x) // return strin name


template<typename T> auto min(T&& val) { return std::forward<T>(val); }
template<typename T0, typename T1, typename... Ts> auto min(T0&& a, T1&& b, Ts&&... vs) {
    return min((a < b) ? a : b, std::forward<Ts>(vs)...);
}
template<typename T> auto max(T&& val) { return std::forward<T>(val); }
template<typename T0, typename T1, typename... Ts> auto max(T0&& a, T1&& b, Ts&&... vs) {
    return max((a > b) ? a : b, std::forward<Ts>(vs)...);
}
#include <string>
#include <sstream>

inline std::string StripPathFromfilepath(std::string filepath) {
	for(auto it = find(filepath.begin(), filepath.end(), '/'); it < filepath.end();
        it = find(filepath.begin(), filepath.end(), '/')) {
		filepath.erase(filepath.begin(), it+1); // strip directory
	}
    filepath.erase(find(filepath.begin(), filepath.end(), '.'), filepath.end()); // strip extension
	return filepath;
}


inline std::string bsfn(double x) {
	std::ostringstream out;
	out.precision(2);

	// out << (abs(x) < 100 ? "  " : "") << (abs(x) < 10 ? "  " : "");
	out << (x < 0 ? "– " : "+ ");
    out << std::fixed << abs(x);

    return out.str();
}
template<typename T>
std::string str(T x) { return std::to_string(x) + " "; }
// std::string str(int x) { return std::to_string(x) + " "; }
// std::string str(uint x) { return std::to_string(x) + " "; }
// std::string str(float x) { return bsfn(x) + " "; }
// std::string str(double x) { return bsfn(x) + " "; }
// std::string str(bool x) { return x ? "1 " : "0 "; }
#define EPS 5
#define S_SPEED 1.0f
#define F_SPEED 2.0f

#define PRINT(x) std::cout << #x << std::endl;
#define fnDEBUG std::cout << __PRETTY_FUNCTION__ << std::endl;
#define fiDEBUG std::cout <<"IN "<< __FUNCTION__ << std::endl;
#define foDEBUG std::cout <<"OUT "<< __FUNCTION__ << std::endl;
#define lDEBUG std::cout << __LINE__ << std::endl;
#define DEBUG(msg) std::cout <<__LINE__<< msg << std::endl;


#endif//UTILITIES_H