#ifndef _DEBUG_HPP_
#define _DEBUG_HPP_
#include <iostream>

#ifndef DEBUG
#define DEBUG 1 // set debug mode
#endif

#if DEBUG
#define log(...)                                                                                                   \
    {                                                                                                              \
        char str[1000];                                                                                            \
        snprintf(str, 1000, __VA_ARGS__);                                                                          \
        std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "] " << str << std::endl; \
    }
#else
#define log(...)
#endif

template <typename T>
std::ostream &print(std::ostream &out, T const &val)
{
    return (out << val << " ");
}

template <typename T1, typename T2>
std::ostream &print(std::ostream &out, std::pair<T1, T2> const &val)
{
    return (out << "{" << val.first << " " << val.second << "} ");
}

template <template <typename, typename...> class TT, typename... Args>
std::ostream &operator<<(std::ostream &out, TT<Args...> const &cont)
{
    for (auto &&elem : cont)
        print(out, elem);
    return out;
}

#endif