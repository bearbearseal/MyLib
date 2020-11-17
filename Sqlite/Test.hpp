#include "Sqlite3Listener.h"
#include "Sqlite3.h"
#include <sqlite3.h>
#include <thread>
#include <chrono>

namespace Test {
    void run_sqlite3() {
        try{
            Sqlite3 sqlite("NcuConfig.db");
            auto result = sqlite.execute_query("Select * from ModbusIpPoint");
            for(size_t i=0; i<result->get_row_count(); ++i) {
                for(size_t j=0; j<result->get_column_count(); ++j) {
                    auto data = result->get_string(i, j);
                    if(data.first) {
                        printf( "%s |", data.second.c_str());
                    }
                    else {
                        printf(" NULL |");
                    }
                }
                printf("\n");
            }
        } catch(Sqlite3::Exception e) {
            printf("%s\n", e.message.c_str());
        }
        try {
            Sqlite3 sqlite("/var/sqlite/NcuAlarm.db");
            auto result = sqlite.execute_update("Insert into Alarm (Message)Values('dsfretrett')");
            printf("%s\n", result ? "Good" : "Bad");
        } catch(Sqlite3::Exception e) {
            printf("%s\n", e.message.c_str());
        }
    }

    void run_tcp_listener() {
        Sqlite3Listener sqlite3Listener(12345, "/var/sqlite/NcuConfig.db");
        sqlite3Listener.start();
        while(1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void use_raw() {
        sqlite3* theDb;
        int result = sqlite3_open("/var/sqlite/NcuConfig.db", &theDb);
        if(result != SQLITE_OK) {
            printf("Open failed.\n");
            return;
        }
        sqlite3_stmt* theStatement;
        const std::string statementString("Select Id, Compare from AlarmLogic");
        result = sqlite3_prepare_v2(theDb, statementString.c_str(), statementString.size(), &theStatement, NULL);
        if(result != SQLITE_OK) {
            printf("Prepare failed.\n");
            return;
        }
        result = sqlite3_column_count(theStatement);
        printf("Statement returns %d columns.\n", result);
        result = sqlite3_step(theStatement);
        for(int i=0; i<result; ++i) {
            int subResult = sqlite3_column_type(theStatement, i);
            switch(subResult) {
                case SQLITE_INTEGER:
                    printf("Column %d is integer.\n", i);
                    break;
                case SQLITE_FLOAT:
                    printf("Column %d is float.\n", i);
                    break;
                case SQLITE_BLOB:
                    printf("Column %d is blob.\n", i);
                    break;
                case SQLITE_NULL:
                    printf("Column %d is null.\n", i);
                    break;
                case SQLITE_TEXT:
                    printf("Column %d is text.\n", i);
                    break;
                default:
                    printf("Column %d is undefined.\n", i);
                    break;
            }
        }
        result = sqlite3_finalize(theStatement);
        result = sqlite3_close(theDb);
    }
}
