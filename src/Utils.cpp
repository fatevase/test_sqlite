#include <any>

#include "Utils.h"

void Utils::myUsleep(int microseconds) {
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

void Utils::getCurrentYYYYMMDDHHMMSSMS(int &year, int &month, int &day, int &hour, int &minute, int &second, int &millisecond) {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        std::time_t now_c = std::chrono::system_clock::to_time_t(now_ms);

        millisecond = value.count() % 1000;

        std::tm* ttm = std::localtime(&now_c);

        year = 1900 + ttm->tm_year;
        month = 1 + ttm->tm_mon;
        day = ttm->tm_mday;
        hour = ttm->tm_hour;
        minute = ttm->tm_min;
        second = ttm->tm_sec;
    }

    std::string Utils::getCurrentDateString() {
        int year, month, day, hour, minute, second, millisecond;
        getCurrentYYYYMMDDHHMMSSMS(year, month, day, hour, minute, second, millisecond);

        char buffer[24];
        snprintf(buffer, sizeof(buffer), 
            "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
            year, month, day, hour, minute, second, millisecond);
        return std::string(buffer);
    }



std::string Utils::escapeJsonString(const std::string& input) {
    std::stringstream ss;
    for (auto iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            // case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}

std::string Utils::mapToJson(const std::unordered_map<std::string, std::string>& map) {
    std::stringstream json;
    json << "{";
    for (auto it = map.begin(); it != map.end(); ++it) {
        json << "\"" << escapeJsonString(it->first) << "\":";
        json << "\"" << escapeJsonString(it->second) << "\"";
        if (std::next(it) != map.end()) {
            json << ",";
        }
    }
    json << "}";
    return json.str();
    
}

// hash_code 在多次调用中，可能会变化，但是在一个函数中调用多次没问题，
// 如果发现问题，则将其放到函数中调用，放弃持久化的思路。
// https://stackoverflow.com/questions/35985960/why-does-stdtype-indexhash-code-change-between-calls
std::unordered_map<size_t, std::string(*)(const std::any&)> Utils::handlers_m2j = {
    { typeid(int).hash_code(), [](const std::any& value) -> std::string {
        return std::to_string(std::any_cast<int>(value));
    } },
    { typeid(double).hash_code(), [](const std::any& value) -> std::string {
        char *buffer;
        std::snprintf(buffer, 64, "%.2f", std::any_cast<double>(value));
        return std::string(buffer);
        // directly use inside to_string function to controll the precision
        // return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, 64, "%.2f", std::any_cast<double>(value));
    } },
    { typeid(float).hash_code(), [](const std::any& value) -> std::string {
        char *buffer;
        std::snprintf(buffer, 64, "%.2f", std::any_cast<float>(value));
        return std::string(buffer);
    } },
    { typeid(bool).hash_code(), [](const std::any& value) -> std::string {
        return std::any_cast<bool>(value) ? "true" : "false";
    } },
    { typeid(std::string).hash_code(), [](const std::any& value) -> std::string {
        return "\"" + escapeJsonString(std::any_cast<std::string>(value)) + "\"";
    } },
    { typeid(const char*).hash_code(), [](const std::any& value) -> std::string {
        return "\"" + escapeJsonString(std::any_cast<const char*>(value)) + "\"";
    } },
    { typeid(char).hash_code(), [](const std::any& value) -> std::string {
        return "\"" + std::string(1, std::any_cast<char>(value)) + "\"";
    } },
    { typeid(std::unordered_map<std::string, std::any>).hash_code(), [](const std::any& value) -> std::string {
        return mapToJson(std::any_cast<std::unordered_map<std::string, std::any>>(value));
    } },
    { typeid(std::vector<std::any>).hash_code(), [](const std::any& value) -> std::string {
        return vectorToJson(std::any_cast<std::vector<std::any>>(value));
    } }
    // Add more handlers for other types...
};

// 这是上述描述出错时的解决方案(optioanl 模板函数， 如果出现问题则在该函数处定义handlers_m2j)
std::optional<std::string> Utils::convertAnyToJson(const std::any& value) {
    // TODO: define handlers_m2j if staitc member variable is not allowed
    // but in this function we need consider multi-threading
    auto handlerIt = handlers_m2j.find(value.type().hash_code());
    if (handlerIt != handlers_m2j.end()) {
        return handlerIt->second(value);
    } else {
        return std::nullopt;
    }
}

// for c++17
/****
 * 将map转换为json字符串，其中key为string，value为基本类型(int, double, float, bool, string)
 * example:
 *  
 * std::unordered_map<std::string, std::any> any_map;
    std::vector<std::any> v_any;
    v_any.push_back("value1");
    v_any.push_back(std::vector<std::any >{"value2", "value3"});
    any_map["key1"] = "value1";
    any_map["key2"] = 2;
    any_map["key3"] = 3.14;
    any_map["key4"] = true;
    any_map["key6"] = 'c';
    any_map["key7"] = v_any;
  
    std::string json_any_map = Utils::mapToJson(any_map);
*/
std::string Utils::mapToJson(const std::unordered_map<std::string, std::any>& map) {
    std::stringstream json;
    json << "{";
    for (auto it = map.begin(); it != map.end(); ++it) {
        auto valueJsonOpt = convertAnyToJson(it->second);
        if (!valueJsonOpt.has_value()) {
            // Handle unknown type, e.g., throw an exception
            valueJsonOpt = "\"UNKNOWN_TYPE\"";
            // throw std::runtime_error("Unknown-[" + it->first + "] type: "+ it->second.type().name());
        }
        json << "\"" << escapeJsonString(it->first) << "\":" << *valueJsonOpt;
        if (std::next(it) != map.end()) {
            json << ",";
        }
    }

    json << "}";
    return json.str();
}

// origin function without optional
std::string Utils::vectorToJson(const std::vector<std::any>& vec) {
    std::stringstream json;
    json << "[";

    for (auto it = vec.begin(); it != vec.end(); ++it) {
        auto handlerIt = handlers_m2j.find(it->type().hash_code());
        if (handlerIt != handlers_m2j.end()) {
            json << handlerIt->second(*it);
        } else {
            // Handle unknown type, e.g., throw an exception or return a default value
            json << "\"UNKNOWN_TYPE\"";
        }
        if (std::next(it) != vec.end()) {
            json << ",";
        }
    }

    json << "]";
    return json.str();
}


/******
 * 将float 或则double 转换为string，并且去掉后续的0
*/
std::string Utils::floatToString(const std::any& value, int precision){
    //TODO: optimize setprecision
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << std::any_cast<double>(value);
    ss.str().erase(ss.str().find_last_not_of('0') + 1, std::string::npos);
    if(ss.str().back() == '.') {
        ss.str().pop_back();
    }
    return ss.str();
}; //only for float or double




void Utils::consoleLog(LogLevel level, const std::string& message) {
    switch(level) {
        case LogLevel::INFO:
            std::cout << "\033[1;32m" << "[INFO] " << message << "\033[0m" << std::endl;
            break;
        case LogLevel::WARNING:
            std::cout << "\033[1;33m" << "[WARNING] " << message << "\033[0m" << std::endl;
            break;
        case LogLevel::ERROR:
            std::cout << "\033[1;31m" << "[ERROR] " << message << "\033[0m" << std::endl;
            break;
    }
}