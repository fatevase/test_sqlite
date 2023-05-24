#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#define SQLITE_DATA_FILE "db.sqlite3"
#define CONFIG_TABLE "client_deviceinfo"
#define CFG_STR_BIG_LEN 500
#define CFG_STR_SMALL_LEN 100

class CommSQL{
public:
    static sqlite3* openDatabase(const std::string& db_name, int try_times=0);

    static int closeDatabase(sqlite3* db);
    static sqlite3_stmt* prepareStatement(sqlite3 *db, const std::string sql);

    static std::string generateSelectQuery(
        const std::string& table_name, 
        const std::string& column_name, 
        const std::string& condition);

    template<typename Container>
    static std::string generateSelectQuery(
        const std::string& table_name, 
        const Container& column_names,
        const Container& conditions);

    template<typename Container>
    // 对于 std::map 的特化版本
    static std::string generateSelectQuery(
            const std::string& table_name, 
            const Container& column_names, 
            const std::map<std::string, std::string>& conditions);


    template<typename U>
    static U get_column_value(sqlite3_stmt* stmt, int i);
    



    // template <typename T>
    // static void readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&) = nullptr);
    
    // for c++17
    // 实例化模板函数的定义
    template <typename T>
    static void readSingleData(
        sqlite3* db, const std::string& sql,
        T& result, void (*handle)(T&) = nullptr) {
        sqlite3_stmt* stmt = CommSQL::prepareStatement(db, sql);
        if (sqlite3_step(stmt) != SQLITE_ROW) {
            // 处理错误逻辑
            fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }

        // for int type
        if constexpr (std::is_same_v<T, int>) {
            result = sqlite3_column_int(stmt, 0);

        // for string type
        }else if constexpr (std::is_same_v<T, std::string>) {
            auto text = sqlite3_column_text(stmt, 0);
            result = text ? (const char *)text: "";

        // for float type
        }else if constexpr (
            std::is_same_v<T, double> 
            || std::is_same_v<T, float>) {
            result = sqlite3_column_double(stmt, 0);
        
        // for char* char[] (c type)
        }else if constexpr(
            std::is_same_v<T, char[CFG_STR_SMALL_LEN]> 
            || std::is_same_v<T, char[CFG_STR_BIG_LEN]>
            ||std::is_same_v<T, char*>){
            // NOTE: careful about the length of char array
            // for arrays different length means different type
            if(result == nullptr) {
                fprintf(stderr, "Error: result is nullptr\n");
            }
            std::string text = (const char*)sqlite3_column_text(stmt, 0);
            strcpy(result, text.c_str());

        }else{
            fprintf(stderr, "Error: unsupported type\n");
        }
        if (handle != nullptr) handle(result);
        sqlite3_finalize(stmt);
    }

private:



};