#ifndef SBB_TYPE_TRAITS_H
#define SBB_TYPE_TRAITS_H
#include <type_traits>
#include <array>
#include <vector>
#include <tuple>
#include <memory>


template<typename T, typename Head, typename... Args>
struct type_in : std::integral_constant<bool, std::is_same<T, Head>::value || type_in<T, Args...>::value>{};
template<typename T, typename Head>
struct type_in<T, Head> : std::integral_constant<bool, std::is_same<T, Head>::value>{};

template <typename T, template <typename...> class Template>
struct is_specialization_of : std::false_type{};
template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type{};

template<typename T>
struct is_first_class_enum : std::is_base_of< ::detail::enum_tag, T>{};

//here we consider first class arrays only
//fixed size type trait, user pluggable
template<typename T, class Enable = void>
struct is_enum : std::false_type{};
template<class T>
struct is_enum<T, typename std::enable_if< ::is_enum<T>::value>::type> : std::true_type{};
template<class T>
struct is_enum<T, typename std::enable_if< ::is_first_class_enum<T>::value>::type> : std::true_type{};

//here we consider first class arrays only
template<class T>
struct is_array : std::false_type {};
template<class T, std::size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

template<typename T>
struct is_tuple : std::integral_constant<bool, is_specialization_of<T, std::tuple>::value>{};

template<typename T>
struct has_const_iterator{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template<typename T>
struct is_sequence : has_const_iterator<T>{};

template<class T> struct is_string : std::is_same<T, std::string>{};

template<typename T> struct is_vector : std::false_type{};
template<typename... Ts> struct is_vector<std::vector<Ts...>> : std::true_type{};

//here we consider both first class and second as a hidden convinience
template<typename T> struct array_length;
template<typename T, size_t N>
struct array_length<std::array<T, N>> : std::integral_constant<size_t, N>{};
template<typename T, size_t N>
struct array_length<T[N]> : std::integral_constant<size_t, N>{};

//fixed size type trait, user pluggable
template<typename T, class Enable = void>
struct is_fixed_size : std::false_type{};
template<class T>
struct is_fixed_size<T, typename std::enable_if< ::is_array<T>::value && ::is_fixed_size<typename T::value_type>::value>::type> : std::true_type{};
template<class T>
struct is_fixed_size<T, typename std::enable_if<std::is_trivial<T>::value>::type> : std::true_type{};

namespace detail{
    template<typename... Args>
    struct tuple_fixed_check_recursive{
        static const bool value = false;
    };
    template<typename Head>
    struct tuple_fixed_check_recursive<Head>{
        static const bool value = is_fixed_size<Head>::value;
    };
    template<typename Head, typename... Args>
    struct tuple_fixed_check_recursive<Head, Args...>{
        static const bool value = is_fixed_size<Head>::value || tuple_fixed_check_recursive<Args...>::value;
    };

    template<typename... Args>
    struct tuple_trivial_check_recursive{
        static const bool value = false;
    };
    template<typename Head>
    struct tuple_trivial_check_recursive<Head>{
        static const bool value = std::is_trivial<Head>::value;
    };
    template<typename Head, typename... Args>
    struct tuple_trivial_check_recursive<Head, Args...>{
        static const bool value = std::is_trivial<Head>::value || tuple_trivial_check_recursive<Args...>::value;
    };
}

template<class T, typename... Args>
struct is_fixed_tuple : std::integral_constant<bool, std::is_same<T, std::tuple<Args...>>::value && detail::tuple_fixed_check_recursive<Args...>::value>{};

template<class T>
struct is_fixed_size<T, typename std::enable_if< ::is_fixed_tuple<T>::value>::type> : std::true_type{};

template<class T, typename... Args>
struct is_trivial_tuple : std::integral_constant<bool, std::is_same<T, std::tuple<Args...>>::value && detail::tuple_trivial_check_recursive<Args...>::value>{};

template<typename T, class Enable = void>
struct is_trivial : std::false_type{};
template<class T>
struct is_trivial<T, typename std::enable_if<std::is_trivial<T>::value || ::is_trivial_tuple<T>::value>::type > : std::true_type{};

template<typename... Ts> struct is_contiguous_sequence : std::integral_constant<bool,
    ::is_array<Ts...>::value || is_vector<Ts...>::value || is_string<Ts...>::value>{};

template<typename T>
struct has_data_method
{
private:
    typedef std::true_type yes;
    typedef std::false_type no;

    template<typename U> static auto test(int) -> decltype(std::declval<U>().data(), yes());

    template<typename> static no test(...);

public:

    static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
};

template<typename T>
struct has_size_method
{
private:
    typedef std::true_type yes;
    typedef std::false_type no;

    template<typename U> static auto test(int) -> decltype(std::declval<U>().size(), yes());

    template<typename> static no test(...);

public:

    static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
};

template<typename T, typename Enable = void>
struct is_sequence_of_fixed : std::false_type{};
template<typename T>
struct is_sequence_of_fixed<T, typename std::enable_if< ::is_contiguous_sequence<T>::value && is_fixed_size<typename T::value_type>::value>::type>: std::true_type{};

template<typename T>
struct is_map{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::key_type*, typename C::value_type*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0, 0)) == sizeof(yes);
    typedef T type;
};

namespace detail{
    template<class T> struct is_shared_helper     : std::false_type {};
    template<class T> struct is_shared_helper<std::shared_ptr<T>> : std::true_type {};

    template<class T> struct is_dereferencable_helper     : std::false_type {};
    template<class T> struct is_dereferencable_helper<std::shared_ptr<T>> : std::true_type {};
    template<class T> struct is_dereferencable_helper<T*> : std::true_type {};
}

template<typename T>
struct is_shared_ptr : std::false_type{};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>>:std::true_type{};

template<class T> struct is_dereferencable : detail::is_dereferencable_helper<typename std::remove_cv<T>::type> {};

template<typename... Args>
struct sizeof_unroller : std::integral_constant<size_t, 0>{};
template<typename Head>
struct sizeof_unroller<Head> : std::integral_constant<size_t, sizeof(Head)>{};
template<typename Head, typename... Args>
struct sizeof_unroller<Head, Args...> : std::integral_constant<size_t, sizeof_unroller<Head>::value + sizeof_unroller<Args...>::value>{};

template<typename... Args>
constexpr size_t sizeof_args(Args&&... args){
    return sizeof_unroller<decltype(args)...>::value;
}

namespace detail{
    template<size_t A, size_t B>
    struct max2 : std::integral_constant<size_t, (A > B) ? A : B>{};
}

template<typename... Args>
struct max_sizeof_amoung : std::integral_constant<size_t, 0>{};
template<typename Head>
struct max_sizeof_amoung<Head> : std::integral_constant<size_t, sizeof(Head)>{};
template<typename Head, typename... Args>
struct max_sizeof_amoung<Head, Args...> : std::integral_constant<size_t, detail::max2<sizeof(Head), max_sizeof_amoung<Args...>::value>::value>{};
template<typename... Args>
constexpr size_t max_sizeof_amoung_args(Args&&... args){
    return max_sizeof_amoung<decltype(args)...>::value;
}


#endif // SBB_TYPE_TRAITS_H
