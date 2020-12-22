#include "Sqlite3JsonTalker.h"
#include "../../OtherLib/nlohmann/json.hpp"

using namespace std;

Sqlite3JsonTalker::Sqlite3JsonTalker(const std::string& dbName) : theDb(dbName) {

}

Sqlite3JsonTalker::~Sqlite3JsonTalker() {

}

nlohmann::json to_json(std::unique_ptr<Sqlite3::ResultSet>& resultSet) {
    nlohmann::json retVal;
    //retVal.array();
	nlohmann::json& columnName = retVal["Label"];
    for(size_t i=0; i<resultSet->get_column_count(); ++i) {
		columnName.push_back(resultSet->get_column_name(i));
	}
	nlohmann::json& columnData = retVal["Value"];
    for(size_t i=0; i<resultSet->get_row_count(); ++i) {
        nlohmann::json row;
        //row.array();
        for(size_t j=0; j<resultSet->get_column_count(); ++j) {
            row.push_back(resultSet->get_string(i, j).second);
        }
        columnData.push_back(row);
    }
    return retVal;
}

string Sqlite3JsonTalker::execute(const std::string& input) {
    nlohmann::json theReply;
    nlohmann::json theJson;
    try {
        theJson = nlohmann::json::parse(input);
    } catch(nlohmann::json::parse_error& error) {
		printf("Parse error: %s.\n", input.c_str());
        theReply["Status"] = "Bad";
		theReply["Message"] = "Json parse error";
		return theReply.dump() + '\n';
    }
    if(!theJson.is_object()) {
		theReply["Status"] = "Bad";
		theReply["Message"] = "Frame is not a json object";
		return theReply.dump() + '\n';
    }
	if (!theJson.contains(Property_Command)) {
		theReply["Status"] = "Bad";
		theReply["Message"] = "No command";
		return theReply.dump() + '\n';
	}
	if (!theJson.contains(Property_Statement)) {
		theReply["Status"] = "Bad";
		theReply["Message"] = "No statement";
		return theReply.dump() + '\n';
	}
	nlohmann::json& jCommand = theJson[Property_Command];
	nlohmann::json& jStatement = theJson[Property_Statement];
	if (!jCommand.is_string()) {
		theReply["Status"] = "Bad";
		theReply["Message"] = "Unknown command data type";
		return theReply.dump() + '\n';
	}
	if (!jStatement.is_string()) {
		theReply["Status"] = "Bad";
		theReply["Message"] = "Unknown statement data type";
		return theReply.dump() + '\n';
	}
    string command = jCommand.get<string>();
    string statement = jStatement.get<string>();
    if(!command.compare(Command_Query)) {
        std::unique_ptr<Sqlite3::ResultSet> result;
		{
			lock_guard<mutex> lock(dbMutex);
			try {
				result = theDb.execute_query(statement);
			} catch(Sqlite3::Exception e) {
				theReply["Status"] = "Bad";
				theReply["Message"] = e.message;
				return theReply.dump() + '\n';
			}
		}
        if(result == nullptr) {
            theReply["Status"] = "Bad";
            theReply["Message"] = "Error while executing statement";
            return theReply.dump() + '\n';
        }
        else {
            theReply["Status"] = "Good";
            theReply["Data"] = to_json(result);
            return theReply.dump() + '\n';
        }
    }
    else if(!command.compare(Command_Update)) {
        bool result;
		{
			lock_guard<mutex> lock(dbMutex);
			try {
				result = theDb.execute_update(statement);
			} catch(Sqlite3::Exception e) {
				theReply["Status"] = "Bad";
				theReply["Message"] = e.message;
				return theReply.dump() + '\n';
			}
		}
        if(result) {
            theReply["Status"] = "Good";
            return theReply.dump() + '\n';
        }
        else {
            theReply["Status"] = "Bad";
            theReply["Message"] = "Error while executing statement";
            return theReply.dump() + '\n';
        }
    }
	theReply["Status"] = "Bad";
	theReply["Message"] = "Unknown command";
	return theReply.dump() + '\n';
}
