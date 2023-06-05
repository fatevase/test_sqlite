#include <iostream>
#include <fstream>

#include <any> //c++17

#include <test.hpp>

int main(){
  TestSQLite tsql;
  TestUtils tutils;
  // tsql.testReadSingleData();
  // tutils.testTestUtils();

  tsql.testStmt();

  return 0;

}

