#include "log_duration.h"

#include <iostream>
#include <sstream>      // std::stringstream
#include <string>

using namespace std;

class StreamUntier {
public:
     StreamUntier(istream& input_stream) : tie_input_(input_stream){
         tied_before_ = input_stream.tie(nullptr);
    } 
    
    ~StreamUntier() {
    tie_input_.tie(tied_before_);
    }

private:
    ostream* tied_before_;
    istream& tie_input_;
};

int main() {
    LOG_DURATION("\\n with tie"s);

    StreamUntier guard(cin);
    int i;
    while (cin >> i) {
        cout << i * i << "\n"s;
    }

    return 0;
}