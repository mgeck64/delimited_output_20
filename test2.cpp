// test delimited_output with wcout (as opposed to cout). (can't test together
// because presumably not supposed to use both cout and wcout in the same
// program; see: http://gcc.gnu.org/ml/gcc-bugs/2006-05/msg01196.html)

#include "delimited_output.hpp"

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <map>
#include <tuple>
#include <string>
#include <sstream>

int main() {
    using namespace std;
    using namespace delimited_output;

    {
        wcout << wdelimited(6) << endl;

        tuple<int, wstring, int> tup{1, L"Two", 3};
        array<int, 5> ints = {10, 20, 30, 40, 50};
        wcout << wdelimited(tup) << endl;
        wcout << wdelimited(ints) << endl;

        vector<tuple<int, wstring, int>> tups = {{1, L"Two", 3}, {4, L"Five", 6}, {7, L"Eight", 9}};
        wcout << wdelimited(tups) << endl;

        pair<int, wstring> par = {1, L"One"};
        map<int, wstring> map = {{1, L"One"}, {2, L"Two"}, {3, L"Three"}};
        wcout << wdelimited(par) << endl;
        wcout << wdelimited(map) << endl;

        wcout << endl;
        wcout << wdelimited(tup).as_sub() << endl;
        wcout << wdelimited(ints).as_sub() << endl;
        wcout << wdelimited(tups).as_sub() << endl;
        wcout << wdelimited(par).as_sub() << endl;
        wcout << wdelimited(map).as_sub() << endl;
        wcout << wdelimited(L"Hello").as_sub() << endl;
        wcout << wdelimited(123).as_sub() << endl;
    }
    {
        wcout << endl;
        std::wstringstream ss;
        ss << wdelimited(tuple()) << '\n';
        ss << wdelimited(L"Hello!") << '\n';
        ss << wdelimited(wstring(L"Hello again!")) << '\n';
        ss << wdelimited(L"").empty(L"empty string") << '\n';
        ss << wdelimited(6);
        wcout << ss.str() << endl;
    }
    {
        wcout << endl;
        std::wstringstream ss;
        auto arr = array{7, 3, 11, 1, 9, 5};
        ss << wdelimited(arr) << '\n';
        sort(arr.begin(), arr.end());
        ss << wdelimited(arr) << '\n';
        ss << wdelimited(arr.begin() + 1, arr.end() - 1);
        wcout << ss.str() << endl;
    }
    {
        wcout << endl;
        auto week = array{L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday", L"Sunday"};
        week.front() = L"Fooday";
        wcout << wdelimited(week).delimiter(L" - ") << endl;
    }
    {
        wcout << endl;
        auto maps = array{
            map<int, const wchar_t*>{{1, L"One"}, {3, L"Three"}, {5, L"Five"}},
            map<int, const wchar_t*>{{2, L"Two"}, {4, L"Four"}, {6, L"Six"}},
            map<int, const wchar_t*>{{0, L"Zero"}, {9, L"Nine"}}
        };
        wcout << wdelimited(maps).sub_prefix(L"").sub_suffix(L"").top_delim(L"\n") << endl;
    }
    {
        wcout << endl;
        auto strs = array{wstring{L"Hello"}, wstring{L"world"}};
        wcout << wdelimited(strs) << endl;
    }
    {
        wcout << endl;
        wcout << wdelimited(std::wstring(L"Wide string")) << endl;
        auto vec = vector{10, 20, 50, 40, 60, 30, 100, 150, 110, 0};
        vec.emplace_back(90);
        vec.emplace_back(70);
        wcout << wdelimited(vec).as_sub() << endl;
        sort(vec.begin(), vec.end());
        wcout << wdelimited(vec).as_sub() << endl;
        vec.clear();
        wcout << wdelimited(vec) << endl;
        wcout << wdelimited(vec).empty(L"Empty!") << endl;
    }
    {
        wcout << endl;
        auto a_map = map<int, const char*>{{1, "One"}, {2, "Two"}, {4, "Four"}};
        wcout << wdelimited(a_map) << endl;
        auto delims = wdelimiters{};
        delims.pair_prefix = L"(Key: ";
        delims.pair_delim = L", Value: ";
        delims.pair_suffix = L")";
        delims.top_delim = L"\n";
        wcout << delimited(a_map, delims) << endl;
    }
    {
        wcout << endl;
        auto maps = array{
            map<int, const char*>{{1, "One"}, {3, "Three"}, {5, "Five"}},
            map<int, const char*>{{2, "Two"}, {4, "Four"}, {6, "Six"}},
            map<int, const char*>{{0, "Zero"}, {9, "Nine"}}
        };
        wcout << wdelimited(maps).sub_prefix(L"").sub_suffix(L"").top_delim(L"\n") << endl;
    }
    {
        wcout << endl;
        std::wstringstream ss;
        auto vectors = vector<vector<vector<int>>> {
            {{1, 2, 3}, {4}},
            {{5, 6, 7, 8}, {9, 10}},
            {{11, 12}, {13, 14, 15}}
        };
        ss << wdelimited(vectors) << '\n';
        ss << wdelimited(vectors).top_delim(L" | ") << '\n';
        ss << wdelimited(vectors).delimiter(L",");
        wcout << ss.str() << endl;
    }
    {
        wcout << endl;
        auto seasons = array{
            tuple{"Jan", "Feb", "Mar"},
            tuple{"Apr", "May", "Jun"},
            tuple{"Jul", "Aug", "Sep"},
            tuple{"Oct", "Nov", "Dec"}
        };
        wcout << wdelimited(seasons).top_delim(L"\n") << endl;
    }
}
