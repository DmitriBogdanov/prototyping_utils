// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/UTL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::enum_reflect
// Documentation: https://github.com/DmitriBogdanov/UTL/blob/master/docs/module_enum_reflect.md
// Source repo:   https://github.com/DmitriBogdanov/UTL
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_ENUM_REFLECT)
#ifndef UTLHEADERGUARD_ENUM_REFLECT
#define UTLHEADERGUARD_ENUM_REFLECT

// _______________________ INCLUDES _______________________

#include <array>       // array<>
#include <cstddef>     // size_t
#include <stdexcept>   // out_of_range
#include <string>      // string
#include <string_view> // string_view
#include <type_traits> // underlying_type_t<>, enable_if_t<>, is_enum_v<>
#include <utility>     // pair<>
#include <tuple>       // tuple_size_v<>

// ____________________ DEVELOPER DOCS ____________________

// Reflection mechanism is based entirely around the map macro and a single struct with partial specialization for the
// reflected enum. Map macro itself is quire non-trivial, but completely standard, a good explanation of how it works
// can be found here: [https://github.com/swansontec/map-macro].
//
// Once we have a map macro all reflection is a matter of simply mapping __VA_ARGS__ into a few "metadata"
// arrays which we will then traverse to perform string conversions.
//
// Partial specialization allows for a pretty concise implementation and provides nice error messages due to
// static_assert on incorrect template arguments.
//
// An alternative frequently used way to do enum reflection is through constexpr parsing of strings returned by
// compiler-specific '__PRETTY_FUNCTION__' and '__FUNCSIG__', it has a benefit of not requiring the reflection
// macro however it hammers compile times and improses restrictions on enum values. Some issues such as binary
// bloat and bitflag-enums can be worked around through proper implementation and some conditional metadata
// templates, however such approach seems to be quite complex and unreliable (due to being compiler-specific),
// better leave it to continuously supported libs like 'magic_enum'.

// ____________________ IMPLEMENTATION ____________________

