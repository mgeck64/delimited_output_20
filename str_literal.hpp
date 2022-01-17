#ifndef STR_LITERAL_HPP
#define STR_LITERAL_HPP

#include <utility>
#include <string>

// Helper struct and function for converting normal char string literals to
// strings of other char types (e.g., wchar_t) at compile-time. Solves a problem
// where we want to assign a string literal to a string type with a
// parameterized char type (see usage example for clarification).

namespace delimited_output::detail {

template <typename CharT, typename Traits, std::size_t Size>
struct str_literal {
    constexpr const CharT* data() const {return cdata;}
    constexpr const CharT* c_str() const {return cdata;}
    constexpr std::size_t size() const {return Size;}
    constexpr std::basic_string_view<CharT, Traits> view() const {return {cdata, Size};}
    // extend as needed

    const CharT cdata[Size + 1]; // +1 for null-terminator
    // Note: want to make cdata private but can't figure out how to make that
    // work with str_literal_cast; even making str_literal_cast a friend doesn't
    // work.
};

template <typename CharT, typename Traits = std::char_traits<CharT>, std::size_t N, typename Indices = std::make_index_sequence<N - 1>>
constexpr auto str_literal_cast(const char(&a)[N]) -> str_literal<CharT, Traits, N - 1> {
    static_assert(N > 0);
    auto helper = []<std::size_t... I>
            (const char(&a)[N], std::index_sequence<I...>) -> str_literal<CharT, Traits, N - 1> {
        return {a[I]..., 0};
    };
    return helper(a, Indices{});
}

// Usage:
//
// template <typename CharT>
// struct S {
//     static constexpr auto str = str_literal_cast<CharT>("abc");
// };

} // namespace delimited_output::detail

#endif // STR_LITERAL_HPP
