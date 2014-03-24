#ifndef SBB_ENUMERATION_H
#define SBB_ENUMERATION_H
#include <type_traits>
#include <typeinfo>
#include <boost/lexical_cast.hpp>

namespace detail{
    struct enum_tag{};
}

#define declare_enum(x) struct x : ::detail::enum_tag

#define _enum_common_class_body(struct_name)                                       \
    type value;                                                                    \
    typedef typename std::underlying_type<type>::type integral_type;               \
    inline bool operator==(const type &value) const{                               \
        return this->value == value;                                               \
    }                                                                              \
    inline bool operator==(const struct_name &rhs) const{                          \
        return this->value == rhs.value;                                           \
    }                                                                              \
    inline operator integral_type() const{                                         \
        return (integral_type) value;                                              \
    }                                                                              \
    inline static size_t index_of(const std::string &v){                           \
        for(size_t i = 0; i < size(); ++i){                                        \
            if(v == string_at(i)){                                                 \
                return i;                                                          \
            }                                                                      \
        }                                                                          \
        return size();                                                             \
    }                                                                              \
    inline size_t index_of() const {                                               \
        return index_of(value);                                                    \
    }                                                                              \
    inline bool is_valid() const {                                                 \
        return is_valid((integral_type) value);                                    \
    }                                                                              \
    inline static bool is_valid(integral_type v){                                  \
        return index_of(v) != size();                                              \
    }                                                                              \
    inline static bool is_valid(const std::string &v){                             \
        return index_of(v) != size();                                              \
    }                                                                              \
    inline static struct_name from_string(const std::string &v){                   \
        const size_t index = index_of(v);                                          \
        if(index == size()){                                                       \
            throw std::bad_cast();                                                 \
        }                                                                          \
        return value_at(index);                                                    \
    }                                                                              \
    inline const char* to_string() const{                                          \
        const size_t index = index_of();                                           \
        if(index != size()){                                                       \
            return string_at(index);                                               \
        }                                                                          \
        return "__invalid__";                                                      \
    }                                                                              \

#define enum_class_body(struct_name)                                               \
    static const char *str[];                                                      \
    _enum_common_class_body(struct_name)                                           \
    inline struct_name() : value(MAX){}                                            \
    inline struct_name(const type &value) : value(value){}                         \
    inline struct_name(const integral_type &value):value((type) value){}           \
    inline static constexpr size_t size(){                                         \
        return (size_t)((integral_type) MAX);                                      \
    }                                                                              \
    inline static const char* string_at(const size_t index){                       \
        return str[index];                                                         \
    }                                                                              \
    inline static type value_at(const size_t index){                               \
        return (type) index;                                                       \
    }                                                                              \
    inline static size_t index_of(integral_type x){                                \
        return x >= 0 && x < MAX ? (size_t) x : size();                            \
    }                                                                              \


#define enum_general_class_body(struct_name)                                        \
    _enum_common_class_body(struct_name)                                            \
    inline struct_name() : value(){}                                                \
    inline struct_name(const type &value) : value(value){}                          \
    inline struct_name(const integral_type &value):value((type) value){}            \
    inline static size_t index_of(integral_type x){                                 \
        for(size_t i = 0; i < size(); ++i){                                         \
            if(value_at(i) == x){                                                   \
                return i;                                                           \
            }                                                                       \
        }                                                                           \
        return size();                                                              \
    }                                                                               \
    static size_t size();                                                           \
    static const char* string_at(const size_t index);                               \
    static type value_at(const size_t index);                                       \


#define declare_enum_to_string(fcenum) inline std::string to_string(const fcenum &v){ return v .to_string(); }
#define declare_enum_stream_ops(fcenum)                                              \
inline std::ostream& operator<<(std::ostream& os, const fcenum &v){                  \
    os<<v.to_string();                                                               \
    return os;                                                                       \
}                                                                                    \
inline std::istream& operator>>(std::istream& is, fcenum &v){                                 \
    std::string s;                                                                            \
    is>>s;                                                                                    \
    size_t index_of = fcenum::index_of(s);                                                    \
    if(index_of == fcenum::size()){                                                           \
        fcenum::integral_type integral_value = boost::lexical_cast<fcenum::integral_type>(s); \
        index_of = fcenum::index_of(integral_value);                                          \
    }                                                                                         \
    v = fcenum::value_at(index_of);                                                           \
    return is;                                                                                \
}                                                                                             \

#define declare_enum_helpers(fcenum)                                                 \
declare_enum_to_string(fcenum)                                                       \
declare_enum_stream_ops(fcenum)                                                      \

#define enum_str_decl(struct_name)                         \
    const char *struct_name::str[struct_name::size()] =

#endif // SBB_ENUMERATION_H
