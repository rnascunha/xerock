#ifndef UTILITY_TRAITS_TYPES_HPP__
#define UTILITY_TRAITS_TYPES_HPP__

#include <type_traits>

#include <chrono>
#include <memory>

namespace my_traits{

/**
 * Empty type
 */
struct empty{};

/**
 * Check if is a chrono duration type
 */
template <typename T>
struct is_chrono_duration
{
    static constexpr bool value = false;
};

template <typename Rep, typename Period>
struct is_chrono_duration<std::chrono::duration<Rep, Period>>
{
    static constexpr bool value = true;
};

/**
 * Check if class inheritance from std::enable_shared_from_this
 */
template<class T>
using is_enable_shared_from_this = std::is_base_of<std::enable_shared_from_this<T>, T>;

/**
 * Exclude type if false
 */
template<bool V, typename T>
using exclude_attr_if = typename std::conditional<V == true, T, empty>::type;



template< class ... > using void_t = void;

/**
 * Check is a class Session is shareable (has shareable member)
 */
template< class , class = void >
struct is_shareable : std::false_type
{ };

// specialized as has_member< T , void > or discarded (sfinae)
template< class T >
struct is_shareable< T , void_t< decltype( T::is_shareable ) > >
{
	static constexpr const bool value = T::is_shareable;
};

}//my_traits

#endif /* UTILITY_TRAITS_TYPES_HPP__ */
