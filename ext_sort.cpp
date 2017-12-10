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
#include <cstdio>
#include <queue>

using namespace std;

using p_t = pair<string, size_t>;
using my_heap = priority_queue<p_t, vector<p_t>, greater<p_t>>;

size_t split_and_sort(const char* input_fname,
                      const size_t max_mem,
                      vector<string>& split_names) {

    size_t max_char_count = max_mem / 3;
    size_t ios_buf_size = (max_mem - max_char_count) / 2 + 1;
    vector<char> ios_input_buf(ios_buf_size);
    vector<char> ios_output_buf(ios_buf_size);
    ifstream ifs;

    ifs.rdbuf()->pubsetbuf(ios_input_buf.data(), ios_buf_size);
    ifs.open(input_fname, ios_base::in);

    if (!ifs) {
        throw runtime_error{"Error while openning input file"};
    }

    size_t max_line_len = 0ull;
    size_t split_counter = 0ull;

    while (ifs) {
        vector<string> lines;
        size_t char_count = 0ull;
        string line;

        while (char_count < max_char_count && ifs >> line) {
            max_line_len = max(max_line_len, line.size());
            char_count += line.size();
            lines.push_back(move(line));
        }

        sort(lines.begin(), lines.end());

        auto split_name = to_string(++split_counter);
        split_names.push_back(split_name);
        ofstream ofs;
        ofs.rdbuf()->pubsetbuf(ios_output_buf.data(), ios_buf_size);
        ofs.open(split_name, ios_base::out);

        for (auto& line : lines) {
            ofs << line << "\n"; 
        }
    }

    return max_line_len;
}

string merge_splits(vector<string>& split_names,
                    const size_t max_mem,
                    const size_t max_len) {

    const size_t max_heap_size = max_mem / 3;
    const size_t nways = max_heap_size / max_len;

    if (nways <= 1) {
        throw runtime_error{"Error: can not fit into "
                            "memory constraints while merging"};
    }

    if (split_names.empty()) {
        throw runtime_error{"Error: no sorted split files"};
    }

    const size_t ios_input_buf_size = (max_mem - max_heap_size) / 2;
    const size_t ios_output_buf_size = ios_input_buf_size;
    vector<vector<char>> ios_input_bufs(nways, vector<char>(ios_output_buf_size / nways));
    vector<char> ios_output_buf(ios_output_buf_size);

    vector<string> split_names_tmp;
    split_names_tmp.reserve(split_names.size() / nways + 1);

    while (split_names.size() > 1) {
        size_t block_start = 0;

        while (block_start < split_names.size()) {
            size_t rem = min(nways,
                             split_names.size() - block_start);

            vector<ifstream> file_streams;
            file_streams.reserve(rem);

            for (size_t i = 0ull; i < rem; ++i) {
                const string& fname = split_names[block_start + i];
                file_streams.push_back(ifstream{fname});
                file_streams.back().rdbuf()->pubsetbuf(ios_input_bufs[i].data(),
                                                       ios_input_bufs[i].size());
            }

            my_heap heap;

            for (size_t i = 0ull; i < rem; ++i) {
                string line;
                if (file_streams[i] >> line) {
                    heap.push({line, i});
                } else if (file_streams[i].eof()) {
                    file_streams[i].close();
                    remove(split_names[block_start + i].c_str());
                } else {
                    throw runtime_error{"Error while processing sorted block"};
                }
            }

            split_names_tmp.push_back(split_names[block_start] + "1");
            ofstream ofs;
            ofs.rdbuf()->pubsetbuf(ios_output_buf.data(), ios_output_buf_size);
            ofs.open(split_names_tmp.back());

            while (!heap.empty()) {
                auto p = heap.top();
                heap.pop();
                ofs << p.first << "\n";
                string line;
                if (file_streams[p.second] >> line) {
                    heap.push({line, p.second});
                } else if (file_streams[p.second].eof()) {
                    file_streams[p.second].close();
                    remove(split_names[block_start + p.second].c_str());
                } else {
                    throw runtime_error{"Error while processing sorted block"};
                }
            }

            block_start += rem;
        }

        split_names.swap(split_names_tmp);
        split_names_tmp.clear();
    }

    return split_names.front();
}

void ext_sort(const char* input_fname, const char* output_fname, size_t max_mem) {
    vector<string> init_split_names;
    size_t max_len = split_and_sort(input_fname, max_mem, init_split_names);
    string fname = merge_splits(init_split_names, max_mem, max_len);
    rename(fname.c_str(), output_fname);
}

int main(int argc, char** argv)
{
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    if (argc < 3 || (argc == 4 && (atoll(argv[3]) < 4))) {
        cout << "Usage: ./a.out input_file output_file [max_memory (in mb, >= 4)]\n"
                "max_memory is only a hint, not a guarantee.";
        return 0;
    }

    size_t max_mem = 100 * 1024 * 1024; // 100 mb

    if (argc >= 4) {
        max_mem = atoll(argv[3]) * 1024 * 1024;
    }

    // heuristic
    max_mem *= 0.85;

    try {
        ext_sort(argv[1], argv[2], max_mem);
    } catch (runtime_error& err) {
        cerr << err.what() << endl;
        return -1;
    }

    cout << "Successfully sorted!\n";

    return 0;
}
