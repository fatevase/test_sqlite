#include <OOPSQLite3.h>


/***********************************
 * OOPSQLite3DB
 * *********************************/

/*******Init Sqlite3 Database******/
OOPSQLite3DB::OOPSQLite3DB(){
    mp_db = 0;
    mn_busy_timeout = 60*1000;
}


OOPSQLite3DB::OOPSQLite3DB(const OOPSQLite3DB& db){
    mp_db = db.mp_db;
    mn_busy_timeout = db.mn_busy_timeout;
}

/********close sqlite3 database*************/
OOPSQLite3DB::~OOPSQLite3DB(){
    close();
}

/**************** move database instance **************/ 
//TODO: change to right move constructor c++15 
OOPSQLite3DB& OOPSQLite3DB::operator=(const OOPSQLite3DB& db){
    mp_db = db.mp_db;
    mn_busy_timeout = db.mn_busy_timeout;
    return *this;
}

/****************set max time to wait for database*************/
void OOPSQLite3DB::setBusyTimeout(int nMillisecs){
    mn_busy_timeout = nMillisecs;
    if (mp_db){
        sqlite3_busy_timeout(mp_db, mn_busy_timeout);
    }
}

/*************open database**************/
void OOPSQLite3DB::open(const std::string file_path){
    int nret = 0;
    nret = sqlite3_open(file_path.c_str(), &mp_db);
    if (nret != SQLITE_OK){
        //TODO: throw exception
        std::cout << "OOPSQLite3DB::open() - Error opening database " << file_path << std::endl;
        return;
    }

    setBusyTimeout(mn_busy_timeout);
}


/*************close database**************/
void OOPSQLite3DB::close(){
    if (mp_db){
        int nret = 0;
        nret = sqlite3_close(mp_db);
        if (nret != SQLITE_OK){
            //TODO: throw exception
            std::cout << "OOPSQLite3DB::close() - Error closing database " << std::endl;
            return;
        }
        mp_db = nullptr;
    }
}

bool OOPSQLite3DB::tableExists(const std::string table_name){
    const std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name="+table_name+";";
    int nret = execScalar(sql);
    return (nret > 0);
}

/********************compile sql statement******************/
sqlite3_stmt *OOPSQLite3DB::compile(const std::string sql){
    checkDB();
    sqlite3_stmt *p_vm = nullptr;
    int nret = sqlite3_prepare_v2(mp_db, sql.c_str(), -1, &p_vm, nullptr);
    if (nret != SQLITE_OK){
        //TODO: throw exception
        std::cout << "OOPSQLite3DB::compile() - Error compiling SQL statement: " << sql << std::endl;
        return nullptr;
    }
    return p_vm;
}

/**************execute sql statement**************/
int OOPSQLite3DB::execDML(const std::string sql){
    int nret = 0;
    sqlite3_stmt *p_vm; 
    do{
        p_vm = compile(sql); // optimize compile function to got sqlite3_stmt
        nret = sqlite3_step(p_vm);
        if(nret == SQLITE_ERROR){
            //TODO: throw exception, 
            std::cout << "OOPSQLite3DB::execDML() - Error executing SQL statement: " << sql << std::endl;
            break;
        }
    }while(nret == SQLITE_SCHEMA);
    return nret;
}

OOPSQLite3Query OOPSQLite3DB::execQuery(const std::string sql){
    int nret = 0;
    sqlite3_stmt *p_vm;
    do{ 
        p_vm = compile(sql);

        nret = sqlite3_step(p_vm);

        if (nret == SQLITE_DONE)
        {	// no rows
                return OOPSQLite3Query(mp_db, p_vm, true/*eof*/);
        }
        else if (nret == SQLITE_ROW)
        {	// at least 1 row
                return OOPSQLite3Query(mp_db, p_vm, false/*eof*/);
        }
        nret = sqlite3_finalize(p_vm);
    }while( nret == SQLITE_SCHEMA ); 
    
    std::string error_msg = sqlite3_errmsg(mp_db);
    std::cout << "OOPSQLite3DB::execQuery() - Error executing SQL statement: " << error_msg << std::endl;
    throw("OOPSQLite3DB::execQuery() - Error executing SQL statement");
}


