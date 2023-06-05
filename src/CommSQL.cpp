#include <CommSQL.h>




sqlite3* CommSQL::openDatabase(const std::string& db_name, int try_times) {
    sqlite3* db;
    if (try_times > MAX_OPEN_TIMES) {
        try_times = MAX_OPEN_TIMES;
    }
    int rc = sqlite3_open_v2(db_name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        if (try_times < 1) {
            // 在这里，你可以选择抛出一个异常或者执行其他的错误处理程序
            fprintf(stderr, "Can't open database: %s.\n", sqlite3_errmsg(db));
            return nullptr;
        }
        fprintf(stderr, "[R|W] Try open %s - (%d / %d) times : %s\n", db_name.c_str(), try_times, MAX_OPEN_TIMES, sqlite3_errmsg(db));
        return openDatabase(db_name, try_times - 1);
    }
    return db;
}

int CommSQL::closeDatabase(sqlite3* db) {
    return sqlite3_close(db);
}

sqlite3_stmt* CommSQL::prepareStatement(sqlite3* db, const std::string& sql) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // 这里可以处理错误
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
    }
    return stmt;
}

// std::string CommSQL::generateSelectQuery(const std::string& table_name, const std::string& column_name, const std::string& condition) {
//     std::string query = "SELECT " + column_name + " FROM " + table_name;
//     std::cout << "query: " << query << std::endl;
//     if (!condition.empty()) {
//         query += " WHERE " + condition;
//     }
//     return query;
// }

// sqlite3* CommSQL::openDatabase(const std::string& db_name, int try_times) {
//     sqlite3* db;
//     if (try_times > MAX_OPEN_TIMES) {
//         try_times = MAX_OPEN_TIMES;
//     }
//     int rc = sqlite3_open_v2(db_name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, nullptr);
//     if (rc != SQLITE_OK) {
//         if (try_times < 1) {
//             // 在这里，你可以选择抛出一个异常或者执行其他的错误处理程序
//             fprintf(stderr, "Can't open database: %s.\n", sqlite3_errmsg(db));
//             return nullptr;
//         }
//         fprintf(stderr, "[R|W] Try open %s - (%d / %d) times : %s\n", db_name.c_str(), try_times, MAX_OPEN_TIMES, sqlite3_errmsg(db));
//         return openDatabase(db_name, try_times - 1);
//     }
//     return db;
// }

// int CommSQL::closeDatabase(sqlite3* db) {
//     return sqlite3_close(db);
// }

// sqlite3_stmt* CommSQL::prepareStatement(sqlite3* db, const std::string& sql) {
//     sqlite3_stmt* stmt = nullptr;
//     int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
//     if (rc != SQLITE_OK) {
//         // 这里可以处理错误
//         fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
//     }
//     return stmt;
// }

// template <typename... Args>
// inline sqlite3_stmt* CommSQL::prepareStatement(sqlite3* db, const std::string& sql, Args&&... args) {
//     sqlite3_stmt* stmt = nullptr;
//     int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
//     if (rc != SQLITE_OK) {
//         // 处理错误
//         fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
//     }

//     int index = 1;
//     (bind_value(stmt, index++, std::forward<Args>(args)), ...);
//     return stmt;
// }

// std::string CommSQL::generateSelectQuery(const std::string& table_name, const std::string& column_name, const std::string& condition) {
//     std::string query = "SELECT " + column_name + " FROM " + table_name;
//     if (!condition.empty()) {
//         query += " WHERE " + condition;
//     }
//     return query;
// }

// template <typename Container>
// std::string CommSQL::generateSelectQuery(const std::string& table_name, const Container& column_names, const Container& conditions) {
//     std::string query = "SELECT ";
//     for (const auto& column : column_names) {
//         query += column + ", ";
//     }
//     if (!column_names.empty()) {
//         query = query.substr(0, query.size() - 2);  // Remove the last ", "
//     }
//     query += " FROM " + table_name;
//     if (!conditions.empty()) {
//         query += " WHERE ";
//         for (const auto& condition : conditions) {
//             query += condition + " AND ";
//         }
//         query = query.substr(0, query.size() - 5);  // Remove the last " AND "
//     }
//     return query;
// }

// template <typename Container>
// std::string CommSQL::generateSelectQuery(const std::string& table_name, const Container& column_names, const std::map<std::string, std::string>& conditions) {
//     std::string query = "SELECT ";
//     for (const auto& column : column_names) {
//         query += column + ", ";
//     }
//     if (!column_names.empty()) {
//         query = query.substr(0, query.size() - 2);  // Remove the last ", "
//     }
//     query += " FROM " + table_name;
//     if (!conditions.empty()) {
//         query += " WHERE ";
//         for (const auto& condition : conditions) {
//             query += condition.first + " = " + condition.second + " AND ";
//         }
//         query = query.substr(0, query.size() - 5);  // Remove the last " AND "
//     }
//     return query;
// }

