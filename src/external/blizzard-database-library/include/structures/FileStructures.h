#pragma once
#include <string>
#include <map>
#include <vector>
#include <structures/Build.h>
#include <structures/BuildRange.h>

namespace BlizzardDatabaseLib {
    namespace Structures {

        struct BlizzardDatabaseColumn
        {
            std::string Value;
            std::vector<std::string> Values;
            int ReferenceId;
        };

        struct BlizzardDatabaseRow
        {
            int RecordId = -1;
            std::map<std::string, BlizzardDatabaseColumn> Columns;

            BlizzardDatabaseRow() = default;
            BlizzardDatabaseRow(int recordId) : RecordId(recordId) {}
        };

        struct BlizzardDatabaseRowDefiniton
        {
            std::string Type;
            std::string Name;
            bool isID;
            int arrLength;
            bool isRelation;
        };

        struct ColumnDefinition
        {
            std::string type;
            std::string foreignTable;
            std::string foreignColumn;
            bool hasForeignKey;
            bool verified;
            std::string comment;
        };

        struct Definition
        {
            int size;
            int arrLength;
            std::string name;
            bool isID;
            bool isRelation;
            bool IsInline;
            bool isSigned;
            std::string comment;
        };

        struct VersionDefinitions
        {
            std::vector<Build> builds;
            std::vector<BuildRange> buildRanges;
            std::vector<std::string> layoutHashes;
            std::string comment;
            std::vector<Definition> definitions;

            VersionDefinitions()
            {
                builds = std::vector<Build>();
                buildRanges = std::vector<BuildRange>();
                layoutHashes = std::vector<std::string>();
                comment = std::string();
                definitions = std::vector<Definition>();
            }
        };

        struct DBDefinition
        {
            std::map<std::string, ColumnDefinition> columnDefinitions;
            std::vector<VersionDefinitions> versionDefinitions;

            DBDefinition()
            {
                columnDefinitions = std::map<std::string, ColumnDefinition>();
                versionDefinitions = std::vector<VersionDefinitions>();
            }
        };

        struct VersionDefinition
        {
            std::map<std::string, ColumnDefinition> columnDefinitions;
            VersionDefinitions versionDefinitions;

            VersionDefinition()
            {
                columnDefinitions = std::map<std::string, ColumnDefinition>();
                versionDefinitions = VersionDefinitions();
            }
        };
    }
}