#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <string>
#include <algorithm>
#include <chrono>
#include <limits>
#include <cmath>
#include <unordered_set>
#include <set>
#include <stack>
#include <queue>
#include <limits>
#include <cassert>
#include <fstream>

using namespace std;

string generate(size_t len) {
    string s;
    s.reserve(len);

    while (len--) {
        s.append(1, 'a' + (rand() % 26));
    }

    return s;
}

int main(int argc, char** argv)
{
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    if (argc < 4) {
        cout << "Usage: ./generator line_count line_length filename\n";
        return 0;
    }

    srand(time(0));

    size_t line_count = atoll(argv[1]);
    size_t line_length = atoll(argv[2]);
    const char* fname = argv[3];

    ofstream ofs(fname);

    while (line_count--) {
        ofs << generate(line_length) << "\n";
    }

    return 0;
}
