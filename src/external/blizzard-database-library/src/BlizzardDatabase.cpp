#include "BlizzardDatabase.h"
#include <cassert>

namespace BlizzardDatabaseLib
{
    BlizzardDatabase::BlizzardDatabase(const std::string& databaseDefinitionDirectory, const Structures::Build& build)
    : _databaseDefinitionFilesLocation(databaseDefinitionDirectory)
    , _build(build)
    {
        _loadedTables = std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>>();
        _blizzardTableReaderFactory = Reader::BlizzardTableReaderFactory();
    }

    const BlizzardDatabaseTable& BlizzardDatabase::LoadTable(const std::string& tableName,
       std::function<std::shared_ptr<BlizzardDatabaseLib::Stream::IMemStream>(std::string const&)> file_callback)
    {
        if (_loadedTables.contains(tableName))
            std::cout << "Table Already Loaded" << std::endl;

        auto absoluteFilePathOfDatabaseTableDefinition =  std::filesystem::path(_databaseDefinitionFilesLocation) /
            (tableName + ".dbd");

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition.generic_string());
        auto tableDefinition = Structures::VersionDefinition();
        auto tableFound = databaseDefinition.For(_build, tableDefinition);

        if (!tableFound)
            std::cout << "Verion Not found" << std::endl;

        auto fileStream = file_callback("DBFilesClient\\" + tableName + ".dbc");

        auto streamReader = std::make_shared<Stream::StreamReader>(fileStream);
        auto fileFormatIdentifier = streamReader->ReadString(4);

        auto tableReader = _blizzardTableReaderFactory.For(streamReader, tableDefinition,fileFormatIdentifier);

        auto constructedTable = std::make_shared<BlizzardDatabaseTable>(tableReader);
        constructedTable->LoadTableStructure();

        _loadedTables.emplace(tableName, constructedTable);

        return *_loadedTables[tableName];
    }

    bool BlizzardDatabase::SaveTable(const std::string& outputDirectory, const std::string& tableName, std::vector<Structures::BlizzardDatabaseRow>& rows)
    {  
        auto absoluteFilePathOfDatabaseTableDefinition = std::filesystem::path(_databaseDefinitionFilesLocation) /
            (tableName + ".dbd");

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition.generic_string());
        auto tableDefinition = Structures::VersionDefinition();
        auto tableFound = databaseDefinition.For(_build, tableDefinition);

        if (!tableFound)
            std::cout << "Verion Not found" << std::endl;

        auto filePath = std::filesystem::path(outputDirectory) / (tableName + ".dbc");
        auto outputStream = std::ofstream(filePath, std::ios::out | std::ios::binary);

        auto fileWriter = Writer::WDBCTableWriter(outputStream,  tableDefinition);
        return fileWriter.Write(rows);
    }

    void BlizzardDatabase::UnloadTable(const std::string& tableName)
    {
        if (!_loadedTables.contains(tableName))
            std::cout << "Table Not Loaded" << std::endl;

        auto table = _loadedTables.at(tableName);

        table.reset();

        _loadedTables.erase(tableName);
    }
}
