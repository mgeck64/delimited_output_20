#ifndef DELIMITED_OUTPUT_HPP
#define DELIMITED_OUTPUT_HPP

#include <ostream>
#include <tuple>
#include <type_traits>
#include <cassert>
#include "str_literal.hpp"

namespace delimited_output {

// delimited():

// delimited() can be used to output a range (container-like object) as
// delimited text; for example:
//    auto arr = std::array{1, 3, 5, 7, 9};
//    cout << delimited(arr);
// outputs:
//    1, 3, 5, 7, 9
// However, strings (which are char ranges) as an exception are output normally,
// so:
//    cout << delimited(std::string{"Hello"})
// outputs:
//    Hello

// delimited() can also be used to output a sequence delimited by a pair of
// iterators as delimited text; for example:
//    cout << delimited(arr.begin() + 1, arr.end() - 1)
// outputs:
//    3, 5, 7

// delimited() can also be used to output a pair or a tuple; for example:
//    cout << delimited(std::pair{1, "One"})
// outputs:
//    1: One
// and:
//    cout << delimited(std::tuple{1, "Two", 3})
// outputs:
//    1, Two, 3

// By default, delimited() will output any other type of object for which a
// stream insertion operator (operator<<) is defined, in which case it will be
// output via that operator.

// The logic for the above behavior is applied recursively, so, for example, if
// delimited() is given a map, the pair elements contained therein will be
// output as described, with the addition that for such elements, pair elements
// will be enclosed in square brackets and range and tuple elements will be
// enclosed in parentheses; see delimiters below for details.

// Delimiter values can be specified via setter functions; for example:
//     cout << delimited(arr).delimiter(" - ").empty("Empty")
// Delimiter values can also be specified via a delimiters object. The complete
// set of delimiter values is in delimiters below and the complete set of setter
// functions is in delimiter_inserter below.

// wdelimited() is also provided for wide char (wchar_t) streams (with default
// char traits). wdelimited() works just like delimited(). delimited() can also
// be parameterized to work with wide char streams.

// delimited() or wdelimited() returns a helper object that stores a reference
// or iterator pair to the object or sequence to output, and thus is valid for
// so long as the reference or iterator pair is valid. Note: an expression such
// as
//    std::cout << delimited(std::string("Hello"))
// is OK as the helper object will be valid for the duration of the expression.

template <typename T>
concept iterator = std::input_or_output_iterator<T>;

template <typename, typename CharT, typename Traits = std::char_traits<CharT>> class basic_inserter;
template <iterator, typename CharT, typename Traits = std::char_traits<CharT>> class basic_sequence_inserter;
template <typename, typename> struct basic_delimiters;

template <typename CharT = char, typename Traits = std::char_traits<CharT>, typename Object>
inline auto delimited(const Object& obj)
{return basic_inserter<Object, CharT, Traits>{obj};}

template <typename CharT, typename Traits, typename Object>
inline auto delimited(const Object& obj, const basic_delimiters<CharT, Traits>& delims)
{return basic_inserter<Object, CharT, Traits>{obj, delims};}

template <typename CharT = char, typename Traits = std::char_traits<CharT>, iterator Iterator>
inline auto delimited(Iterator begin, Iterator end)
{return basic_sequence_inserter<Iterator, CharT, Traits>{begin, end};}

template <typename CharT, typename Traits, iterator Iterator>
inline auto delimited(Iterator begin, Iterator end, const basic_delimiters<CharT, Traits>& delims)
{return basic_sequence_inserter<Iterator, CharT, Traits>{begin, end, delims};}

template <typename Object>
inline auto wdelimited(const Object& obj)
{return basic_inserter<Object, wchar_t>{obj};}

template <iterator Iterator>
inline auto wdelimited(Iterator begin, Iterator end)
{return basic_sequence_inserter<Iterator, wchar_t>{begin, end};}

// note: delimited() and wdelimited() are technically redundant but convenient
// shorthand for instantiating basic_inserter and basic_sequence_inserter for
// common cases

// basic_delimiters, delimiters, wdelimiters:

template <typename CharT, typename Traits = std::char_traits<CharT>>
struct basic_delimiters { // delimiters and related values
    // herein, "collection" refers to a sequence or collection of elements such
    // as in a container, sequence, tuple or pair

