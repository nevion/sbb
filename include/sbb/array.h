#ifndef SBB_ARRAY_H
#define SBB_ARRAY_H

#include <array>
#include <utility>

template<class T, class... Tail>
std::array<T,1+sizeof...(Tail)> make_array(T&& head, Tail&&... values)
{
  return {{ std::forward<T>(head), std::forward<Tail>(values)... }};
}

#endif // SBB_ARRAY_H