int OOPSQLite3DB::execScalar(const std::string sql){
    OOPSQLite3Query query = execQuery(sql);
    if ( query.eof() || query.numFields() < 1 ){
        return 0;
    }
    return query.getIntField(0);
}

OOPSQLite3Statement OOPSQLite3DB::compileStatement(const std::string sql){
    sqlite3_stmt *p_vm = compile(sql);
    return OOPSQLite3Statement(mp_db, p_vm);
}

void OOPSQLite3DB::checkDB(){
    if (!mp_db){
        std::cout << "OOPSQLite3DB::checkDB() - Database is not open" << std::endl;
        throw("error");
        return;
    }
}

/*************
 * OOPSQLIte3Statement
 * *****************/


OOPSQLite3Statement::OOPSQLite3Statement(){
    mp_db = 0;
    mp_vm = 0;
}


OOPSQLite3Statement::OOPSQLite3Statement(sqlite3 *db, sqlite3_stmt *vm){
    mp_db = db;
    mp_vm = vm;
}


OOPSQLite3Statement::~OOPSQLite3Statement(){
    try{
        finalize();
    }
    catch(...){}
}

OOPSQLite3Statement& OOPSQLite3Statement::operator=(const OOPSQLite3Statement& st){
    mp_db = st.mp_db;
    mp_vm = st.mp_vm;
    const_cast<OOPSQLite3Statement&>(st).mp_vm = 0; //singlteon
    return *this;
}

int OOPSQLite3Statement::execDML(){
    //TODO: optimize db and vm, save in OOPSQLite3DB.
    checkDB();
    checkVM();

    int nret = 0;
    do{
        nret = sqlite3_step(mp_vm);
    }while(nret == SQLITE_SCHEMA);
    
    if (nret != SQLITE_OK && nret != SQLITE_DONE && nret != SQLITE_ROW){
        std::cout << "OOPSQLite3Statement::execDML() - Error executing SQL statement" << std::endl;
        return -1;
    }
    nret = sqlite3_reset(mp_vm);
    return nret;
}

/*************************bind function for multiple types*********************/
//TODO： optimize bind function, use template

// template<typename T>
// void OOPSQLite3Statement::bind(int nparam, const T& nvalue){
//     auto typename = typeid(nvalues).name();
//     std::unorderd_map<std::string std::function<int()> > func_dict;
//     func_dict["i"] = sqlite3_bind_int;
//     func_dict["d"] = sqlite3_bind_double;
//     func_dict["s"] = sqlite3_bind_text;
//     func_dict["b"] = sqlite3_bind_blob;

// }

void OOPSQLite3Statement::bind(int nparam, const std::string nvalue){
    checkVM();
    int nret = sqlite3_bind_text(mp_vm, nparam, nvalue.c_str(), -1, SQLITE_TRANSIENT);
    if (nret != SQLITE_OK){
        std::cout << "OOPSQLite3Statement::bind() - Error binding string value" << std::endl;
        return;
    }
}

void OOPSQLite3Statement::bind(int nparam, const int nvalue){
    checkVM();
    int nret = sqlite3_bind_int(mp_vm, nparam, nvalue);
    if (nret != SQLITE_OK){
        std::cout << "OOPSQLite3Statement::bind() - Error binding int value" << std::endl;
        return;
    }
}

void OOPSQLite3Statement::bind(int nparam, const double value){
    checkVM();
    int nret = sqlite3_bind_double(mp_vm, nparam, value);
    if (nret != SQLITE_OK){
        std::cout << "OOPSQLite3Statement::bind() - Error binding double value" << std::endl;
        return;
    }
}

void OOPSQLite3Statement::bind(int nparam, const unsigned char *value, int nlen){
    checkVM();
    int nret = sqlite3_bind_blob(mp_vm, nparam, value, nlen, SQLITE_TRANSIENT);
    if (nret != SQLITE_OK){
        std::cout << "OOPSQLite3Statement::bind() - Error binding blob value" << std::endl;
        return;
    }
}