// template <typename T>
// void CommSQL::readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&)) {
//     sqlite3_stmt* stmt = prepareStatement(db, sql);
//     if (sqlite3_step(stmt) != SQLITE_ROW) {
//         // 处理错误逻辑
//         fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
//         sqlite3_finalize(stmt);
//         return;
//     }
//     result = get_column_value<T>(stmt, 0);
//     if (handle != nullptr) {
//         handle(result);
//     }
//     sqlite3_finalize(stmt);
// }

// template <typename T>
// void CommSQL::readData(sqlite3* db, const std::string& sql, std::vector<T>& result, void (*handle)(T&)) {
//     sqlite3_stmt* stmt = prepareStatement(db, sql);
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         T data = get_column_value<T>(stmt, 0);
//         if (handle != nullptr) {
//             handle(data);
//         }
//         result.push_back(data);
//     }
//     sqlite3_finalize(stmt);
// }

// template <typename T>
// void CommSQL::readSingleData(sqlite3_stmt* stmt, T& result, void (*handle)(T&)) {
//     if (sqlite3_step(stmt) == SQLITE_ROW) {
//         result = get_column_value<T>(stmt, 0);
//         if (handle != nullptr) {
//             handle(result);
//         }
//     }
// }

// template <typename T>
// void CommSQL::readData(sqlite3_stmt* stmt, std::vector<T>& result, void (*handle)(T&)) {
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         T data = get_column_value<T>(stmt, 0);
//         if (handle != nullptr) {
//             handle(data);
//         }
//         result.push_back(data);
//     }
// }

// template <typename U>
// U CommSQL::get_column_value(sqlite3_stmt* stmt, int i) {
//     if constexpr (std::is_same_v<U, int>) {
//         return sqlite3_column_int(stmt, i);
//     } else if constexpr (std::is_same_v<U, double> || std::is_same_v<U, float>) {
//         return sqlite3_column_double(stmt, i);
//     } else if constexpr (std::is_same_v<U, std::string>) {
//         auto text = sqlite3_column_text(stmt, i);
//         return text ? std::string(reinterpret_cast<const char*>(text)) : "";
//     } else if constexpr (std::is_same_v<U, char*> || std::is_same_v<U, char[CFG_STR_SMALL_LEN]> || std::is_same_v<U, char[CFG_STR_BIG_LEN]>) {
//         return (const char*)sqlite3_column_text(stmt, 0);
//     } else {
//         // 对于其他类型的处理
//         // ...
//         return U{};  // 返回默认值，可根据需要进行修改
//     }
// }

// template <typename U>
// void CommSQL::bind_value(sqlite3_stmt* stmt, int index, U&& value) {
//     if constexpr (std::is_same_v<std::decay_t<U>, int>) {
//         sqlite3_bind_int(stmt, index, value);
//     } else if constexpr (std::is_same_v<std::decay_t<U>, double>) {
//         sqlite3_bind_double(stmt, index, value);
//     } else if constexpr (std::is_same_v<std::decay_t<U>, std::string>) {
//         sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
//     } else if constexpr (std::is_array_v<std::decay_t<U>>) {
//         using ValueType = typename std::decay_t<U>::value_type;
//         if constexpr (std::is_same_v<ValueType, const char*>) {
//             sqlite3_bind_text(stmt, index, value, -1, SQLITE_STATIC);  // 处理字符串字面量
//         } else {
//             // 处理其他数组类型
//         }
//     } else {
//         // 处理错误
//     }
// }
// // 显式实例化模板函数
// template void CommSQL::readSingleData<int>(sqlite3* db, const std::string& sql, int& result, void (*handle)(int&));
// template void CommSQL::readSingleData<std::string>(sqlite3* db, const std::string& sql, std::string& result, void (*handle)(std::string&));
// template void CommSQL::readSingleData<double>(sqlite3* db, const std::string& sql, double& result, void (*handle)(double&));
// template void CommSQL::readSingleData<float>(sqlite3* db, const std::string& sql, float& result, void (*handle)(float&));
// template void CommSQL::readSingleData<char[CFG_STR_SMALL_LEN]>(sqlite3* db, const std::string& sql, char (&result)[CFG_STR_SMALL_LEN], void (*handle)(char (&)[CFG_STR_SMALL_LEN]));
// template void CommSQL::readSingleData<char[CFG_STR_BIG_LEN]>(sqlite3* db, const std::string& sql, char (&result)[CFG_STR_BIG_LEN], void (*handle)(char (&)[CFG_STR_BIG_LEN]));
// template void CommSQL::readSingleData<char*>(sqlite3* db, const std::string& sql, char*& result, void (*handle)(char*&));

// // template sqlite3_stmt* CommSQL::prepareStatement<int, int, int>(sqlite3* db, const std::string& sql, int&&, int&&, int&&);