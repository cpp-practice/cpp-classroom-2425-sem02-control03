#include "curry.hpp"

#include "string"


struct Idter {
    int operator()(int x) { return x; }
};


int main() {
    std::string s;
    control_03::curry(Idter{})(s);
    return 0;
}
