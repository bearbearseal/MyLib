#include "Sqlite3.h"
#include <iostream>

using namespace std;

int main() {
    try{
        Sqlite3 sqlite("NcuConfig.db");
        auto result = move(sqlite.execute_query("Select * from ModbusIpPoint"));
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
        cout<<e.message<<endl;
    }
    return 0;
}