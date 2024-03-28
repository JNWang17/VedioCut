#include <iostream>
#include "json.hpp"

int main() {
    json::JSON obj;
    obj["name"] = "John";
    obj["age"] = 25;
    obj["is_student"] = true;

    json::JSON t;
    t["null_num"] = 1;
    t["count"] = 12;
    obj["col_nulls"].append(t);

    t["null_num"] = 2;
    t["count"] = 20;
    obj["col_nulls"].append(t);

    std::cout << json::to_string(obj) << std::endl;
    return 0;
}
