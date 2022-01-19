#ifndef STR_LITERAL_HPP
#define STR_LITERAL_HPP

#include <utility>
#include <string>

// Struct and function for converting a c-style string literal to a c-style
// string literal of a parameterized character type at compile-time.
// (See usage example below for clarification.)
//
// This is adapted from Method 3 described here:
// https://stackoverflow.com/questions/52737760/how-to-define-string-literal-with-character-type-that-depends-on-template-parame

namespace delimited_output {

template <typename CharT, std::size_t Capacity>
class str_literal {
    CharT data_[Capacity]; // includes space for null-terminator

public:
    static_assert(Capacity > 0);

    constexpr const CharT* data() const {return data_;}
    constexpr const CharT* c_str() const {return data_;}
    constexpr std::size_t size() const {return Capacity - 1;} // exclude null-terminator
    constexpr CharT operator[](std::size_t i) const {return data_[i];}
    constexpr CharT const* begin() const {return data_;}
    constexpr CharT const* end() const {return data_ + size();} // end is at null-terminator

    template <typename Traits = std::char_traits<CharT>>
    constexpr std::basic_string_view<CharT, Traits> view() const {return {data_, size()};}
    // extend as needed

    template <typename SrcCharT>
    constexpr str_literal(SrcCharT(&src)[Capacity]) {
        auto p_data = data_;
        for (auto c : src) {
            if (c < 0 || c > 127)
                // Assume Unicode encoding; restrict source characters to common
                // ASCII subset so they can simply be copied.
                throw std::out_of_range("Value in ASCII range (0...127) was expected");
            if (p_data == end() && c != 0)
                throw std::invalid_argument("Null-terminated string was expected");
            *p_data++ = c;
        }
    }
};

template <typename DstCharT, typename SrcCharT, std::size_t Capacity>
constexpr auto str_literal_cast(const SrcCharT(&src)[Capacity]) -> auto {
    return str_literal<DstCharT, Capacity>(src);
}


// Usage:
//
// template <typename CharT>
// struct S {
//     static constexpr auto str = str_literal_cast<CharT>("abc");
// };

} // namespace delimited_output

#endif // STR_LITERAL_HPP
