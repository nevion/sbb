#ifndef SBB_VECTOR_H
#define SBB_VECTOR_H

#include <vector>
#include <utility>

template<class T, class... Tail>
std::vector<T> make_vector(T&& head, Tail&&... values)
{
  return {{ std::forward<T>(head), std::forward<Tail>(values)... }};
}


#endif // SBB_VECTOR_H
