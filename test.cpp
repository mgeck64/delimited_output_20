#include "delimited_output.hpp"

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <map>
#include <tuple>
#include <string>

int main() {
    using namespace std;
    using namespace delimited_output;

    {
        cout << delimited(6) << endl;

        tuple<int, string, int> tup{1, "Two", 3};
        array<int, 5> ints = {10, 20, 30, 40, 50};
        cout << delimited(tup) << endl;
        cout << delimited(ints) << endl;

        vector<tuple<int, string, int>> tups = {{1, "Two", 3}, {4, "Five", 6}, {7, "Eight", 9}};
        cout << delimited(tups) << endl;

        pair<int, string> par = {1, "One"};
        map<int, string> map = {{1, "One"}, {2, "Two"}, {3, "Three"}};
        cout << delimited(par) << endl;
        cout << delimited(map) << endl;

        cout << endl;
        cout << delimited(tup).as_sub() << endl;
        cout << delimited(ints).as_sub() << endl;
        cout << delimited(tups).as_sub() << endl;
        cout << delimited(par).as_sub() << endl;
        cout << delimited(map).as_sub() << endl;
        cout << delimited("Hello").as_sub() << endl;
        cout << delimited(123).as_sub() << endl;
    }
    {
        cout << endl;
        cout << delimited(tuple()) << endl;
        cout << delimited("Hello!") << endl;
        cout << delimited(string("Hello again!")) << endl;
        cout << delimited("").empty("empty string") << endl;
        cout << delimited(6) << endl;
    }
    {
        cout << endl;
        auto arr = array{7, 3, 11, 1, 9, 5};
        cout << delimited(arr) << endl;
        sort(arr.begin(), arr.end());
        cout << delimited(arr) << endl;
        cout << delimited(arr.begin() + 1, arr.end() - 1) << endl;
    }
    {
        cout << endl;
        auto week = array{"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        week.front() = "Fooday";
        cout << delimited(week).delimiter(" - ") << endl;
    }
    {
        cout << endl;
        auto vec = vector{10, 20, 50, 40, 60, 30, 100, 150, 110, 0};
        vec.emplace_back(90);
        vec.emplace_back(70);
        cout << delimited(vec).as_sub() << endl;
        sort(vec.begin(), vec.end());
        cout << delimited(vec).as_sub() << endl;
        vec.clear();
        cout << delimited(vec) << endl;
        cout << delimited(vec).empty("Empty!") << endl;
    }
    {
        cout << endl;
        auto a_map = map<int, const char*>{{1, "One"}, {2, "Two"}, {4, "Four"}};
        cout << delimited(a_map) << endl;
        auto delims = delimiters{};
        delims.pair_prefix = "(Key: ";
        delims.pair_delim = ", Value: ";
        delims.pair_suffix = ")";
        delims.base_delim = "\n";
        cout << delimited(a_map, delims) << endl;
    }
    {
        cout << endl;
        auto maps = array{
            map<int, const char*>{{1, "One"}, {3, "Three"}, {5, "Five"}},
            map<int, const char*>{{2, "Two"}, {4, "Four"}, {6, "Six"}},
            map<int, const char*>{{0, "Zero"}, {9, "Nine"}}
        };
        cout << delimited(maps).sub_prefix("").sub_suffix("").base_delim("\n") << endl;
    }
    {
        cout << endl;
        auto vectors = vector<vector<vector<int>>> {
            {{1, 2, 3}, {4}},
            {{5, 6, 7, 8}, {9, 10}},
            {{11, 12}, {13, 14, 15}}
        };
        cout << delimited(vectors) << endl;
        cout << delimited(vectors).base_delim(" | ") << endl;
        cout << delimited(vectors).delimiter(",") << endl;
    }
    {
        cout << endl;
        auto strs = array{string{"Hello"}, string{"world"}};
        cout << delimited(strs) << endl;
    }
    {
        cout << endl;
        auto seasons = array{
            tuple{"Jan", "Feb", "Mar"},
            tuple{"Apr", "May", "Jun"},
            tuple{"Jul", "Aug", "Sep"},
            tuple{"Oct", "Nov", "Dec"}
        };
        cout << delimited(seasons).base_delim("\n") << endl;
    }
}
