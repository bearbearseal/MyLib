#include "Sqlite3TcpListener.h"
#include "Sqlite3UdpListener.h"
#include "Sqlite3JsonTalker.h"
#include "Sqlite3.h"
#include <sqlite3.h>
#include <thread>
#include <chrono>

using namespace std;

namespace Test
{
    void run_sqlite3()
    {
        try
        {
            Sqlite3 sqlite("NcuConfig.db");
            auto result = sqlite.execute_query("Select * from ModbusIpPoint");
            for (size_t i = 0; i < result->get_row_count(); ++i)
            {
                for (size_t j = 0; j < result->get_column_count(); ++j)
                {
                    auto data = result->get_string(i, j);
                    if (data.first)
                    {
                        printf("%s |", data.second.c_str());
                    }
                    else
                    {
                        printf(" NULL |");
                    }
                }
                printf("\n");
            }
        }
        catch (Sqlite3::Exception e)
        {
            printf("%s\n", e.message.c_str());
        }
        try
        {
            Sqlite3 sqlite("/var/sqlite/NcuAlarm.db");
            auto result = sqlite.execute_update("Insert into Alarm (Message)Values('dsfretrett')");
            printf("%s\n", result ? "Good" : "Bad");
        }
        catch (Sqlite3::Exception e)
        {
            printf("%s\n", e.message.c_str());
        }
    }

    void run_tcp_udp_listener()
    {
        std::shared_ptr<Sqlite3JsonTalker> jsonTalker = std::make_shared<Sqlite3JsonTalker>("/var/sqlite/NcuConfig.db");
        Sqlite3TcpListener sqlite3TcpListener(12345, jsonTalker);
        Sqlite3UdpListener sqlite3UdpListener(12345, jsonTalker);
        sqlite3TcpListener.start();
        sqlite3UdpListener.start();
        while (1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void use_raw()
    {
        sqlite3 *theDb;
        int result = sqlite3_open("/var/sqlite/NcuConfig.db", &theDb);
        if (result != SQLITE_OK)
        {
            printf("Open failed.\n");
            return;
        }
        sqlite3_stmt *theStatement;
        const std::string statementString("Select Id, Compare from AlarmLogic");
        result = sqlite3_prepare_v2(theDb, statementString.c_str(), statementString.size(), &theStatement, NULL);
        if (result != SQLITE_OK)
        {
            printf("Prepare failed.\n");
            return;
        }
        result = sqlite3_column_count(theStatement);
        printf("Statement returns %d columns.\n", result);
        result = sqlite3_step(theStatement);
        for (int i = 0; i < result; ++i)
        {
            int subResult = sqlite3_column_type(theStatement, i);
            switch (subResult)
            {
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

    void test_bulk()
    {
        sqlite3 *db;
        int rc = sqlite3_open_v2("/var/sqlite/PointLog.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (rc != SQLITE_OK)
        {
            printf("Open also cannot, bye bye\n");
            return;
        }
        // sqlite3_mutex_enter(sqlite3_db_mutex(db));
        char *errorMessage;
        // sqlite3_exec(db, "PRAGMA synchronous=OFF", NULL, NULL, &errorMessage);
        // sqlite3_exec(db, "PRAGMA count_changes=OFF", NULL, NULL, &errorMessage);
        // sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL, NULL, &errorMessage);
        // sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL, NULL, &errorMessage);

        rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            printf("Cannot begin, bye bye\n");
            return;
        }

        const string szSQL = "INSERT INTO TableInfo (Name, BeginSec)VALUES(?,?)";

        sqlite3_stmt *stmt;
        //-1, so the szSQL should be null terminated, or can pass in strlen(szSQL)
        rc = sqlite3_prepare_v2(db, szSQL.c_str(), szSQL.size(), &stmt, NULL);

        if (rc == SQLITE_OK)
        {
            // -1 bwcause table1 is null terminated, else should pass in number of bytes
            // SQLITE_STATIC means the application would dispose the text manually.
            sqlite3_bind_text(stmt, 1, "table1", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, uint64_t(12345));
            int retVal = sqlite3_step(stmt);
            if (retVal != SQLITE_DONE)
            {
                printf("Commit Failed! %d\n", retVal);
            }
            sqlite3_reset(stmt);
            sqlite3_bind_text(stmt, 1, "table2", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, uint64_t(123456));
            retVal = sqlite3_step(stmt);
            if (retVal != SQLITE_DONE)
            {
                printf("Commit Failed! %d\n", retVal);
            }
            sqlite3_reset(stmt);
            sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
            /*
            for(int x=0;x<saData.size();x++){
                // bind the value
                sqlite3_bind_int(stmt, 1, saData[x].at("lastSAId"));
                std::string hash = saData[x].at("public_key");
                sqlite3_bind_text(stmt, 2,  hash.c_str(), strlen(hash.c_str()), 0);
                sqlite3_bind_int64(stmt, 3, saData[x].at("amount"));
                string amount_index = saData[x].at("amount_idx");
                sqlite3_bind_int(stmt, 4, atoi(amount_index.c_str()));

                int retVal = sqlite3_step(stmt);
                if (retVal != SQLITE_DONE)
                {
                    printf("Commit Failed! %d\n", retVal);
                }

                sqlite3_reset(stmt);
            }

            sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
            */
            sqlite3_finalize(stmt);
        }
        else
        {
            fprintf(stderr, "SQL error %d\n", rc);
        }
        sqlite3_close_v2(db);
    }

    void test_bulk_insert()
    {
        Sqlite3 sqlite("/var/sqlite/PointLog.db");
        vector<vector<variant<int64_t, double, string>>> parameters;
        parameters.resize(4);
        vector<variant<int64_t, double, string>>& entry1 = parameters[0];
        entry1.push_back(string("table0"));
        entry1.push_back(int64_t(1000));
        vector<variant<int64_t, double, string>>& entry2 = parameters[1];
        entry2.push_back(string("table1"));
        entry2.push_back(int64_t(1001));
        vector<variant<int64_t, double, string>>& entry3 = parameters[2];
        entry3.push_back(string("table2"));
        entry3.push_back(int64_t(1002));
        vector<variant<int64_t, double, string>>& entry4 = parameters[3];
        entry4.push_back(string("table3"));
        entry4.push_back(int64_t(1003));
        sqlite.execute_bulk_insert("INSERT Into TableInfo (Name, BeginSec) VALUES (?, ?)", parameters);
    }
}
