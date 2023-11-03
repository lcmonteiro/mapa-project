#pragma once
#include <type_traits>
#include <functional>

namespace traits
{
template <typename T>
struct result
{
	using type = T;
};
// ==============================================
// invoke result
// ==============================================
namespace invoke_result
{
template <typename>
struct resolve;

// handle function pointers
template <typename R, typename... A>
struct resolve<R (*)(A...)> : result<R>
{
};
// handle member functions
template <typename T, typename R, typename... A>
struct resolve<R (T::*)(A...) const> : result<R>
{
};
template <typename T, typename R, typename... A>
struct resolve<R (T::*)(A...)> : result<R>
{
};
// handle function signatures
template <typename R, typename... A>
struct resolve<R(A...)> : result<R>
{
};
// handle callable object
template <typename T>
struct resolve : resolve<decltype(&T::operator())>
{
};
} // namespace invoke_result
template <typename T>
using invoke_result_t =
	typename invoke_result::resolve<T>::type;

// ==============================================
// invoke signature
// ==============================================
namespace invoke_signature
{
template <typename>
struct resolve;
// handle function pointers
template <typename R, typename... A>
struct resolve<R (*)(A...)> : result<R(A...)>
{
};
// handle member functions
template <typename T, typename R, typename... A>
struct resolve<R (T::*)(A...) const> : result<R(A...)>
{
};
template <typename T, typename R, typename... A>
struct resolve<R (T::*)(A...)> : result<R(A...)>
{
};
// handle std functions
template <typename R, typename... A>
struct resolve<std::function<R(A...)>> : result<R(A...)>
{
};
// handle functions signature
template <typename R, typename... A>
struct resolve<R(A...)> : result<R(A...)>
{
};

// handle callable object
template <typename T>
struct resolve : resolve<decltype(&T::operator())>
{
};

} // namespace invoke_signature

template <typename T>
using invoke_signature_t =
	typename invoke_signature::resolve<T>::type;

// ==============================================
// invoke signature r
// ==============================================
namespace invoke_signature_r
{
template <typename R, typename T>
struct resolve : resolve<R, invoke_signature_t<T>>
{
};
// replace result
template <typename T, typename R, typename... A>
struct resolve<T, R(A...)> : result<T(A...)>
{
};

} // namespace invoke_signature_r
template <typename R, typename T>
using invoke_signature_r_t =
	typename invoke_signature_r::resolve<R, T>::type;

// ==============================================
// last
// ==============================================
namespace last
{
template <typename... Ts>
struct resolve
	: result<decltype((std::declval<Ts>(), ...))>
{
};
} // namespace last
template <typename... Ts>
using last_t =
	typename last::resolve<Ts...>::type;
} // namespace traits