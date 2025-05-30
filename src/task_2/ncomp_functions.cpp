#include "curry.hpp"

#include "string"


int id(int x) { return x; }


int main() {
    std::string s;
    control_03::curry(id)(s);
    return 0;
}
