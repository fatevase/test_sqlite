#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#define CFG_STR_BIG_LEN 500
#define CFG_STR_SMALL_LEN 100

// temporary solution, will be rewritten ubing concepts in c++20
template <typename T, typename = void>
struct can_str : std::false_type {};
template <typename T>
struct can_str<T, 
   std::void_t<decltype(std::declval<std::string&>() += std::declval<T>())> 
> : std::true_type {};

template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, 
   std::void_t<decltype(std::begin(std::declval<T>()))
   , decltype(std::end(std::declval<T>()))> 
> : std::true_type {};

template <typename T, typename = void>
struct is_mapable : std::false_type {};

template <typename T>
struct is_mapable<T, 
   std::void_t<decltype(std::end(std::declval<T>()))
               , decltype(std::declval<T>().begin()->first)
               , decltype(std::declval<T>().begin()->second)> 
> : std::true_type {};

class CommSQL {
  public:
   // 打开数据库并返回 sqlite3 指针
   // 它会自动尝试打开数据库 3 次
   // 默认尝试次数为 0，即尝试打开数据库 3 次
   static sqlite3* openDatabase(const std::string& db_name, int try_times = MAX_OPEN_TIMES);

   // 检查数据库状态
   static bool checkDatabaseStatus(sqlite3* db);

   // 关闭数据库
   static int closeDatabase(sqlite3* db);

   // 准备 SQL 语句
   static sqlite3_stmt* prepareStatement(sqlite3* db, const std::string& sql);

   // 准备 SQL 语句（可变参数模板）
   template <typename... Args>
   static inline sqlite3_stmt* prepareStatement(sqlite3* db, const std::string& sql, Args&&... args);

   // 生成 SQL 查询语句（容器版本）
   template <typename C1, typename C2, typename C3=const char*>
   static std::string generateSelectQuery(const std::string& table_name
                                          , C1 const& column_names
                                          , C2 const& conditions
                                          , C3 delimiter = " AND ");

   // 读取单个数据
   template <typename T>
   static void readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&) = nullptr);

   // 读取数据
   template <typename T>
   static void readData(sqlite3* db, const std::string& sql, std::vector<T>& result, void (*handle)(T&) = nullptr);

   // 读取单个数据（预处理语句版本）
   template <typename T>
   static void readSingleData(sqlite3_stmt* stmt, T& result, void (*handle)(T&) = nullptr);

   // 读取数据（预处理语句版本）
   template <typename T>
   static void readData(sqlite3_stmt* stmt, std::vector<T>& result, void (*handle)(T&) = nullptr);

  private:
   static const int MAX_OPEN_TIMES = 3;

   // 获取列的值
   template <typename U>
   static U get_column_value(sqlite3_stmt* stmt, int i);

   // 绑定参数
   template <typename U>
   static void bind_value(sqlite3_stmt* stmt, int index, U&& value);

   template <typename T, typename D>
   static void appendItems(std::string& sql, T const& items, D const& delimiter);

   template <typename T>
   static void checkItems(T items);

   template <typename T>
   static bool isEmpty(T items);
};

template <typename... Args>
inline sqlite3_stmt* CommSQL::prepareStatement(sqlite3* db, const std::string& sql, Args&&... args) {
   sqlite3_stmt* stmt = nullptr;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if (rc != SQLITE_OK) {
      // 处理错误
      fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
   }

   int index = 1;
   (bind_value(stmt, index++, std::forward<Args>(args)), ...);
   return stmt;
}





template <typename C1, typename C2, typename C3>
std::string CommSQL::generateSelectQuery(const std::string& table_name
                                          , C1 const& column_names
                                          , C2 const& conditions
                                          , C3 delitimer) {
   std::string sql = "SELECT ";
   if (isEmpty(column_names)) {
      sql += " * ";
   } else {
      checkItems(column_names);
      appendItems(sql, column_names, ", ");
   }

   sql += " FROM " + table_name;

   if (!isEmpty(conditions)) {
      sql += " WHERE ";
      checkItems(conditions);
      appendItems(sql, conditions, delitimer);
   }
   return sql;
}

template <typename T>
void CommSQL::readSingleData(sqlite3* db, const std::string& sql, T& result, void (*handle)(T&)) {
   sqlite3_stmt* stmt = prepareStatement(db, sql);
   if (sqlite3_step(stmt) != SQLITE_ROW) {
      // 处理错误逻辑
      fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
      sqlite3_finalize(stmt);
      return;
   }
   result = get_column_value<T>(stmt, 0);
   if (handle != nullptr) {
      handle(result);
   }
   sqlite3_finalize(stmt);
}

