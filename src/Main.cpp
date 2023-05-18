#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <OOPSQLite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
bool isFileExists(const char* file_name){
    std::ifstream f(file_name);
    return f.good();
}

void showFunction(std::string id, std::string name, std::string age){
  std::cout << "id: " << id << ", name: " << name << ", age: " << age << std::endl;
}

void testOOPSQLite(void callFun(std::string id, std::string name, std::string age)){
  int ret = 0;
  OOPSQLite3DB db;
  db.open("mydataset.sqlite3");
  const std::string sql = "select * from mytable";
  OOPSQLite3Query q = db.execQuery(sql);
	std::string id, name, age;
  while (!q.eof()){  
    id = q.fieldValue(0);
    name = q.fieldValue(1);
    age = q.fieldValue(2);
    callFun(id, name, age);
    q.nextRow();   
  }
  q.finalize(); db.close();
}

int main(){
  // testOOPSQLite(showFunction);
  const unsigned char *sql = "select * from mytable";
  std::cout << typeid(sql).name() << std::endl;

  return 0;
}

// int main() {
//   // Create a CppSQLite3U database object.
//   sqlite3 *db;
//   char *zErrMsg = 0;
//   int rc;
//   std::string sql_content;
//   std::string retStr;
//   std::string sqlite_path = "../mydataset.sqlite3";

// 	if (!isFileExists(sqlite_path.c_str())){
// 		std::cout << "File not found, try create new Dataset." << std::endl;
// 	}
//   // Open the database file.
//   if(sqlite3_open(sqlite_path.c_str(), &db)) {
//     std::cout << "Could not open database file." << std::endl;
//     return 1;
//   }

//   // Create a new table.
//   sql_content = "CREATE TABLE mytable (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)";
//   rc = sqlite3_exec(db, sql_content.c_str(), callback, 0, &zErrMsg);
//   if( rc!=SQLITE_OK ){
//     fprintf(stderr, "[Create]SQL error:  %s\n", zErrMsg);
//     sqlite3_free(zErrMsg);
//   }

//   // Insert a new record.
//   sql_content = "INSERT INTO mytable (name, age) VALUES ('John Doe', 23)";
//   rc = sqlite3_exec(db, sql_content.c_str(), callback, 0, &zErrMsg);
//   if( rc!=SQLITE_OK ){
//     fprintf(stderr, "[Inster]SQL error: %s\n", zErrMsg);
//     sqlite3_free(zErrMsg);
//   }

//   // Insert a series of records.
//   // Create a prepared statement.
//   const std::string sql = "INSERT INTO mytable (name, age) VALUES (?, ?)";
//   sqlite3_stmt *stmt = nullptr;
//   rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
//   if (rc != SQLITE_OK) {
//     fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
//     return 1;
//   }

//   // Bind the values.
//   int i;
//   for (i = 0; i < 10; i++) {
//     const std::string name = "Person-" + std::to_string(i);
//     int age = i + 18;
//     sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_TRANSIENT);
//     sqlite3_bind_int(stmt, 2, age);

//     // Execute the statement.
//     rc = sqlite3_step(stmt);
//     if (rc != SQLITE_DONE) {
//       fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
//       return 1;
//     }
//     // reset statement content.
//     sqlite3_reset(stmt);
//   }

//   // Close the statement.
//   sqlite3_finalize(stmt);

//   // Print the number of records that were inserted.
//   int rows_affected = sqlite3_changes(db);
//   printf("Number of rows inserted: %d\n", rows_affected);

//   // Update a record.
//   sql_content = "UPDATE mytable SET age = 25 WHERE name = 'John Doe'";
//   rc = sqlite3_exec(db, sql_content.c_str(), callback, 0, &zErrMsg);
//   if( rc!=SQLITE_OK ){
//     fprintf(stderr, "SQL error: %s\n", zErrMsg);
//     sqlite3_free(zErrMsg);
//   }

//   // Select records and display
//   sql_content = "SELECT * FROM mytable";
//   rc = sqlite3_exec(db, sql_content.c_str(), callback, 0, &zErrMsg);
//   if( rc!=SQLITE_OK ){
//     fprintf(stderr, "SQL error: %s\n", zErrMsg);
//     sqlite3_free(zErrMsg);
//   }

//   sqlite3_close(db);
//   std::cout << "Database file closed." << std::endl;
//   return 0;
// }
