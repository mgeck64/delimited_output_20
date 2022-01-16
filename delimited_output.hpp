#ifndef DELIMITED_OUTPUT_HPP
#define DELIMITED_OUTPUT_HPP

#include <ostream>
#include <tuple>
#include <type_traits>
#include <cassert>

namespace delimited_output {

// c_str_wrapper:
// A simple wrapper for C style strings that checks for null pointer. I tried
// using std::string_view as a wrapper for C style strings but I became
// disenchanted with it, at least for my purpose here. First, string_view stores
// both a pointer to the string and its length (and not just a pointer); second,
// string_view computes the length of the string on construction, though this is
// ostensibly a compile-time computation for string literals.

class c_str_wrapper {
    const char* p;
public:
    c_str_wrapper(const char* p_)
        : p{p_ ? p_ : "c_str_wrapper: null pointer"} // or throw an exception?
    {assert(p);}

    const char* c_str() const {return p;} // mirroring std::string::c_str()

    // note: no conversion operator to const char* is provided because of
    // well-known problems with automatic conversions.
    // note2: c_str_wrapper is valid for so long as the stored pointer is valid
};

template <typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& out, const c_str_wrapper& str)
{return out << str.c_str();}

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

// delimited() returns a helper object that stores a reference or iterator pair
// to the object or sequence to output, and thus is valid for so long as the
// reference or iterator pair is valid. Note: an expression such as
//    std::cout << delimited(std::string("Hello"))
// is OK as the helper object will be valid for the duration of the expression.

template <typename T>
concept iterator = std::input_or_output_iterator<T>;

template <typename> class delimited_inserter;
template <iterator> class delimited_inserter_for_sequence;
struct delimiters;

template <typename Object>
inline auto delimited(const Object& obj)
{return delimited_inserter{obj};}

template <typename Object>
inline auto delimited(const Object& obj, const delimiters& delims)
{return delimited_inserter{obj, delims};}

template <iterator Iterator>
inline auto delimited(Iterator begin, Iterator end)
{return delimited_inserter_for_sequence{begin, end};}

template <iterator Iterator>
inline auto delimited(Iterator begin, Iterator end, const delimiters& delims)
{return delimited_inserter_for_sequence{begin, end, delims};}

// note: delimited() is technically redundant but convenient shorthand for
// instantiating delimited_inserter and delimited_inserter_for_sequence

// delimiters:

struct delimiters { // delimiters and related values
    // herein, "collection" refers to a sequence or collection of elements such
    // as in a container, sequence, tuple or pair

    c_str_wrapper base_delim = ", "; // base-level collection delimiter (except for pair)
    // example for tuple<int, string, int>: 1, Two, 3
    // example for container of ints: 10, 20, 30, 40, 50

    // values for recursively output sub-level collection (except for pair):
    c_str_wrapper sub_prefix = "("; // sub-level collection prefix
    c_str_wrapper sub_delim = ", "; // sub-level delimiter
    c_str_wrapper sub_suffix = ")"; // sub-level collection suffix
    // example for container of tuples: (1, Two, 3), (4, Five, 6), (7, Eight, 9)

    // values for base-level and recursively output sub-level pair:
    c_str_wrapper pair_prefix = "["; // sub-level pair prefix
    c_str_wrapper pair_delim = ": "; // base- and sub-level pair delimiter
    c_str_wrapper pair_suffix = "]"; // sub-level pair suffix
    // base-level example for pair<int, string>: 1: One
    // sub-level example for map<int, string>: [1: One], [2: Two], [3: Three]

    bool base_as_sub = false; // output base-level collection as sub-level one
    // true example for tuple<int, string, int>: (1, Two, 3)
    // true example for container of ints: (10, 20, 30, 40, 50)
    // true example for container of tuples: ((1, Two, 3), (4, Five, 6), (7, Eight, 9))
    // true example for pair<int, string>: [1: One]
    // true example for map<int, string>: ([1: One], [2: Two], [3: Three])
    // n/a for string or default output; for example, string("Hello") and
    // int(123) will be output normally regardless of this setting

    c_str_wrapper empty = "<empty>"; // text for empty object or empty sequence

    // note: delimiter stores c_str_wrappers and thus is valid for so long as
    // the c_str_wrappers (C string pointers) are valid
};

// delimited_inserter:

template <typename Object>
class delimited_inserter {
    const Object& obj;
    delimiters delims;
public:
    delimited_inserter(const Object& obj_)
        : obj{obj_} {}
    delimited_inserter(const Object& obj_, const delimiters& delims_)
        : obj{obj_}, delims{delims_} {}

