#include <ClientData.hpp>
#include <Exception.hpp>
#include <MPQArchive.hpp>
#include <DirectoryArchive.hpp>
#include <CASCArchive.hpp>

#include <filesystem>
#include <cassert>
#include <regex>

using namespace BlizzardArchive;
namespace fs = std::filesystem;

ClientData::ClientData(std::string const& path, ClientVersion version, Locale locale, std::string const& local_path)
  : _version(version)
  , _open_mode(OpenMode::LOCAL)
  , _storage_type((version > ClientVersion::WOTLK) ? StorageType::CASC : StorageType::MPQ)
  , _locale_mode(locale)
  , _path(path)
  , _local_path(ClientData::normalizeFilenameUnix(local_path))
{

  validateLocale();

  switch (_storage_type)
  {
  case StorageType::MPQ:
    initializeMPQStorage();
    break;
  case StorageType::CASC:
    initializeCASCStorage();
    break;
  }
}

ClientData::ClientData(std::string const& path, std::string const& cdn_cache_path, ClientVersion version, Locale locale, std::string const& local_path)
    : _version(version)
    , _open_mode(OpenMode::REMOTE)
    , _storage_type((version > ClientVersion::WOTLK) ? StorageType::CASC : StorageType::MPQ)
    , _locale_mode(locale)
    , _path(path)
    , _local_path(ClientData::normalizeFilenameUnix(local_path))
    , _cdn_cache_path(cdn_cache_path)
{

  validateLocale();

  switch (_storage_type)
  {
    case StorageType::CASC:
      initializeCASCStorage();
      break;
    case StorageType::MPQ:
      throw Exceptions::Archive::ArchiveOpenError("MPQ storage does not support online loading.");
      break;
  }
}

ClientData::~ClientData()
{
  // clean up
  for (auto archive : _archives)
  {
    delete archive;
  }
}

void ClientData::loadMPQArchive(std::string const& mpq_path)
{
  if (!fs::exists(mpq_path) || fs::equivalent(mpq_path, _local_path))
    return;

  if (fs::is_directory(mpq_path))
  {
    _archives.push_back(new Archive::DirectoryArchive(mpq_path, _locale_mode, &_listfile));
  }
  else
  {
    _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile));
  }


}

void ClientData::initializeMPQStorage()
{
  for (auto const& filename : ClientData::ArchiveNameTemplates)
  {
    std::string mpq_path = (fs::path(_path) / "Data" / filename).string();

    std::string::size_type location(std::string::npos);
    std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode) - 1];

    do
    {
      location = mpq_path.find("{locale}");
      if (location != std::string::npos)
      {
        mpq_path.replace(location, 8, locale);
      }
    } while (location != std::string::npos);

    if (mpq_path.find("{number}") != std::string::npos)
    {
      location = mpq_path.find("{number}");
      mpq_path.replace(location, 8, " ");
      for (char j = '2'; j <= '9'; j++)
      {
        mpq_path.replace(location, 1, std::string(&j, 1));
        loadMPQArchive(mpq_path);
      }
    }
    else if (mpq_path.find("{character}") != std::string::npos)
    {
      location = mpq_path.find("{character}");
      mpq_path.replace(location, 11, " ");
      for (char c = 'a'; c <= 'z'; c++)
      {
        mpq_path.replace(location, 1, std::string(&c, 1));
        loadMPQArchive(mpq_path);
      }
    }
    else
    {
      loadMPQArchive(mpq_path);
    }
  }

 
}

void ClientData::initializeCASCStorage()
{
  _listfile.initFromCSV((fs::path(_local_path) / "listfile.csv").string());

  switch (_open_mode)
  {
    case OpenMode::LOCAL:
    {
      _archives.push_back(new Archive::CASCArchive(_path, "", _locale_mode, _open_mode, &_listfile));
      break;
    }
    case OpenMode::REMOTE:
    {
      assert(_cdn_cache_path.has_value());
      _archives.push_back(new Archive::CASCArchive(_path, _cdn_cache_path.value(), _locale_mode, _open_mode, &_listfile));
      break;
    }
  }

}

