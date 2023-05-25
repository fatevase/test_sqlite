#include <chrono>
#include <ctime>
#include <iostream>
#include <vector>
#include <unordered_map> // select O(1) map was O(lgn) unordered_map:hash, map:tree
#include <sstream>
#include <string>
#include <iomanip> // for setprecision (精度控制)

#include <thread> // for sleep

#include <any> // C++17
#include <optional> // C++17
#include <typeindex>

class Utils{
public:
    static void getCurrentYYYYMMDDHHMMSSMS(int &year, int &month, int &day, int &hour, int &minute, int &second, int &millisecond);
    static std::string getCurrentDateString();
    static void myUsleep(int microseconds);

    // 过滤json字符串中的特殊字符
    static std::string escapeJsonString(const std::string& input);
    // map转json
    static std::string mapToJson(const std::unordered_map<std::string, std::string>& map);
    static std::string mapToJson(const std::unordered_map<std::string, std::any>& map); // for c++17
    static std::string vectorToJson(const std::vector<std::any>& vec);
    static std::optional<std::string> convertAnyToJson(const std::any& value);
    static std::string floatToString(const std::any& value, int precision=6); //only for float or double

private:
    Utils() = delete;
    ~Utils() = delete;
    Utils(const Utils&) = delete;
    Utils& operator=(const Utils&) = delete;
    // static std::unordered_map<size_t, std::string(*)(const std::any&) > handlers_m2j;
    static std::unordered_map<size_t, std::string(*)(const std::any&)> handlers_m2j;
};
