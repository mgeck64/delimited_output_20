# delimited_output
Provides a stream inserter that recursively outputs the contents of pairs, tuples and container-like objects.

Usage example:
```
vector<tuple<int, string, int>> tups = {{1, "Two", 3}, {4, "Five", 6}, {7, "Eight", 9}};
cout << delimited(tups) << endl;
```
Output:
```
(1, Two, 3), (4, Five, 6), (7, Eight, 9)
```