namespace utl::enum_reflect {

// =================
// --- Map macro ---
// =================

#define utl_erfl_eval_0(...) __VA_ARGS__
#define utl_erfl_eval_1(...) utl_erfl_eval_0(utl_erfl_eval_0(utl_erfl_eval_0(__VA_ARGS__)))
#define utl_erfl_eval_2(...) utl_erfl_eval_1(utl_erfl_eval_1(utl_erfl_eval_1(__VA_ARGS__)))
#define utl_erfl_eval_3(...) utl_erfl_eval_2(utl_erfl_eval_2(utl_erfl_eval_2(__VA_ARGS__)))
#define utl_erfl_eval_4(...) utl_erfl_eval_3(utl_erfl_eval_3(utl_erfl_eval_3(__VA_ARGS__)))
#define utl_erfl_eval(...) utl_erfl_eval_4(utl_erfl_eval_4(utl_erfl_eval_4(__VA_ARGS__)))

#define utl_erfl_map_end(...)
#define utl_erfl_map_out
#define utl_erfl_map_comma ,

#define utl_erfl_map_get_end_2() 0, utl_erfl_map_end
#define utl_erfl_map_get_end_1(...) utl_erfl_map_get_end_2
#define utl_erfl_map_get_end(...) utl_erfl_map_get_end_1
#define utl_erfl_map_next_0(test, next, ...) next utl_erfl_map_out
#define utl_erfl_map_next_1(test, next) utl_erfl_map_next_0(test, next, 0)
#define utl_erfl_map_next(test, next) utl_erfl_map_next_1(utl_erfl_map_get_end test, next)

#define utl_erfl_map_0(f, x, peek, ...) f(x) utl_erfl_map_next(peek, utl_erfl_map_1)(f, peek, __VA_ARGS__)
#define utl_erfl_map_1(f, x, peek, ...) f(x) utl_erfl_map_next(peek, utl_erfl_map_0)(f, peek, __VA_ARGS__)

#define utl_erfl_map_list_next_1(test, next) utl_erfl_map_next_0(test, utl_erfl_map_comma next, 0)
#define utl_erfl_map_list_next(test, next) utl_erfl_map_list_next_1(utl_erfl_map_get_end test, next)

#define utl_erfl_map_list_0(f, x, peek, ...)                                                                           \
    f(x) utl_erfl_map_list_next(peek, utl_erfl_map_list_1)(f, peek, __VA_ARGS__)
#define utl_erfl_map_list_1(f, x, peek, ...)                                                                           \
    f(x) utl_erfl_map_list_next(peek, utl_erfl_map_list_0)(f, peek, __VA_ARGS__)

// Applies the function macro 'f' to all '__VA_ARGS__'
#define utl_erfl_map(f, ...) utl_erfl_eval(utl_erfl_map_1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// Applies the function macro 'f' to to all '__VA_ARGS__' and inserts commas between the results
#define utl_erfl_map_list(f, ...) utl_erfl_eval(utl_erfl_map_list_1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// Note: 'erfl' is short for 'enum_reflect'

// =======================
// --- Enum reflection ---
// =======================

// --- Implementation ---
// ----------------------

template <class>
constexpr bool _always_false_v = false;

template <class E>
struct _meta {
    static_assert(_always_false_v<E>,
                  "Provided enum does not have a defined reflection. Use 'UTL_ENUM_REFLECT' macro to define one.");
    // makes instantiation of this template a compile-time error
};

// Helper macros for codegen
#define utl_erfl_make_value(arg_) type::arg_
#define utl_erfl_make_name(arg_) std::string_view(#arg_)
#define utl_erfl_make_entry(arg_)                                                                                      \
    std::pair { std::string_view(#arg_), type::arg_ }

#define UTL_ENUM_REFLECT(enum_name_, ...)                                                                              \
    template <>                                                                                                        \
    struct utl::enum_reflect::_meta<enum_name_> {                                                                      \
        using type            = enum_name_;                                                                            \
                                                                                                                       \
        constexpr static std::string_view type_name = #enum_name_;                                                     \
                                                                                                                       \
        constexpr static auto names   = std::array{utl_erfl_map_list(utl_erfl_make_name, __VA_ARGS__)};                \
        constexpr static auto values  = std::array{utl_erfl_map_list(utl_erfl_make_value, __VA_ARGS__)};               \
        constexpr static auto entries = std::array{utl_erfl_map_list(utl_erfl_make_entry, __VA_ARGS__)};               \
    }

// --- Public API ---
// ------------------

template <class E>
constexpr auto type_name = _meta<E>::type_name;

template <class E>
constexpr auto names = _meta<E>::names;

template <class E>
constexpr auto values = _meta<E>::values;

template <class E>
constexpr auto entries = _meta<E>::entries;

template <class E>
constexpr auto size = std::tuple_size_v<decltype(values<E>)>;

template <class E, std::enable_if_t<std::is_enum_v<E>, bool> = true>
[[nodiscard]] constexpr auto to_underlying(E value) noexcept {
    return static_cast<std::underlying_type_t<E>>(value);
    // doesn't really require reflection, but might as well have it here,
    // in C++23 gets replaced by builtin 'std::to_underlying'
}

template <class E>
[[nodiscard]] constexpr bool is_valid(E value) noexcept {
    for (const auto& e : values<E>)
        if (value == e) return true;
    return false;
}

template <class E>
[[nodiscard]] constexpr std::string_view to_string(E val) {
    for (const auto& [name, value] : entries<E>)
        if (val == value) return name;

    throw std::out_of_range("enum_reflect::to_string<" + std::string(type_name<E>) + ">(): value " +
                            std::to_string(to_underlying(val)) + " is not a part of enumeration.");
}

template <class E>
[[nodiscard]] constexpr E from_string(std::string_view str) {
    for (const auto& [name, value] : entries<E>)
        if (str == name) return value;

    throw std::out_of_range("enum_reflect::from_string<" + std::string(type_name<E>) + ">(): name \"" +
                            std::string(str) + "\" is not a part of enumeration.");
}

} // namespace utl::enum_reflect

#endif
#endif // module utl::enum_reflect