template <typename T>
void CommSQL::readData(sqlite3* db, const std::string& sql, std::vector<T>& result, void (*handle)(T&)) {
   sqlite3_stmt* stmt = prepareStatement(db, sql);
   while (sqlite3_step(stmt) == SQLITE_ROW) {
      T data = get_column_value<T>(stmt, 0);
      if (handle != nullptr) {
         handle(data);
      }
      result.push_back(data);
   }
   sqlite3_finalize(stmt);
}

template <typename T>
void CommSQL::readSingleData(sqlite3_stmt* stmt, T& result, void (*handle)(T&)) {
   if (sqlite3_step(stmt) == SQLITE_ROW) {
      result = get_column_value<T>(stmt, 0);
      if (handle != nullptr) {
         handle(result);
      }
   }
}

template <typename T>
void CommSQL::readData(sqlite3_stmt* stmt, std::vector<T>& result, void (*handle)(T&)) {
   while (sqlite3_step(stmt) == SQLITE_ROW) {
      T data = get_column_value<T>(stmt, 0);
      if (handle != nullptr) {
         handle(data);
      }
      result.push_back(data);
   }
}


/*************
 * private  *
*/
template <typename T>
void CommSQL::checkItems(T items) {
   // Validate the items here.
   // This is just a placeholder and doesn't actually do any checks.
}

template <typename T>
bool CommSQL::isEmpty(T items) {
   if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
      return items == "" || std::string(items).find_first_not_of(' ') == std::string::npos;

   } else {
      return items.begin() == items.end();
   }
}

template <typename T, typename D>
void CommSQL::appendItems(std::string& sql, T const& items, D const& delimiter) {
   int _delimiter_iter = 0;
   if constexpr (std::is_constructible_v<std::string, T>) {
      sql += items;
   } else if constexpr (is_mapable<T>::value) {
      for (auto it = items.begin(); it != items.end(); ++it) {
         sql += it->first + "=" + it->second;
         if (std::next(it) != items.end()) {

            if constexpr (is_iterable<D>::value && !std::is_constructible_v<std::string, D>) {
               sql += delimiter[_delimiter_iter++];
            } else {
               sql += delimiter;
            }
         }
      }
   } else if constexpr (is_iterable<T>::value) {
      for (auto it = items.begin(); it != items.end(); ++it) {
         sql += *it;
         if (it != --items.end()) {
            if constexpr (is_iterable<D>::value && !std::is_constructible_v<std::string, D>) {
               sql += delimiter[_delimiter_iter++];
            } else {
               sql += delimiter;
            }
         }
      }
   } else {
      std::cout << "Unsupported type, only support string, vector<string>, map and iterable type" << std::endl;
   }
}


template <typename U>
U CommSQL::get_column_value(sqlite3_stmt* stmt, int i) {
   if constexpr (std::is_same_v<U, int>) {
      return sqlite3_column_int(stmt, i);
   } else if constexpr (std::is_same_v<U, double> || std::is_same_v<U, float>) {
      return sqlite3_column_double(stmt, i);
   } else if constexpr (std::is_same_v<U, std::string>) {
      auto text = sqlite3_column_text(stmt, i);
      return text ? std::string(reinterpret_cast<const char*>(text)) : "";
   } else if constexpr (std::is_constructible_v<std::string, U>) {
      return (const char*)sqlite3_column_text(stmt, i);
   } else {
      // 对于其他类型的处理
      // ...
      std::cerr << "Unsupported type, only support int, double, float, string and their vector type" << std::endl;
      // Utils::consoleLog(2, "Unsupported type, only support int, double, float, string and their vector type");
      return U{};  // 返回默认值，可根据需要进行修改
   }
}

template <typename U>
void CommSQL::bind_value(sqlite3_stmt* stmt, int index, U&& value) {
   if constexpr (std::is_same_v<std::decay_t<U>, int>) {
      sqlite3_bind_int(stmt, index, value);
   } else if constexpr (std::is_same_v<std::decay_t<U>, double> || std::is_same_v<std::decay_t<U>, float>) {
      sqlite3_bind_double(stmt, index, value);
   } else if constexpr (std::is_constructible_v<std::string, std::decay_t<U>>) {
      sqlite3_bind_text(stmt, index, std::string(value).c_str(), -1, SQLITE_TRANSIENT);

   // } else if constexpr (std::is_same_v<std::decay_t<U>, const char*>) {
   //    std::cout << " const char* path" << std::endl;
   //    sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);

   } else {
      // 处理错误
      std::cout << "CommSQL::bind_value:error type." << std::endl;
   }
}