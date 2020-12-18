#include "FileBrowser.h"
#include "FileIOer.h"
#include "../StringManipulator/StringManipulator.h"
#include <iostream>

using namespace std;

FileBrowser::FileBrowser(const string& _rootFolder) : rootPath(_rootFolder) {
    currentPath.assign(rootPath);
    //cout<<_rootFolder<<endl;
}

FileBrowser::~FileBrowser() {

}

string FileBrowser::handle_open_folder(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_OpenFolder;
    if(!input.contains(Property_Name)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "OpenFolder need a Name";
        return reply.dump();
    }
    const nlohmann::json& jName = input[Property_Name];
    if(!jName.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Name is not a string";
        return reply.dump();
    }
    string sName = jName.get<string>();
    //currentPath.has_relative_path();
    sName = currentPath.u8string() + "/" + sName;
    filesystem::path newPath(sName);
    if(!filesystem::is_directory(newPath)) {
        reply[Property_Status] = "Bad";
        string message = "Folder ";
        message += sName + " not found";
        reply[Property_Message] = message;
        return reply.dump();
    }
    currentPath = newPath;
    reply[Property_Status] = "Good";
    reply[Property_Name] = sName;
    return reply.dump();
}

string FileBrowser::handle_read_file_utf8(const nlohmann::json& input) {
    nlohmann::json reply;
    //cout<<input.dump()<<endl;
    reply[Property_Command] = Command_ReadFileUtf8;
    if(!input.contains(Property_Name)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "ReadFile need a Name";
        return reply.dump();
    }
    const nlohmann::json& jName = input[Property_Name];
    if(!jName.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Name is not a string";
        return reply.dump();
    }
    string sName = jName.get<string>();
    //currentPath.has_relative_path();
    sName = currentPath.u8string() + "/" + sName;
    filesystem::path newPath(sName);
    if(!filesystem::is_regular_file(newPath)) {
        reply[Property_Status] = "Bad";
        string message = "File ";
        message += sName + " not found";
        reply[Property_Message] = message;
        return reply.dump();
    }
    FileIOer aFile(sName);
    string content = aFile.read_data();
    reply[Property_Status] = "Good";
    reply[Property_Name] = sName;
    reply[Property_FileContent] = content;
    return reply.dump();
}

string FileBrowser::handle_write_file_utf8(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_WriteFileUtf8;
    if(!input.contains(Property_Name)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "WriteFile need a Name";
        return reply.dump();
    }
    const nlohmann::json& jName = input[Property_Name];
    if(!jName.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Name is not a string";
        return reply.dump();
    }
    if(!input.contains(Property_FileContent)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "WriteFile needs Content";
        return reply.dump();
    }
    const nlohmann::json& jContent = input[Property_FileContent];
    if(!jContent.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Content is not a string";
        return reply.dump();
    }
    string sName = jName.get<string>();
    sName = currentPath.u8string() + "/" + sName;
    filesystem::path newPath(sName);
    if(filesystem::is_regular_file(newPath)) {
        FileIOer aFile(sName);
        string unformatted = jContent.get<string>();
        aFile.write_new(StringManipulator::Formatter::to_unformatted(unformatted));
        reply[Property_Status] = "Good";
        reply[Property_Name] = sName;
        reply[Property_Message] = "File writen";
        return reply.dump();
    }
    else if(filesystem::is_empty(newPath)) {
        FileIOer aFile(sName);
        string unformatted = jContent.get<string>();
        aFile.write_new(StringManipulator::Formatter::to_unformatted(unformatted));
        reply[Property_Status] = "Good";
        reply[Property_Name] = sName;
        reply[Property_Message] = "File created";
        return reply.dump();
    }
    reply[Property_Status] = "Bad";
    string message = "File ";
    message += sName + " is not writable";
    reply[Property_Message] = message;
    return reply.dump();
}