    static constexpr auto base_delim_default = detail::str_literal_cast<CharT>(", ");
    static constexpr auto sub_prefix_default = detail::str_literal_cast<CharT>("(");
    static constexpr auto sub_delim_default = detail::str_literal_cast<CharT>(", ");
    static constexpr auto sub_suffix_default = detail::str_literal_cast<CharT>(")");
    static constexpr auto pair_prefix_default = detail::str_literal_cast<CharT>("[");
    static constexpr auto pair_delim_default = detail::str_literal_cast<CharT>(": ");
    static constexpr auto pair_suffix_default = detail::str_literal_cast<CharT>("]");
    static constexpr auto empty_default = detail::str_literal_cast<CharT>("<empty>");

    using string_view = std::basic_string_view<CharT, Traits>;

    string_view base_delim = base_delim_default.view(); // base-level collection delimiter (except for pair)
    // example for tuple<int, string, int>: 1, Two, 3
    // example for container of ints: 10, 20, 30, 40, 50

    // values for recursively output sub-level collection (except for pair):
    string_view sub_prefix = sub_prefix_default.view(); // sub-level collection prefix
    string_view sub_delim = sub_delim_default.view(); // sub-level delimiter
    string_view sub_suffix = sub_suffix_default.view(); // sub-level collection suffix
    // example for container of tuples: (1, Two, 3), (4, Five, 6), (7, Eight, 9)

    // values for base-level and recursively output sub-level pair:
    string_view pair_prefix = pair_prefix_default.view(); // sub-level pair prefix
    string_view pair_delim = pair_delim_default.view(); // base- and sub-level pair delimiter
    string_view pair_suffix = pair_suffix_default.view(); // sub-level pair suffix
    // base-level example for pair<int, string>: 1: One
    // sub-level example for map<int, string>: [1: One], [2: Two], [3: Three]

    bool base_as_sub = false; // output base-level collection like sub-level one
    // true example for tuple<int, string, int>: (1, Two, 3)
    // true example for container of ints: (10, 20, 30, 40, 50)
    // true example for container of tuples: ((1, Two, 3), (4, Five, 6), (7, Eight, 9))
    // true example for pair<int, string>: [1: One]
    // true example for map<int, string>: ([1: One], [2: Two], [3: Three])
    // n/a for string or default output; for example, string("Hello") and
    // int(123) will be output normally regardless of this setting

    string_view empty = empty_default.view(); // text for empty object or empty sequence

    // note: delimiter stores string views and thus is valid for so long as
    // those string views are valid
};

using delimiters = basic_delimiters<char>;
using wdelimiters = basic_delimiters<wchar_t>;

// basic_inserter, inserter, winserter:

template <typename Object, typename CharT, typename Traits>
class basic_inserter {
    const Object& obj;
    basic_delimiters<CharT, Traits> delims;
public:
    basic_inserter(const Object& obj_)
        : obj{obj_} {}
    basic_inserter(const Object& obj_, const basic_delimiters<CharT, Traits>& delims_)
        : obj{obj_}, delims{delims_} {}

    // stream inserter:

    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& out, const basic_inserter<Object, CharT, Traits>& di)
    {output_helper(di.obj, di.delims, di.delims.base_as_sub, out); return out;}

    // value setters:
    // Each function return a reference to *this so calls can be chained; e.g.,
    // cout << delimited(container).delimiter(" - ").empty("Empty")

    using string_view = std::basic_string_view<CharT, Traits>;

    auto& delimiter(string_view str) // sets base_delim and sub_delim (but not pair_delim)
    {delims.base_delim = str; delims.sub_delim = str; return *this;}

    auto& base_delim(string_view str)
    {delims.base_delim = str; return *this;}

    auto& sub_prefix(string_view str)
    {delims.sub_prefix = str; return *this;}

    auto& sub_delim(string_view str)
    {delims.sub_delim = str; return *this;}

    auto& sub_suffix(string_view str)
    {delims.sub_suffix = str; return *this;}

    auto& pair_prefix(string_view str)
    {delims.pair_prefix = str; return *this;}

    auto& pair_delim(string_view str)
    {delims.pair_delim = str; return *this;}

    auto& pair_suffix(string_view str)
    {delims.pair_suffix = str; return *this;}

    auto& base_as_sub(bool b = true)
    {delims.base_as_sub = b; return *this;}

    auto& as_sub(bool b = true) // concise alternative; e.g.: delimited(arr).as_sub()
    {delims.base_as_sub = b; return *this;}

    auto& empty(string_view str)
    {delims.empty = str; return *this;}
};

template <typename Object> using inserter = basic_inserter<Object, char>;
template <typename Object> using winserter = basic_inserter<Object, wchar_t>;

// sequence, basic_sequence_inserter, sequence_inserter, sequence_winserter:

template <iterator Iterator>
struct sequence {
    Iterator begin_itr, end_itr;
    Iterator begin() const {return begin_itr;}
    Iterator end() const {return end_itr;}
};

template <iterator Iterator, typename CharT, typename Traits>
class basic_sequence_inserter: public basic_inserter<sequence<Iterator>, CharT, Traits> {
    sequence<Iterator> seq;
public:
    basic_sequence_inserter(Iterator begin, Iterator end)
        : basic_inserter<sequence<Iterator>, CharT, Traits>{seq} {seq.begin_itr = begin; seq.end_itr = end;}
    basic_sequence_inserter(Iterator begin, Iterator end, const basic_delimiters<CharT, Traits>& delims)
        : basic_inserter<sequence<Iterator>, CharT, Traits>{seq, delims} {seq.begin_itr = begin; seq.end_itr = end;}
};

template <iterator Iterator> using sequence_inserter = basic_sequence_inserter<Iterator, char>;
template <iterator Iterator> using sequence_winserter = basic_sequence_inserter<Iterator, wchar_t>;

// output helper functions:

// output a pair:

template <typename T1, typename T2, typename CharT, typename Traits>
void output_helper(const std::pair<T1, T2>& pair, const basic_delimiters<CharT, Traits>& delims, bool as_sub, std::basic_ostream<CharT, Traits>& out) {
    if (as_sub)
        out << delims.pair_prefix;
    output_helper(pair.first, delims, true, out);
    out << delims.pair_delim;
    output_helper(pair.second, delims, true, out);
    if (as_sub)
        out << delims.pair_suffix;
}

// output a tuple:

template<typename... Ts, typename CharT, typename Traits>
void output_helper(const std::tuple<Ts...>& tuple, const basic_delimiters<CharT, Traits>& delims, bool as_sub, std::basic_ostream<CharT, Traits>& out)
{
    if (as_sub)
        out << delims.sub_prefix;
    auto n = sizeof...(Ts);
    if (n == 0)
        out << delims.empty;
    else {
        auto delim = as_sub ? delims.sub_delim : delims.base_delim;
        decltype(delim) delim2 = "";
        std::apply([&](const auto&... args) {
            ((out << delim2, output_helper(args, delims, true, out), delim2 = delim), ...);
        }, tuple);
    }
    if (as_sub)
        out << delims.sub_suffix;
}

// output a range:

template <std::ranges::range T, typename CharT, typename Traits>
void output_helper(const T& range, const basic_delimiters<CharT, Traits>& delims, bool as_sub, std::basic_ostream<CharT, Traits>& out)
{
    if (as_sub)
        out << delims.sub_prefix;
    auto begin = range.begin();
    auto end = range.end();
    if (begin == end)
        out << delims.empty;
    else {
        auto itr = begin;
        output_helper(*itr, delims, true, out);
        auto delim = as_sub ? delims.sub_delim : delims.base_delim;
        while (++itr != end) {
            out << delim;
            output_helper(*itr, delims, true, out);
        }
    }
    if (as_sub)
        out << delims.sub_suffix;
}

// output a string:

template <typename CharT, typename Traits>
inline void output_helper(const CharT* str, const basic_delimiters<CharT, Traits>& delims, bool, std::basic_ostream<CharT, Traits>& out)
{if (*str) out << str; else out << delims.empty;}

template <typename CharT, typename Traits, typename Allocator>
inline void output_helper(const std::basic_string<CharT, Traits, Allocator>& str, const basic_delimiters<CharT, Traits>& delims, bool, std::basic_ostream<CharT, Traits>& out)
{if (str.size()) out << str; else out << delims.empty;}

template <typename CharT, typename Traits>
inline void output_helper(const std::basic_string_view<CharT, Traits>& str, const basic_delimiters<CharT, Traits>& delims, bool, std::basic_ostream<CharT, Traits>& out)
{if (str.size()) out << str; else out << delims.empty;}

// default output:

template <typename T, typename CharT, typename Traits>
concept ostream_insertable = requires(std::basic_ostream<CharT, Traits>& out, const T& x) {
    {out << x} -> std::convertible_to<std::basic_ostream<CharT, Traits>&>;
};

template <typename CharT, typename Traits, ostream_insertable<CharT, Traits> T>
inline void output_helper(const T& x, const basic_delimiters<CharT, Traits>&, bool, std::basic_ostream<CharT, Traits>& out)
{out << x;}


} // namespace delimited_output

#endif // DELIMITED_OUTPUT_HPP
