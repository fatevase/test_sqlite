#include <sqlite3.h>

#include "CommSQL.h"
#include "OOPSQLite3.h"
#include "Utils.h"

#define SQLITE_DATA_PATH "db.sqlite3"
#define TABLE_NAME "client_deviceinfo"
struct EnvParaStruct {
   std::string path;
   int typeNum;
};

class TestSQLite {
  public:
   static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
      int i;
      for (i = 0; i < argc; i++) {
         printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      }
      printf("\n");
      return 0;
   }
   bool isFileExists(const char* file_name) {
      std::ifstream f(file_name);
      return f.good();
   }

   void showFunction(std::string id, std::string name, std::string age) {
      std::cout << "id: " << id << ", name: " << name << ", age: " << age << std::endl;
   }

   void testOOPSQLite(void callFun(std::string id, std::string name, std::string age)) {
      int ret = 0;
      OOPSQLite3DB db;
      db.open("mydataset.sqlite3");
      const std::string sql = "select * from mytable where id < (?)";
      OOPSQLite3Statement vm = db.compileStatement(sql);
      vm.bind(1, 3);

      OOPSQLite3Query q = vm.execQuery();
      std::string id, name, age;
      while (!q.eof()) {
         id = q.fieldValue(0);
         name = q.fieldValue(1);
         age = q.fieldValue(2);
         callFun(id, name, age);
         q.nextRow();
      }
      q.finalize();
      db.close();
   }
   static int callback1(void* NotUsed, int argc, char** argv, char** azColName) {
      int i;
      for (i = 0; i < argc; i++) {
         printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      }
      printf("\n");
      return 0;
   }

   void handleTypeNum(int& typeNum) {
      std::cout << "handle function :typeNum: " << typeNum << std::endl;
   }

   void testReadSingleData() {
      sqlite3* db = CommSQL::openDatabase(SQLITE_DATA_PATH);
      std::string sql = CommSQL::generateSelectQuery(TABLE_NAME, std::string("value"), std::string("key='ai_type_num'"));
      // int intValue = 0;
      float get_value;
      // char get_value[100];
      std::cout << sql << std::endl;
      CommSQL::readSingleData(db, sql, get_value);
      std::cout << "get value: " << get_value << std::endl;
   }

   template <typename T>
   void settingEnvPara(
       sqlite3* db,
       std::string key,
       T& result, std::string cast_type = "TEXT") {
      std::string column_name = "CAST (value AS " + cast_type + ")";

      std::string sql = CommSQL::generateSelectQuery(TABLE_NAME, column_name, "key='" + key + "'");
      std::cout << sql << std::endl;
      CommSQL::readSingleData(db, sql, result);
   }

   void testStmt() {
      sqlite3* db = CommSQL::openDatabase(SQLITE_DATA_PATH);
      std::string device_id = "";
      CommSQL::readSingleData(db, "select value from client_deviceinfo", device_id);
      std::cout << "device_id: " << device_id << std::endl;



      std::string sql = CommSQL::generateSelectQuery(TABLE_NAME, "value", std::vector{"key=?"});
      sqlite3_stmt* stmt = CommSQL::prepareStatement(db, sql, "device_id");
      std::cout << sqlite3_sql(stmt) << std::endl;
      device_id = "";      
      CommSQL::readSingleData(stmt, device_id);

      int rc = sqlite3_step(stmt);
      if (rc == SQLITE_ROW) {
         int typeNum = sqlite3_column_int(stmt, 0);
         std::cout << "typeNum: " << typeNum << std::endl;
         handleTypeNum(typeNum);
      }
      sqlite3_finalize(stmt);
      sqlite3_close(db);
   }
};

class TestUtils {
  public:
   void testTestUtils() {
      int year, month, day, hour, minute, second, millisecond;
      Utils::getCurrentYYYYMMDDHHMMSSMS(year, month, day, hour, minute, second, millisecond);
      std::cout << "year: " << year << ", month: " << month << ", day: " << day << ", hour: " << hour << ", minute: " << minute << ", second: " << second << ", millisecond: " << millisecond << std::endl;
      std::string current_time = Utils::getCurrentDateString();
      std::cout << "current_time: " << current_time << std::endl;

      std::unordered_map<std::string, std::string> umap;
      umap["key1"] = "value1";
      umap["key2"] = "value2";
      std::string json_map = Utils::mapToJson(umap);
      std::cout << "json_map: " << json_map << std::endl;

      std::unordered_map<std::string, std::any> any_map;
      any_map["key1"] = "value1";
      any_map["key2"] = 2;
      any_map["key3"] = 3.14;
      any_map["key4"] = true;
      any_map["key5"] = "123";
      any_map["key6"] = 'c';
      std::string json_any_map = Utils::mapToJson(any_map);
      std::cout << "json_any_map: " << json_any_map << std::endl;

      std::vector<std::any> any_vector;
      any_vector.push_back("value1");
      any_vector.push_back(2);
      any_vector.push_back(3.14);
      any_vector.push_back(true);

      std::string json_any_vector = Utils::vectorToJson(any_vector);
      std::cout << "json_any_vector: " << json_any_vector << std::endl;
   }
};