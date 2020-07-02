using uint = unsigned int;
#ifndef UTILITIES_H
#define UTILITIES_H

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

	out << (abs(x) < 100 ? "  " : "") << (abs(x) < 10 ? "  " : "");
	out << (x < 0 ? "– " : "+ ");
    out << std::fixed << abs(x);

    return out.str();
}
#endif//UTILITIES_H