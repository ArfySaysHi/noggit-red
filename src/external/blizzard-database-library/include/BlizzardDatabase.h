#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <BlizzardDatabaseTable.h>
#include <readers/BlizzardTableReaderFactory.h>
#include <DatabaseDefinition.h>
#include <extensions/StringExtensions.h>
#include <stream/StreamReader.h>
#include <sstream>
#include <functional>

namespace BlizzardDatabaseLib
{
	class BlizzardDatabase
	{
		friend class BlizzardDatabaseTable;
	private:
		const std::string _databaseDefinitionFilesLocation;
		const Structures::Build _build;
		Reader::BlizzardTableReaderFactory _blizzardTableReaderFactory;

		std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>> _loadedTables;
	public:
		BlizzardDatabase(const std::string& databaseDefinitionDirectory, const Structures::Build& build);

		const BlizzardDatabaseTable& LoadTable(const std::string& tableName, std::function<std::shared_ptr<BlizzardDatabaseLib::Stream::IMemStream>(std::string const&)> file_callback);
		bool SaveTable(const std::string& outputDirectory, const std::string& tableName, std::vector<Structures::BlizzardDatabaseRow>& rows);
		void UnloadTable(const std::string& tableName);
	};
}