void OOPSQLite3Statement::bindNull(int nparam){
    checkVM();
    int nret = sqlite3_bind_null(mp_vm, nparam);
    if (nret != SQLITE_OK){
        std::cout << "OOPSQLite3Statement::bindNull() - Error binding NULL value" << std::endl;
        return;
    }
}

void OOPSQLite3Statement::reset(){
    if (mp_vm){
        int nret = sqlite3_reset(mp_vm);
        if (nret != SQLITE_OK){
            std::cout << "OOPSQLite3Statement::reset() - Error resetting SQL statement" << std::endl;
            return;
        }
    }
}

void OOPSQLite3Statement::finalize(){
    if (mp_vm){
        int nret = sqlite3_finalize(mp_vm);
        mp_vm = 0;
        if (nret != SQLITE_OK){
            std::cout << "OOPSQLite3Statement::finalize() - Error finalizing SQL statement" << std::endl;
            return;
        }
    }
}

void OOPSQLite3Statement::checkDB(){
    if (!mp_db){
        std::cout << "OOPSQLite3Statement::checkDB() - Database is not open" << std::endl;
        throw("error");
        return;
    }
}

void OOPSQLite3Statement::checkVM(){
    if (!mp_vm){
        std::cout << "OOPSQLite3Statement::checkVM() - Null Virtual Machine" << std::endl;
        throw("error");
        return;
    }
}


/*************
 * OOPSQLIte3Query
 * *****************/

OOPSQLite3Query::OOPSQLite3Query(){
    mp_db = 0;
    mp_vm = 0;
    mb_eof = true;
    mn_cols = 0;
}

OOPSQLite3Query::OOPSQLite3Query(sqlite3 *p_db, sqlite3_stmt *p_vm, bool eof, bool ownvm){
    mp_db = p_db;
    mp_vm = p_vm;
    mb_eof = eof;
    mn_cols = sqlite3_column_count(mp_vm);
    mb_ownvm = ownvm;
}

OOPSQLite3Query::OOPSQLite3Query(const OOPSQLite3Query& rquery){
    mp_vm = rquery.mp_vm;
    const_cast<OOPSQLite3Query&>(rquery).mp_vm = 0; //singleton
    mb_eof = rquery.mb_eof;
    mn_cols = rquery.mn_cols;
    mb_ownvm = rquery.mb_ownvm;
}

OOPSQLite3Query::~OOPSQLite3Query(){
    try{
        finalize();
    }
    catch(...){}
}

OOPSQLite3Query& OOPSQLite3Query::operator=(const OOPSQLite3Query& rquery){
    try{
        finalize();
    }catch (...){ }
    mp_vm = rquery.mp_vm;
    const_cast<OOPSQLite3Query&>(rquery).mp_vm = 0; //singleton
    mb_eof = rquery.mb_eof;
    mn_cols = rquery.mn_cols;
    mb_ownvm = rquery.mb_ownvm;
    return *this;
}

int OOPSQLite3Query::numFields(){
    checkVM();
    return mn_cols;
}

int OOPSQLite3Query::fieldIndex(const std::string& field_name){
    checkVM();
    if (field_name.empty()){
        std::cout << "OOPSQLite3Query::fieldIndex() - Empty field requested" << std::endl;
        return -1;
    }

    for (int nfield = 0; nfield < mn_cols; nfield++){
        std::string temp = sqlite3_column_name(mp_vm, nfield);
        if (field_name == temp){
            return nfield;
        }
    }
    std::cout << "OOPSQLite3Query::fieldIndex() - Invalid field name requested" << std::endl;
    return -1;
}


std::string OOPSQLite3Query::fieldName(int field_index){
    checkVM();
    if(field_index < 0 || field_index > mn_cols-1){
        std::cout << "OOPSQLite3Query::fieldName() - Invalid field index requested" << std::endl;
        return "";
    }
    return sqlite3_column_name(mp_vm, field_index);
}



std::string OOPSQLite3Query::fieldDeclType(int field_index){
    checkVM();
    if(field_index < 0 || field_index > mn_cols-1){
        std::cout << "OOPSQLite3Query::fieldDeclType() - Invalid field index requested" << std::endl;
        return "";
    }
    return sqlite3_column_decltype(mp_vm, field_index);
}


