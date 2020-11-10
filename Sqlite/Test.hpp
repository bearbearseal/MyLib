#include "Sqlite3Listener.h"
#include "Sqlite3.h"
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
}