#pragma once
#include <vector>
#include <BlizzardDatabaseRecordCollection.h>
#include <BlizzardDatabaseRecord.h>
#include <readers/IBlizzardTableReader.h>
#include <structures/FileStructures.h>
#include <string>
#include <algorithm>
#include <structures/Build.h>

namespace BlizzardDatabaseLib {

    class BlizzardDatabaseTable
    {
        friend class BlizzardDatabase;
    private:
        std::shared_ptr<Reader::IBlizzardTableReader> _tableReader;
    public:
        BlizzardDatabaseTable(std::shared_ptr<Reader::IBlizzardTableReader> tableReader) 
            : _tableReader(tableReader)
        {

        }

        ~BlizzardDatabaseTable()
        {
            _tableReader->CloseAllSections();
        }

        unsigned int RecordCount()
        {
            return _tableReader->RecordCount();
        }

        Structures::BlizzardDatabaseRow Record(unsigned int id)
        {
           return _tableReader->RecordById(id);
        }

        BlizzardDatabaseRecordCollection Records()
        {
            return BlizzardDatabaseRecordCollection(_tableReader);
        }

        std::vector<Structures::BlizzardDatabaseRowDefiniton> GetRecordDefinition()
        {
            return _tableReader->RecordDefinition();
        }

        void WriterRecord(Structures::BlizzardDatabaseRow& newRecord)
        {


            //Reload data so it contains new record
            _tableReader->CloseAllSections();
            _tableReader->LoadTableStructure();
        }
    private:
        void LoadTableStructure()
        {  
            _tableReader->LoadTableStructure();
        }
    };
}

