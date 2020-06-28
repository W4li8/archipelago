using uint = unsigned int;
#ifndef UTILITIES_H
#define UTILITIES_H

template<typename T> auto min(T&& val) { return std::forward<T>(val); }
template<typename T0, typename T1, typename... Ts> auto min(T0&& a, T1&& b, Ts&&... vs) {
    return min((a < b) ? a : b, std::forward<Ts>(vs)...);
}
template<typename T> auto max(T&& val) { return std::forward<T>(val); }
template<typename T0, typename T1, typename... Ts> auto max(T0&& a, T1&& b, Ts&&... vs) {
    return max((a > b) ? a : b, std::forward<Ts>(vs)...);
}

#endif//UTILITIES_H