string FileBrowser::handle_delete(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_Delete;
    if(!input.contains(Property_Name)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Delete need a Name";
        return reply.dump();
    }
    const nlohmann::json& jName = input[Property_Name];
    if(!jName.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Name is not a string";
        return reply.dump();
    }
    string sName = currentPath.u8string();
    sName += "/" + jName.get<string>();
    if(filesystem::remove(sName)) {
        reply[Property_Status] = "Good";
        reply[Property_Name] = sName;
        return reply.dump();
    }
    reply[Property_Status] = "Bad";
    reply[Property_Message] = "Invalid target";
    return reply.dump();
}

string FileBrowser::handle_rename(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_Rename;
    if(!input.contains(Property_Name)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Rename need a Name";
        return reply.dump();
    }
    const nlohmann::json& jName = input[Property_Name];
    if(!jName.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Name is not a string";
        return reply.dump();
    }
    if(!input.contains(Property_NewName)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Rename need a NewName";
        return reply.dump();
    }
    const nlohmann::json& jNewName = input[Property_NewName];
    if(!jNewName.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "NewName is not a string";
        return reply.dump();
    }
    string sOldName = currentPath.u8string();
    sOldName += "/" + jName.get<string>();
    string sNewName = currentPath.u8string();
    sNewName += "/" + jNewName.get<string>();
    std::error_code errorCode;
    filesystem::rename(sOldName, sNewName, errorCode);
    if(errorCode.value()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = errorCode.message();
        return reply.dump();
    }
    reply[Property_Status] = "Good";
    reply[Property_NewName] = sNewName;
    return reply.dump();
}

string FileBrowser::handle_to_parent_folder(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_ToParentFolder;
    if(currentPath == rootPath) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Access denied";
        return reply.dump();
    }
    currentPath = currentPath.parent_path();
    reply[Property_Status] = "Good";
    reply[Property_Name] = currentPath.u8string();
    return reply.dump();
}

string FileBrowser::handle_show_content(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_ShowContent;
    filesystem::path targetPath = currentPath;
    reply[Property_Status] = "Good";
    if(input.contains(Property_Path)) {
        const nlohmann::json& jPath = input[Property_Path];
        if(!jPath.is_string()) {
            reply[Property_Status] = "Bad";
            reply[Property_Message] = "Path is not a string";
            return reply.dump();
        }
        string sPath = jPath.get<string>();
        if(sPath.find("./") == string::npos){
            targetPath.assign(targetPath.u8string() + "/" + sPath);
            if(!filesystem::exists(targetPath)) {
                reply[Property_Status] = "Bad";
                reply[Property_Message] = "Invalid path";
                return reply.dump();
            }
        }
        else {
            reply[Property_Status] = "Bad";
            reply[Property_Message] = "Invalid path";
            return reply.dump();
        }
    }
    //cout<<"Going to show content of "<<targetPath<<endl;
    nlohmann::json& theContent = reply[Property_FolderContent];
    for(auto it = filesystem::directory_iterator(targetPath); it != filesystem::directory_iterator(); ++it) {
        nlohmann::json entry;
        entry[Property_Name] = filesystem::relative(it->path(), rootPath).u8string();
        //entry[Property_Name] = it->path().u8string();
        if(it->is_regular_file()) {
            entry["Type"] = "Regular";
        }
        else if(it->is_directory()) {
            entry["Type"] = "Directory";
        }
        else if(it->is_block_file()) {
            entry["Type"] = "Block";
        }
        else if(it->is_character_file()) {
            entry["Type"] = "Character";
        }
        else {
            entry["Type"] = "Unknown";
        }
        theContent.push_back(entry);
    }
    return reply.dump();
}

string FileBrowser::handle_show_current_path(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_CurrentPath;
    reply[Property_Status] = "Good";
    reply[Property_Name] = currentPath.u8string();
    return reply.dump();
}

string FileBrowser::handle_to_root(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_ToRoot;
    reply[Property_Status] = "Good";
    //currentPath.assign(rootPath);
    currentPath = rootPath;
    reply[Property_Name] = rootPath;
    return reply.dump();
}

