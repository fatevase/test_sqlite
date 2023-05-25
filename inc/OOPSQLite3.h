#include <iostream>
#include <sqlite3.h>

#define SQL_MAXSIZE    2048


class OOPSQLite3Query;
class OOPSQLite3Statement;
/***
 * 
 * 
****/
class OOPSQLite3DB{
public:
    OOPSQLite3DB();
    ~OOPSQLite3DB();


    void setBusyTimeout(int nMillisecs);

    void open(const std::string file_path);
    void close();
    bool tableExists(const std::string table_name); // Check if table exists

	int execDML(const std::string sql); // Insert, Delete, Update, Create, Alter(exec data manipulation language)
	int execScalar(const std::string sql); // Select count(*) from table_name
    OOPSQLite3Query execQuery(const std::string sql); // return object, using for select
    OOPSQLite3Statement compileStatement(const std::string sql); // return object, 

    int mn_busy_timeout; // Milliseconds, setting for sqlite3_busy_timeout()
    sqlite3* mp_db;    // SQLite connection, mp_db means "SQLite Main Pointer"


private:
    OOPSQLite3DB(const OOPSQLite3DB& db);
    OOPSQLite3DB& operator=(const OOPSQLite3DB& db);
    sqlite3_stmt* compile(const std::string sql);

	void checkDB();

};


/***
 * 
 * 
****/

class OOPSQLite3Statement{
public:

	OOPSQLite3Statement();
	OOPSQLite3Statement(const OOPSQLite3Statement& rstatement);
	OOPSQLite3Statement(sqlite3* p_db, sqlite3_stmt* p_vm);
	virtual ~OOPSQLite3Statement();

	OOPSQLite3Statement& operator=(const OOPSQLite3Statement& rStatement);

	int execDML();

	OOPSQLite3Query execQuery();

	// template<typename T>
	// void bind(int nparam, const T& nvalue);

	void bind(int nparam, std::string nvalue);
	void bind(int nparam, const int nvalue);
	void bind(int nparam, const double nvalue);
	void bind(int nparam, const unsigned char* nvalue, int nlen);
	void bindNull(int nparam);

	void reset();

	void finalize();

private:

	void checkDB();
	void checkVM();

	sqlite3* mp_db;
	sqlite3_stmt* mp_vm;
};

/***
 * 
 * 
****/

class OOPSQLite3Query{
public:
	OOPSQLite3Query();
    OOPSQLite3Query(const OOPSQLite3Query& rquery);
    OOPSQLite3Query(sqlite3* p_db, sqlite3_stmt* p_vm, bool eof, bool ownvm=true);
    virtual ~OOPSQLite3Query();

	OOPSQLite3Query& operator=(const OOPSQLite3Query& rquery);

	int numFields();
	int fieldIndex(const std::string& field_name);
	std::string fieldName(int field_index);

	std::string fieldDeclType(int field_index);
	int fieldDataType(int field_index);

	std::string fieldValue(int field_index);
	std::string fieldValue(const std::string& field_name);

	int getIntField(int field_index, int null_value=0);
	int getIntField(const std::string& field_name, int null_value=0);

	double getFloatField(int field_index, double null_value=0.0);
	double getFloatField(const std::string& field_name, double null_value=0.0);

	std::string getStringField(int field_index, const std::string& null_value="");
	std::string getStringField(const std::string& field_name, const std::string& null_value="");

	const unsigned char* getBlobField(int field_index, int& nlen);
	const unsigned char* getBlobField(const std::string& field_name, int& nlen);

	bool fieldIsNull(int field_index);
	bool fieldIsNull(const std::string& field_name);

	bool eof();
	void nextRow();

	void finalize();

private:
	
	void checkVM();

	sqlite3* mp_db;
	sqlite3_stmt* mp_vm;
	bool mb_eof;
	bool mb_ownvm;
	int mn_cols;

};