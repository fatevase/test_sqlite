#include <CommSQL.h>

#include <sqlite3.h>

#include <string>
#include <vector>

sqlite3* CommSQL::openDatabase(
    const std::string& db_name, int try_times){
    const int MAX_OPEN_TIMES = 3;
    sqlite3* db;
    int rc = sqlite3_open(db_name.c_str(), &db);
    if(rc != SQLITE_OK){
        if (try_times > MAX_OPEN_TIMES) {
            // 在这里，你可以选择抛出一个异常或者执行其他的错误处理程序
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        }
        else {
            return openDatabase(db_name, try_times+1);
        }
    }
    return db;
}

int CommSQL::closeDatabase(sqlite3* db){
    return sqlite3_close(db);
}

sqlite3_stmt* CommSQL::prepareStatement(sqlite3 *db,const std::string sql){
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if(rc != SQLITE_OK){
        // 这里可以处理错误
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
    }
    return stmt;
}

std::string CommSQL::generateSelectQuery(
    const std::string& table_name, 
    const std::string& column_name, 
    const std::string& condition){

    std::string query = "SELECT " + column_name + " FROM " + table_name;
    if(!condition.empty()){
        query += " WHERE " + condition;
    }
    return query;
}

// 默认的模板实现，对于 std::vector 和 std::array 等顺序容器
template<typename Container>
std::string CommSQL::generateSelectQuery(
    const std::string& table_name, 
    const Container& column_names, 
    const Container& conditions){
    std::string query = "SELECT ";
    for(const auto& column : column_names){
        query += column + ", ";
    }
    query = query.substr(0, query.size()-2); // Remove the last ", "
    query += " FROM " + table_name;
    if(!conditions.empty()){
        query += " WHERE ";
        for(const auto& condition : conditions){
            query += condition + " AND ";
        }
        query = query.substr(0, query.size()-5); // Remove the last " AND "
    }
    return query;
}



template<typename Container>
// 对于 std::map 的特化版本
std::string CommSQL::generateSelectQuery(
    const std::string& table_name, 
    const Container& column_names, 
    const std::map<std::string, std::string>& conditions){
    std::string query = "SELECT ";
    for(const auto& column : column_names){
        query += column + ", ";
    }
    query = query.substr(0, query.size()-2); // Remove the last ", "
    query += " FROM " + table_name;
    if(!conditions.empty()){
        query += " WHERE ";
        for(const auto& condition : conditions){
            query += condition.first + " = " + condition.second + " AND ";
        }
        query = query.substr(0, query.size()-5); // Remove the last " AND "
    }
    return query;
}





// template<>
// int CommSQL::get_column_value<int>(sqlite3_stmt* stmt, int i){
//     return sqlite3_column_int(stmt, i);
// }

// template<>
// std::string CommSQL::get_column_value<std::string>(sqlite3_stmt* stmt, int i){
//     const unsigned char* text = sqlite3_column_text(stmt, i);
//     return text ? reinterpret_cast<const char*>(text) : "";
// }



// // 实例化模板函数的定义
// template <typename T>
// void CommSQL::readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&)) {
//     sqlite3_stmt* stmt = CommSQL::prepareStatement(db, sql);
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
// // template 需要实现在h文件中，或者
// // 显式实例化模板函数, 需要将常用的类型都实例化一遍，避免编译器在链接时找不到定义
// template void CommSQL::readSingleData<int>(sqlite3* db, const std::string& sql, int& result, void (*handle)(int&));
// template void CommSQL::readSingleData<std::string>(sqlite3* db, const std::string& sql, std::string& result, void (*handle)(std::string&));