string FileBrowser::handle_create_folder(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_CreateFolder;
    if(!input.contains(Property_Name)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "CreateFolder need a Name";
        return reply.dump();
    }
    const nlohmann::json& name = input[Property_Name];
    if(!name.is_string()) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Name is not string";
        return reply.dump();
    }
    string newName = currentPath.u8string();
    newName += "/";
    newName += name.get<string>();
    if(!filesystem::create_directory(newName)) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "Folder creation failed";
        return reply.dump();
    }
    reply[Property_Status] = "Good";
    return reply.dump();
}

string FileBrowser::handle_create_file(const nlohmann::json& input) {
    nlohmann::json reply;
    reply[Property_Command] = Command_CreateFile;
    string filename = currentPath.u8string();
    if(!input.contains(Property_Name)) {
        filename += "/NewFile";
        string postFix;
        size_t i = 1;
        while(filesystem::exists(filename + postFix)) {
            postFix = "_";
            postFix += to_string(i);
            ++i;
        }
        filename += postFix;
    }
    else {
        const nlohmann::json& name = input[Property_Name];
        if(!name.is_string()) {
            reply[Property_Status] = "Bad";
            reply[Property_Message] = "Name is not string";
            return reply.dump();
        }
        filename += "/";
        filename += name.get<string>();
        if(filesystem::exists(filename)) {
            reply[Property_Status] = "Bad";
            reply[Property_Message] = "File exist";
            return reply.dump();
        }
    }
    FileIOer aFile(filename);
    if(!aFile.write_new("")) {
        reply[Property_Status] = "Bad";
        reply[Property_Message] = "File creation failed";
        return reply.dump();
    }
    reply[Property_Status] = "Good";
    return reply.dump();
}

string FileBrowser::parse_command(const string& input) {
    nlohmann::json theJson;
    nlohmann::json theReply;
    try {
        theJson = nlohmann::json::parse(input);
    } catch(nlohmann::json::parse_error& error) {
		printf("Parse error: %s.\n", input.c_str());
        theReply[Property_Status] = "Bad";
		theReply[Property_Message] = "Json parse error";
		return theReply.dump() + '\n';
    }
    if(!theJson.is_object()) {
		theReply[Property_Status] = "Bad";
		theReply[Property_Message] = "Frame is not a json object";
		return theReply.dump() + '\n';
    }
	if (!theJson.contains(Property_Command)) {
		theReply[Property_Status] = "Bad";
		theReply[Property_Message] = "No command";
		return theReply.dump() + '\n';
	}
    nlohmann::json& jCommand = theJson[Property_Command];
    if(!jCommand.is_string()) {
        theReply[Property_Status] = "Bad";
        theReply[Property_Message] = "Command is not string type";
		return theReply.dump() + '\n';
    }
    string sCommand = jCommand.get<string>();
    if(!sCommand.compare(Command_OpenFolder)) {
        return handle_open_folder(theJson);
    }
    else if(!sCommand.compare(Command_ReadFileUtf8)) {
        return handle_read_file_utf8(theJson);
    }
    else if(!sCommand.compare(Command_WriteFileUtf8)) {
        return handle_write_file_utf8(theJson);
    }
    else if(!sCommand.compare(Command_Delete)) {
        return handle_delete(theJson);
    }
    else if(!sCommand.compare(Command_Rename)) {
        return handle_rename(theJson);
    }
    else if(!sCommand.compare(Command_ToParentFolder)) {
        return handle_to_parent_folder(theJson);
    }
    else if(!sCommand.compare(Command_ShowContent)) {
        return handle_show_content(theJson);
    }
    else if(!sCommand.compare(Command_CurrentPath)) {
        return handle_show_current_path(theJson);
    }
    else if(!sCommand.compare(Command_ToRoot)) {
        return handle_to_root(theJson);
    }
    else if(!sCommand.compare(Command_CreateFolder)) {
        return handle_create_folder(theJson);
    }
    else if(!sCommand.compare(Command_CreateFile)) {
        return handle_create_file(theJson);
    }
    theReply[Property_Status] = "Bad";
    theReply[Property_Message] = "Unknown Command";
    return theReply.dump();
}
