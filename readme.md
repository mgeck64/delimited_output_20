# delimited_output
Provides a stream inserter that recursively outputs the contents of pairs,
tuples and container-like objects.

This was originally developed as a C++17 learning project and is upgraded here
to use C++20 Concepts instead of SFINAE tricks.
(Also now supports basic_ostream, not just ostream.)
I also tried to upgrade this to use C++20 Modules but Modules seems to still be
in a very experimential state as of the time of this writing (1/15/22), so this
was left implemented as a traditional header file.

Usage example:
```
vector<tuple<int, string, int>> tups = {{1, "Two", 3}, {4, "Five", 6}, {7, "Eight", 9}};
cout << delimited(tups) << endl;
```
Output:
```
(1, Two, 3), (4, Five, 6), (7, Eight, 9)
```