int OOPSQLite3Query::fieldDataType(int field_index){
    checkVM();
    if(field_index < 0 || field_index > mn_cols-1){
        std::cout << "OOPSQLite3Query::fieldDataType() - Invalid field index requested" << std::endl;
        return -1;
    }
    return sqlite3_column_type(mp_vm, field_index);
}


std::string OOPSQLite3Query::fieldValue(int field_index){
    checkVM();
    if(field_index < 0 || field_index > mn_cols-1){
        std::cout << "OOPSQLite3Query::fieldValue() - Invalid field index requested" << std::endl;
        return "";
    }
    return (const char *)sqlite3_column_text(mp_vm, field_index);
}

std::string OOPSQLite3Query::fieldValue(const std::string& field_name){
    checkVM();
    int nfield = fieldIndex(field_name);
    return (const char *)sqlite3_column_text(mp_vm, nfield);
}


/***********************************/

int OOPSQLite3Query::getIntField(int field_index, int null_value){
    checkVM();
    if (fieldDataType(field_index) == SQLITE_NULL){
        return null_value;
    }
    else{
        return sqlite3_column_int(mp_vm, field_index);
    }
}

int OOPSQLite3Query::getIntField(const std::string& field_name, int null_value){
    checkVM();
    int field_index = fieldIndex(field_name);
    return getIntField(field_index, null_value);
}

double OOPSQLite3Query::getFloatField(int field_index, double null_value){
    checkVM();
    if (fieldDataType(field_index) == SQLITE_NULL){
        return null_value;
    }
    else{
        return sqlite3_column_double(mp_vm, field_index);
    }
}

double OOPSQLite3Query::getFloatField(const std::string& field_name, double null_value){
    checkVM();
    int field_index = fieldIndex(field_name);
    return getFloatField(field_index, null_value);
}

std::string OOPSQLite3Query::getStringField(int field_index, const std::string& null_value){
    checkVM();
    if (fieldDataType(field_index) == SQLITE_NULL){
        return null_value;
    }
    else{
        return (const char *)sqlite3_column_text(mp_vm, field_index);
    }
}

std::string OOPSQLite3Query::getStringField(const std::string& field_name, const std::string& null_value){
    checkVM();
    int field_index = fieldIndex(field_name);
    return getStringField(field_index, null_value);
}

const unsigned char* OOPSQLite3Query::getBlobField(int field_index, int& size){
    checkVM();
    if (field_index < 0 || field_index > mn_cols-1){
        std::cout << "OOPSQLite3Query::getBlobField() - Invalid field index requested" << std::endl;
        return 0;
    }
    size = sqlite3_column_bytes(mp_vm, field_index);
    return (const unsigned char *)sqlite3_column_blob(mp_vm, field_index);
}


const unsigned char* OOPSQLite3Query::getBlobField(const std::string& field_name, int& size){
    checkVM();
    int field_index = fieldIndex(field_name);
    return getBlobField(field_index, size);
}


bool OOPSQLite3Query::fieldIsNull(int field_index){
    checkVM();
    return (fieldDataType(field_index) == SQLITE_NULL);
}


void OOPSQLite3Query::checkVM(){
    if (mp_vm == 0){
        std::cout << "OOPSQLite3Query::checkVM() - Invalid VM" << std::endl;
        throw std::exception();
    }
}

bool OOPSQLite3Query::eof(){
    return mb_eof;
}

void OOPSQLite3Query::nextRow(){
    checkVM();
    int nret = sqlite3_step(mp_vm);
    if (nret == SQLITE_DONE){
        mb_eof = true;
    }
    else if (nret == SQLITE_ROW){
        mb_eof = false;
    }
    else{
        nret = sqlite3_finalize(mp_vm);
        std::cout << "OOPSQLite3Query::nextRow() - Error executing query" << std::endl;
        throw std::exception();
    }
}

void OOPSQLite3Query::finalize(){
    checkVM();
    if(!mp_vm || !mb_ownvm) return ;
    int nret = sqlite3_finalize(mp_vm);
    if (nret != SQLITE_OK){
        std::cout << "OOPSQLite3Query::finalize() - Error finalizing query" << std::endl;
        throw std::exception();
    }
    mp_vm = 0;
}