void ClientData::validateLocale()
{
  switch (_storage_type)
  {
    case StorageType::MPQ:
    {
      if (static_cast<int>(_locale_mode)) // manual locale
      [[unlikely]]
      {
        fs::path realmlist_path = fs::path(_path) / "Data"
            / ClientData::Locales[static_cast<int>(_locale_mode) - 1] / "realmlist.wtf";

        if (!fs::exists(realmlist_path))
        {
          throw Exceptions::Locale::LocaleNotFoundError("Requested locale \""
            + std::string(ClientData::Locales[static_cast<int>(_locale_mode) - 1].data()) +
            "\" does not exist in the client directory."
            "Be sure, that there is one containing the file \"realmlist.wtf\".");
        }

      }
      else // auto locale
      [[likely]]
      {
        for (unsigned i = 0; i < ClientData::Locales.size(); ++i)
        {
          fs::path realmlist_path = fs::path(_path) / "Data" / ClientData::Locales[i] / "realmlist.wtf";

          if (fs::exists(realmlist_path))
          {
            _locale_mode = static_cast<Locale>(i + 1);
            return;
          }
        }

        throw Exceptions::Locale::LocaleNotFoundError("Automatic locale detection failed. "
                                                      "The client directory does not contain any locale directory. Be "
                                                      "sure, that there is one containing the file \"realmlist.wtf\".");
      }
      break;
    }
    case StorageType::CASC:
    {
      if (_locale_mode == Locale::AUTO)
      {
        throw Exceptions::Locale::IncorrectLocaleModeError("Automatic locale detection is not"
                                                           " supported for CASC-based clients.");
      }
      break;
    }
  }
  
}

bool ClientData::readFile(Listfile::FileKey const& file_key, std::vector<char>& buffer)
{
  const std::lock_guard _lock(_mutex);

  HANDLE handle = nullptr;

  for (auto it = _archives.rbegin(); it != _archives.rend(); ++it)
  {
    if (!(*it)->openFile(file_key, _locale_mode, &handle))
      continue;

    std::uint64_t buf_size = (*it)->getFileSize(handle);
    buffer.resize(buf_size);

    if (!(*it)->readFile(handle, buffer.data(), buf_size))
    {
      assert(false);
    }

    if (!(*it)->closeFile(handle))
    {
      assert(false);
    }

    return true;
  }

  return false;
}

bool ClientData::existsOnDisk(Listfile::FileKey const& file_key)
{
  if (!file_key.hasFilepath())
    return false;

  return fs::exists(getDiskPath(file_key));
}

bool ClientData::exists(Listfile::FileKey const& file_key)
{
  if (ClientData::existsOnDisk(file_key))
  {
    return true;
  }

  const std::lock_guard _lock(_mutex);

  for (auto it = _archives.rbegin(); it != _archives.rend(); ++it)
  {
    if ((*it)->exists(file_key, _locale_mode))
      return true;
  }

  return false;
}

std::string ClientData::getDiskPath(Listfile::FileKey const& file_key)
{
  const std::lock_guard _lock(_mutex);

  if (file_key.hasFilepath())
  {
    return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(file_key.filepath())).string();
  }
  else
  {
    // try deducing filepath from listfile
    assert(file_key.hasFileDataID());
    std::string filepath = _listfile.getPath(file_key.fileDataID());

    if (!filepath.empty())
    {
      return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(filepath)).string();
    }
    else
    {
      return (fs::path(_local_path) / "unknown_files/" / std::to_string(file_key.fileDataID())).string();
    }
  }
   
}

std::string ClientData::normalizeFilenameUnix(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin()
    , [](char c)
    {
      return c == '\\' ? '/' : c;
    }
  );
  return filename;
}

std::string ClientData::normalizeFilenameInternal(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
  std::transform(filename.begin(), filename.end(), filename.begin()
      , [](char c)
                 {
                   return c == '\\' ? '/' : c;
                 }
  );

  if (filename.ends_with(".mdx"))
  {
    filename = std::regex_replace(filename, std::regex(".mdx"), ".m2");
  }
  else if(filename.ends_with(".mdl"))
  {
    filename = std::regex_replace(filename, std::regex(".mdl"), ".m2");
  }

  return filename;
}

std::string ClientData::normalizeFilenameWoW(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
  std::transform(filename.begin(), filename.end(), filename.begin()
    , [](char c)
    {
      return c == '/' ? '\\' : c;
    }
  );
  return filename;
}