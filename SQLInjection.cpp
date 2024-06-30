// SQLInjection.cpp : This file contains the 'main' function. Program execution begins and ends there.
// This file uses input validation and parameterized queries to detect and prevent SQL Injections.
//

#include <algorithm>
#include <iostream>
#include <locale>
#include <tuple>
#include <vector>
#include <regex>

#include "sqlite3.h"

// DO NOT CHANGE
typedef std::tuple<std::string, std::string, std::string> user_record;
const std::string str_where = " where ";

// DO NOT CHANGE
static int callback(void* possible_vector, int argc, char** argv, char** azColName)
{
    if (possible_vector == NULL)
    { // no vector passed in, so just display the results
        for (int i = 0; i < argc; i++)
        {
            std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << std::endl;
    }
    else
    {
        std::vector< user_record >* rows =
            static_cast<std::vector< user_record > *>(possible_vector);

        rows->push_back(std::make_tuple(argv[0], argv[1], argv[2]));
    }
    return 0;
}

// DO NOT CHANGE
bool initialize_database(sqlite3* db)
{
    char* error_message = NULL;
    std::string sql = "CREATE TABLE USERS(" \
        "ID INT PRIMARY KEY     NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "PASSWORD       TEXT    NOT NULL);";

    int result = sqlite3_exec(db, sql.c_str(), callback, NULL, &error_message);
    if (result != SQLITE_OK)
    {
        std::cout << "Failed to create USERS table. ERROR = " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    std::cout << "USERS table created." << std::endl;

    // insert some dummy data
    sql = "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (1, 'Fred', 'Flinstone');" \
        "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (2, 'Barney', 'Rubble');" \
        "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (3, 'Wilma', 'Flinstone');" \
        "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (4, 'Betty', 'Rubble');";

    result = sqlite3_exec(db, sql.c_str(), callback, NULL, &error_message);
    if (result != SQLITE_OK)
    {
        std::cout << "Data failed to insert to USERS table. ERROR = " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }

    return true;
}

// Checks for common SQL injection patterns
bool isValidInput(const std::string& sql) {
    // Case insensitive regex pattern for SQL injection patterns
    std::regex pattern("(--|#|\\bor\\b.*=|\\bunion\\b|\\bdrop\\b|\\bexec\\b|\\bdeclare\\b|\\bcreate\\b|\\binsert\\b|\\bupdate\\b|\\bdelete\\b)", std::regex_constants::icase);

    // Search for the pattern in the input
    if (std::regex_search(sql, pattern)) {
        return false;
    }
    return true;
}

// Extracts the value after the NAME parameter from a SQL query
std::string extractParam(const std::string& sql) {
    std::string query_all = "SELECT * from USERS";
    std::string afterName;
    // Check for authorized query all
    if (sql != query_all) {
        // Extract value after name column
        std::string keyword = "NAME='";
        size_t pos = sql.find(keyword);
        if (pos != std::string::npos) {
            // Adjust the starting position to exclude the opening quote
            pos += keyword.size();
            // Find the position of the closing quote
            size_t endPos = sql.find("'", pos);
            if (endPos != std::string::npos) {
                // Extract the substring between the quotes
                afterName = sql.substr(pos, endPos - pos);
            }
        }
    }
    // return value after name column or query all string from authorized user
    std::string query = !afterName.empty() ? afterName : sql;
    return query;
}

// Run parameterized queries on a SQLite database
bool run_query(sqlite3* db, const std::string& sql, std::vector< user_record >& records)
{
    // clear any prior results
    records.clear();

    // Check for SQL injection patterns
    if (isValidInput(sql)) {
        std::cout << std::endl << "Valid Input." << std::endl;
    }
    else {
        std::cout << "\nSuspected SQL Injection." << std::endl;
        return false;
    }

    // Extract the value after the NAME parameter from the SQL query
    std::string param = extractParam(sql);

    sqlite3_stmt* stmt;

    // Prepare the SQL statement based on the extracted parameter
    std::string sql_prepared;
    if (param == "SELECT * from USERS") {
        sql_prepared = "SELECT * from USERS";
    }
    else {
        // Prepare a SELECT statement for specific user details
        sql_prepared = "SELECT ID, NAME, PASSWORD FROM USERS WHERE NAME=?";
    }
    // Execute the prepared SQL statement and handle any errors
    if (sqlite3_prepare_v2(db, sql_prepared.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cout << "Data failed to be queried from USERS table. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind the parameter to the SQL statement only if it is required
    if (stmt && param != "SELECT * from USERS") {
        if (sqlite3_bind_text(stmt, 1, param.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
            std::cout << "Failed to bind parameter. ERROR = " << sqlite3_errmsg(db) << "\n";
            sqlite3_finalize(stmt);
            return false;
        }
    }
    // Execute the query
    while (stmt && sqlite3_step(stmt) == SQLITE_ROW) {
        // Process the row
        user_record record;
        std::get<0>(record) = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::get<1>(record) = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::get<2>(record) = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        records.push_back(record);
    }

    // Finalize the statement to avoid resource leaks
    sqlite3_finalize(stmt);
    return true;
}


// DO NOT CHANGE
bool run_query_injection(sqlite3* db, const std::string& sql, std::vector< user_record >& records)
{
    std::string injectedSQL(sql);
    std::string localCopy(sql);

    // we work on the local copy because of the const
    std::transform(localCopy.begin(), localCopy.end(), localCopy.begin(), ::tolower);
    if (localCopy.find_last_of(str_where) >= 0)
    { // this sql has a where clause
        if (localCopy.back() == ';')
        { // smart SQL developer terminated with a semicolon - we can fix that!
            injectedSQL.pop_back();
        }

        switch (rand() % 4)
        {
        case 1:
            injectedSQL.append(" or 2=2;");
            break;
        case 2:
            injectedSQL.append(" or 'hi'='hi';");
            break;
        case 3:
            injectedSQL.append(" or 'hack'='hack';");
            break;
        case 0:
        default:
            injectedSQL.append(" or 1=1;");
            break;
        }
    }

    return run_query(db, injectedSQL, records);
}


// DO NOT CHANGE
void dump_results(const std::string& sql, const std::vector< user_record >& records)
{
    std::cout << std::endl << "SQL: " << sql << " ==> " << records.size() << " records found." << std::endl;

    for (auto record : records)
    {
        std::cout << "User: " << std::get<1>(record) << " [UID=" << std::get<0>(record) << " PWD=" << std::get<2>(record) << "]" << std::endl;
    }
}

// DO NOT CHANGE
void run_queries(sqlite3* db)
{
    char* error_message = NULL;

    std::vector< user_record > records;

    // query all
    std::string sql = "SELECT * from USERS";
    if (!run_query(db, sql, records)) return;
    dump_results(sql, records);

    //  query 1
    sql = "SELECT ID, NAME, PASSWORD FROM USERS WHERE NAME='Fred'";
    if (!run_query(db, sql, records)) return;
    dump_results(sql, records);

    //  run query 1 with injection 5 times
    for (auto i = 0; i < 5; ++i)
    {
        if (!run_query_injection(db, sql, records)) continue;
        dump_results(sql, records);
    }

}

// You can change main by adding stuff to it, but all of the existing code must remain, and be in the
// in the order called, and with none of this existing code placed into conditional statements
int main()
{
    // initialize random seed:
    srand(time(nullptr));

    int return_code = 0;
    std::cout << "SQL Injection Example" << std::endl;

    // the database handle
    sqlite3* db = NULL;
    char* error_message = NULL;

    // open the database connection
    int result = sqlite3_open(":memory:", &db);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to connect to the database and terminating. ERROR=" << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    std::cout << "Connected to the database." << std::endl;

    // initialize our database
    if (!initialize_database(db))
    {
        std::cout << "Database Initialization Failed. Terminating." << std::endl;
        return_code = -1;
    }
    else
    {
        run_queries(db);
    }

    // close the connection if opened
    if (db != NULL)
    {
        sqlite3_close(db);
    }

    return return_code;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
