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
    // well-known problems with automatic conversions
    // note2: c_str_wrapper has the same invalidation rule as const char*
};

inline std::ostream& operator<<(std::ostream& out, const c_str_wrapper& str)
{return out << str.c_str();}

// delimited():

// delimited() can be used to output a container-like object as delimited text;
// for example:
//    auto arr = std::array{1, 3, 5, 7, 9};
//    cout << delimited(arr);
// outputs:
//    1, 3, 5, 7, 9
// However, strings (which are char containers) as an exception are output
// normally, so:
//    cout << delimited(std::string{"Hello"})
// outputs:
//    Hello

// delimited() can also be used to output an iterator range as delimited text;
// for example:
//    cout << delimited(arr.begin() + 1, arr.end() - 1)
// outputs:
//    3, 5, 7

// delimited() can also be used to output a pair or tuple; for example:
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
// will be enclosed in square brackets and container-like and tuple elements
// will be enclosed in parentheses; see delimiters below for details.

// Delimiter values can be specified via setter functions; for example:
//     cout << delimited(arr).delimiter(" - ").empty("Empty")
// Delimiter values can also be specified via a delimiters object. The complete
// set of delimiter values is in delimiters below and the complete set of setter
// functions is in delimiter_inserter below.

// delimited() returns a helper object that stores a reference or iterator range
// to the object or sequence to output, and thus has the invalidation rule for
// that reference or iterator range. Note: an expression such as
//    std::cout << delimited(std::string("Hello"))
// is OK as the helper object will be valid for the duration of the expression.

// Implementation note: delimited output functionality is implemented for
// ostream but not basic_ostream; that generalization is beyond the scope of
// what I want to accomplish in this project.

template <typename> class delimited_inserter;
template <typename> class delimited_inserter_for_range;
struct delimiters;

template <typename Object>
inline auto delimited(const Object& obj)
{return delimited_inserter{obj};}

template <typename Object>
inline auto delimited(const Object& obj, const delimiters& delims)
{return delimited_inserter{obj, delims};}

template <typename Iterator>
inline auto delimited(Iterator begin, Iterator end)
{return delimited_inserter_for_range{begin, end};}

template <typename Iterator>
inline auto delimited(Iterator begin, Iterator end, const delimiters& delims)
{return delimited_inserter_for_range{begin, end, delims};}

// note: delimited() is technically redundant but convenient shorthand for
// instantiating delimited_inserter and delimited_inserter_for_range

// delimiters:

struct delimiters { // delimiters and related values
    // herein, "collection" refers to a sequence or collection of elements such
    // as in a container, iterator range, tuple or pair

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

    c_str_wrapper empty = "<empty>"; // text for empty object or iterator range

    // note: delimiter stores c_str_wrappers and thus has c_str_wrapper's
    // invalidation rule
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

    friend std::ostream& operator<<(std::ostream& out, const delimited_inserter& di)
    {output(di.obj, di.delims, di.delims.base_as_sub, out); return out;}

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

// iterator_range, delimited_inserter_for_range:

template <typename Iterator>
struct iterator_range { // container-like object for delimited_inserter
    Iterator begin_itr, end_itr;
    Iterator begin() const {return begin_itr;}
    Iterator end() const {return end_itr;}
};

template <typename Iterator>
class delimited_inserter_for_range:
        public delimited_inserter<iterator_range<Iterator>> {
    iterator_range<Iterator> range;
public:
    delimited_inserter_for_range(Iterator begin, Iterator end)
        : delimited_inserter<iterator_range<Iterator>>{range} {range.begin_itr = begin; range.end_itr = end;}
    delimited_inserter_for_range(Iterator begin, Iterator end, const delimiters& delims)
        : delimited_inserter<iterator_range<Iterator>>{range, delims} {range.begin_itr = begin; range.end_itr = end;}
};

// output() functions:
// (output() can be additionally overloaded for other types)

// output a pair:

template <typename T1, typename T2>
void output(const std::pair<T1, T2>& pair, const delimiters& delims, bool as_sub, std::ostream& out) {
    if (as_sub)
        out << delims.pair_prefix;
    output(pair.first, delims, true, out);
    out << delims.pair_delim;
    output(pair.second, delims, true, out);
    if (as_sub)
        out << delims.pair_suffix;
}

// output a tuple:

template<typename... Ts>
void output(const std::tuple<Ts...>& tuple, const delimiters& delims, bool as_sub, std::ostream& out)
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
            ((out << delim2, output(args, delims, true, out), delim2 = delim), ...);
        }, tuple);
    }
    if (as_sub)
        out << delims.sub_suffix;
}

// output a container-like object:

template <typename, typename = std::void_t<>>
constexpr bool is_iterator = std::false_type::value;

template <typename T>
constexpr bool is_iterator<T, std::void_t<
// T is an iterator type if it supports increment and dereference operations
    decltype(++std::declval<T&>()),
    decltype(*std::declval<T&>())
>> = std::true_type::value;

template <typename, typename = std::void_t<>>
constexpr bool is_container_like = std::false_type::value;

template <typename T>
constexpr bool is_container_like<T, std::void_t<
// for purposes here, if begin() and end() are const members of T and each
// returns an iterator then assume T is a container-like type
    std::enable_if_t<is_iterator<decltype(std::declval<const T>().begin())>>,
    std::enable_if_t<is_iterator<decltype(std::declval<const T>().end())>>
>> = std::true_type::value;

template <typename T>
inline std::enable_if_t<is_container_like<T>, void>
output(const T& cont, const delimiters& delims, bool as_sub, std::ostream& out)
{
    if (as_sub)
        out << delims.sub_prefix;
    auto begin = cont.begin();
    auto end = cont.end();
    if (begin == end)
        out << delims.empty;
    else {
        auto itr = begin;
        output(*itr, delims, true, out);
        auto delim = as_sub ? delims.sub_delim : delims.base_delim;
        while (++itr != end) {
            out << delim;
            output(*itr, delims, true, out);
        }
    }
    if (as_sub)
        out << delims.sub_suffix;
}

// output a string:

inline void output(const char* str, const delimiters& delims, bool, std::ostream& out)
{auto wstr = c_str_wrapper(str); out << (*wstr.c_str() ? wstr : delims.empty);}

inline void output(const std::string& str, const delimiters& delims, bool, std::ostream& out)
{if (str.size()) out << str; else out << delims.empty;}

inline void output(const std::string_view& str, const delimiters& delims, bool, std::ostream& out)
{if (str.size()) out << str; else out << delims.empty;}

// default output:

template <typename, typename = std::void_t<>>
struct is_output_declared_for : std::false_type {};

template <typename T>
struct is_output_declared_for<T, std::void_t<
    decltype(output(std::declval<T>(), std::declval<delimiters>(), std::declval<bool>(), std::declval<std::ostream&>()))
>> : std::true_type {};

template <typename T>
inline std::enable_if_t<!is_output_declared_for<T>::value, void>
output(const T& x, const delimiters&, bool, std::ostream& out)
{out << x;}


} // namespace delimited_output

#endif // DELIMITED_OUTPUT_HPP
