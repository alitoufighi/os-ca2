#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "defines.hpp"
using namespace std;

int main() {
    string header;
    int num_of_files, num_of_fields;
    vector<string> file_fields; // container for fields which files are described upon
    vector<string> results; // container for filtered results

    string line;
    // read end of pipe is mapped to cin
    getline(cin, header);
    getline(cin, line);
    stringstream ss(line);
    ss >> num_of_files >> num_of_fields;
    vector<string> files(num_of_files);
    vector<Pair> search_fields(num_of_fields);
    vector<int> filter_columns(num_of_fields); // container for indices of filtering columns

    // reading file names
    for (int i = 0; i < num_of_files; ++i)
        getline(cin, files[i]);
    
    // reading filtering fields
    for (int i = 0; i < num_of_fields; ++i)
        cin >> search_fields[i].first >> search_fields[i].second;

    // for each file, filter data
    for (int i = 0; i < num_of_files; ++i) {
        ifstream f(files[i].c_str()); // open file
        string line;
        bool first_line_read = false; // first line of each file is field names. skip them after read once

        // reading from file
        while (getline(f, line)) {
            bool should_filter = true;
            stringstream ss(line);
            string tok;

            // read tokens of line (values)
            for (int j = 0; getline(ss, tok, WS); ++j) {
                if (first_line_read == false) { // if we haven't read the first row (the fields)
                    if (i == 0) { // only for first file
                        file_fields.push_back(tok);
                        for (size_t k = 0; k < search_fields.size(); ++k) {
                            if (tok == search_fields[k].first){
                                filter_columns[k] = j;
                            }
                        }
                    }
                } else {
                    for (int k = 0; k < filter_columns.size(); ++k) {
                        if (j == filter_columns[k]
                            && tok != search_fields[k].second) // if we should filter this one
                                should_filter = false;
                    }
                }
            }
            if (should_filter && first_line_read)
                results.push_back(line);

            first_line_read = true;
        }
        f.close();
    }

    // gathering results into a stringstream
    ss.clear();
    ss << WORKER_HEADER << endl;
    ss << file_fields.size() << WS << results.size() << endl;
    for (size_t i = 0; i < file_fields.size(); ++i)
        ss << file_fields[i] << endl;
    for (size_t i = 0; i < results.size(); ++i)
        ss << results[i] << endl;

    // write final results to named pipe
    ofstream pipe_stream(FIFO_TEMP_PATH, ofstream::in | ofstream::out);
    if (pipe_stream.is_open()) {
        pipe_stream << ss.str();
        pipe_stream.close();
    }
    else
        cerr << "error in opening file." << endl;
    return 0;
}
