#pragma once
#include <iostream>
#include <memory>
#include <readers/IBlizzardTableReader.h>
#include <readers/wdbc/WDBCTableReader.h>
#include <readers/wdc3/WDC3TableReader.h>
#include <stream/StreamReader.h>
#include <string>

namespace BlizzardDatabaseLib {
namespace Reader {
class BlizzardTableReaderFactory {
public:
  std::shared_ptr<IBlizzardTableReader>
  For(std::shared_ptr<Stream::StreamReader> streamReader,
      Structures::VersionDefinition &versionDefinition,
      const std::string &formatSignature);
};
} // namespace Reader
} // namespace BlizzardDatabaseLib
