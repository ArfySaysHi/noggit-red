#pragma once
#include <vector>
#include <structures/FileStructures.h>

namespace BlizzardDatabaseLib {
    namespace Reader {
        struct IBlizzardTableReader
        {
            virtual void LoadTableStructure() = 0;
            virtual void CloseAllSections() = 0;
            virtual Structures::BlizzardDatabaseRow RecordById(unsigned int Id) = 0;
            virtual Structures::BlizzardDatabaseRow Record(unsigned int index) = 0;
            virtual std::vector<Structures::BlizzardDatabaseRowDefiniton> RecordDefinition() = 0;
            virtual std::size_t RecordCount() = 0;
        };

        struct IBlizzardTableWriter
        {
            virtual void WriteRecord(Structures::BlizzardDatabaseRow& record) = 0;
        };
    }
}

