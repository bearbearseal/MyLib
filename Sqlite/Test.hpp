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
            auto sqlite = Sqlite3::open_database("NcuConfig.db");
            auto result = sqlite->execute_query("Select * from ModbusIpPoint");
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
            auto sqlite = Sqlite3::open_database("/var/sqlite/NcuAlarm.db");
            auto result = sqlite->execute_update("Insert into Alarm (Message)Values('dsfretrett')");
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
        auto sqlite = move(Sqlite3::open_database("/var/sqlite/PointLog.db"));
        {
            auto bulkInsert = move(sqlite->create_prepared_statement<int64_t, int64_t, int64_t, int64_t, double>("Insert Into table1 (Id, Device, Point, SecTime, Value) VALUES (?, ?, ?, ?, ?)"));
            printf("After create bulk.\n");
            if (bulkInsert != nullptr)
            {
                // printf("Has value.\n");
                bulkInsert->add_line(21, 1, 1, 100, 80.5);
                bulkInsert->add_line(22, 1, 2, 1200, 180.5);
                bulkInsert->add_line(23, 2, 1, 1030, 8.5);
                bulkInsert->add_line(24, 2, 3, 1004, 80.4);
                bulkInsert->quick_update(25, 4, 3, 1004, 90.4);
                bulkInsert->commit_insert();
            }
            else
            {
                printf("Empty value.\n");
            }
        }
        printf("Going to end.\n");
    }
}
