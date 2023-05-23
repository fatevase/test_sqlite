#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#define SQLITE_DATA_FILE "db.sqlite3"
#define CONFIG_TABLE "client_deviceinfo"


class CommSQL{
public:
    static sqlite3* openDatabase(const std::string& db_name, int try_times=0);

    static int closeDatabase(sqlite3* db);
    static sqlite3_stmt* prepareStatement(sqlite3 *db, const std::string sql);

    static std::string generateSelectQuery(
        const std::string& tableName, 
        const std::string& columnName, 
        const std::string& condition);

    template<typename Container>
    static std::string generateSelectQuery(
        const std::string& tableName, 
        const Container& columnNames,
        const Container& conditions);

    template<typename Container>
    // 对于 std::map 的特化版本
    static std::string generateSelectQuery(
            const std::string& tableName, 
            const Container& columnNames, 
            const std::map<std::string, std::string>& conditions);


    template<typename U>
    static U get_column_value(sqlite3_stmt* stmt, int i);
    



    template <typename T>
    static void readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&) = nullptr);
    
    // template <typename T>
    // static void readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&) = nullptr) {
    //     sqlite3_stmt* stmt = prepareStatement(db, sql);
    //     if (sqlite3_step(stmt) != SQLITE_ROW) {
    //         // 处理错误逻辑
    //         fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
    //         sqlite3_finalize(stmt);
    //         return;
    //     }

    //     if (handle != nullptr) {
    //         handle(result);
    //     }
    //     sqlite3_finalize(stmt);
    // }

private:



};