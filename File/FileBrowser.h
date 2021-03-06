#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include "../../OtherLib/nlohmann/json.hpp"

class FileBrowser {
private:
    const std::string Command_OpenFolder = "OpenFolder";
    const std::string Command_ReadFileUtf8 = "ReadFile_UTF-8";
    const std::string Command_WriteFileUtf8 = "WriteFile_UTF-8";
    const std::string Command_Delete = "Delete";
    const std::string Command_Rename = "Rename";
    const std::string Command_ToParentFolder = "ToParentFolder";
    const std::string Command_ShowContent = "Show";
    const std::string Command_CurrentPath = "CurrentPath";
    const std::string Command_ToRoot = "ToRoot";
    const std::string Command_NewFolder = "NewFolder";
    const std::string Command_NewFile = "NewFile";
    const std::string Command_CreateFolder = "CreateFolder";
    const std::string Command_CreateFile = "CreateFile";

    const std::string Property_Command = "Command";
    const std::string Property_Name = "Name";
    const std::string Property_Path = "Path";
    const std::string Property_NewName = "NewName";
    const std::string Property_Status = "Status";
    const std::string Property_Message = "Message";
    const std::string Property_FileContent = "Content";
    const std::string Property_FolderContent = "Content";

public:
    FileBrowser(const std::string& _rootFolder);
    ~FileBrowser();

    std::string parse_command(const std::string& command);

private:
    //const std::string rootPath;
    const std::filesystem::path rootPath;
    std::filesystem::path currentPath;

    std::string handle_open_folder(const nlohmann::json& input);
    std::string handle_read_file_utf8(const nlohmann::json& input);
    std::string handle_write_file_utf8(const nlohmann::json& input);
    std::string handle_delete(const nlohmann::json& input);
    std::string handle_rename(const nlohmann::json& input);
    std::string handle_to_parent_folder(const nlohmann::json& input);
    std::string handle_show_content(const nlohmann::json& input);
    std::string handle_show_current_path(const nlohmann::json& input);
    std::string handle_to_root(const nlohmann::json& input);
    std::string handle_create_folder(const nlohmann::json& input);
    std::string handle_create_file(const nlohmann::json& input);
};