    // inserter:

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& out, const delimited_inserter& di)
    {output_helper(di.obj, di.delims, di.delims.base_as_sub, out); return out;}

    // value setters:
    // Each function return a reference to *this so calls can be chained; e.g.,
    // cout << delimited(container).delimiter(" - ").empty("Empty")

    auto& delimiter(const char* str) // sets base_delim and sub_delim (but not pair_delim)
    {delims.base_delim = str; delims.sub_delim = str; return *this;}

    auto& base_delim(const char* str)
    {delims.base_delim = str; return *this;}

    auto& sub_prefix(const char* str)
    {delims.sub_prefix = str; return *this;}

    auto& sub_delim(const char* str)
    {delims.sub_delim = str; return *this;}

    auto& sub_suffix(const char* str)
    {delims.sub_suffix = str; return *this;}

    auto& pair_prefix(const char* str)
    {delims.pair_prefix = str; return *this;}

    auto& pair_delim(const char* str)
    {delims.pair_delim = str; return *this;}

    auto& pair_suffix(const char* str)
    {delims.pair_suffix = str; return *this;}

    auto& base_as_sub(bool b = true)
    {delims.base_as_sub = b; return *this;}

    auto& as_sub(bool b = true) // concise alternative; e.g.: delimited(arr).as_sub()
    {delims.base_as_sub = b; return *this;}

    auto& empty(const char* str)
    {delims.empty = str; return *this;}
};

// sequence, delimited_inserter_for_sequence:

template <iterator Iterator>
struct sequence {
    Iterator begin_itr, end_itr;
    Iterator begin() const {return begin_itr;}
    Iterator end() const {return end_itr;}
};

template <iterator Iterator>
class delimited_inserter_for_sequence: public delimited_inserter<sequence<Iterator>> {
    sequence<Iterator> seq;
public:
    delimited_inserter_for_sequence(Iterator begin, Iterator end)
        : delimited_inserter<sequence<Iterator>>{seq} {seq.begin_itr = begin; seq.end_itr = end;}
    delimited_inserter_for_sequence(Iterator begin, Iterator end, const delimiters& delims)
        : delimited_inserter<sequence<Iterator>>{seq, delims} {seq.begin_itr = begin; seq.end_itr = end;}
};

// output helper functions:

// output a pair:

template <typename T1, typename T2, typename CharT, typename Traits>
void output_helper(const std::pair<T1, T2>& pair, const delimiters& delims, bool as_sub, std::basic_ostream<CharT, Traits>& out) {
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
void output_helper(const std::tuple<Ts...>& tuple, const delimiters& delims, bool as_sub, std::basic_ostream<CharT, Traits>& out)
{
    if (as_sub)
        out << delims.sub_prefix;
    auto n = sizeof...(Ts);
    if (n == 0)
        out << delims.empty;
    else {
        auto delim = as_sub ? delims.sub_delim.c_str() : delims.base_delim.c_str();
        auto delim2 = "";
        std::apply([&](const auto&... args) {
            ((out << delim2, output_helper(args, delims, true, out), delim2 = delim), ...);
        }, tuple);
    }
    if (as_sub)
        out << delims.sub_suffix;
}

// output a range:

template <std::ranges::range T, typename CharT, typename Traits>
void output_helper(const T& range, const delimiters& delims, bool as_sub, std::basic_ostream<CharT, Traits>& out)
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
inline void output_helper(const char* str, const delimiters& delims, bool, std::basic_ostream<CharT, Traits>& out)
{auto wstr = c_str_wrapper(str); out << (*wstr.c_str() ? wstr : delims.empty);}

template <typename CharT, typename Traits>
inline void output_helper(const std::string& str, const delimiters& delims, bool, std::basic_ostream<CharT, Traits>& out)
{if (str.size()) out << str; else out << delims.empty;}

template <typename CharT, typename Traits>
inline void output_helper(const std::string_view& str, const delimiters& delims, bool, std::basic_ostream<CharT, Traits>& out)
{if (str.size()) out << str; else out << delims.empty;}

// default output:

template <typename T, typename CharT, typename Traits>
concept ostream_insertable = requires(std::basic_ostream<CharT, Traits>& out, const T& x) {
    {out << x} -> std::convertible_to<std::basic_ostream<CharT, Traits>&>;
};

template <typename CharT, typename Traits, ostream_insertable<CharT, Traits> T>
inline void output_helper(const T& x, const delimiters&, bool, std::basic_ostream<CharT, Traits>& out)
{out << x;}


} // namespace delimited_output

#endif // DELIMITED_OUTPUT_HPP
