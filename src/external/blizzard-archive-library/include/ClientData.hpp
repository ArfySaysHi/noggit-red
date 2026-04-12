#ifndef BLIZZARD_ARCHIVE_CLIENT_DATA_HPP
#define BLIZZARD_ARCHIVE_CLIENT_DATA_HPP

#include <array>
#include <vector>
#include <string>
#include <optional>
#include <mutex>
#include <string_view>

#include <Listfile.hpp>

typedef void* HANDLE;

namespace BlizzardArchive
{

  namespace Archive
  {
    class BaseArchive;
  }


  enum class ClientVersion : char
  {
    WOTLK = 0,
    SL = 1,
  };

  enum class StorageType : char
  {
    MPQ = 0,
    CASC = 1
  };

  enum class OpenMode
  {
    LOCAL,
    REMOTE
  };

  enum class Locale
  {
    AUTO, 
    enGB,
    enUS,
    deDE,
    koKR,
    frFR,
    zhCN,
    zhTW,
    esES,
    esMX,
    ruRU
  };

  class ClientData
  {
  public:
    /*
    * path - path to game directory for MPQ-based clients, path to storage directory (the one containing .build.info) for CASC-based clients.
    * CDN URL for online CASC Storages.
    * version - version of the game client. Currently only WotLK and Shadowlands are supported.
    * locale - prefered locale of the client. Wotlk supports automatic detection, for that use Locale::AUTO
    * local_path - project directory, should also contain listfile.csv for CASC-based projects.
    */
    explicit ClientData(std::string const& path
      , ClientVersion version
      , Locale locale
      , std::string const& local_path);

    explicit ClientData(std::string const& path
        , std::string const& cdn_cache_path
        , ClientVersion version
        , Locale locale
        , std::string const& local_path);

    ~ClientData();

    [[nodiscard]]
    ClientVersion version() const { return _version; }

    [[nodiscard]]
    StorageType storageType() const { return _storage_type; }

    [[nodiscard]]
    OpenMode openMode() const { return _open_mode; }

    [[nodiscard]]
    std::string const& path() const { return _path; }

    [[nodiscard]]
    std::string getDiskPath(Listfile::FileKey const& file_key);

    const Listfile::Listfile* listfile() const { return &_listfile; }

    /* Methods used to universally request client file data in an archive type agnostic way. */

    [[nodiscard]]
    bool readFile(Listfile::FileKey const& file_key, std::vector<char>& buffer);

    [[nodiscard]]
    bool exists(Listfile::FileKey const& file_key);

    [[nodiscard]]
    bool existsOnDisk(Listfile::FileKey const& file_key);

    /* Static helper methods */
    [[nodiscard]]
    static std::string normalizeFilenameUnix(std::string filename);

    [[nodiscard]]
    static std::string normalizeFilenameInternal(std::string filename);

    [[nodiscard]]
    static std::string normalizeFilenameWoW(std::string filename);


  public:
    inline static constexpr std::array<std::string_view, 10> Locales { "enGB", "enUS", "deDE", "koKR", "frFR", "zhCN", "zhTW", "esES", "esMX", "ruRU" };

    // Templates in correct order for opening the wotlk client MPQs
    inline static constexpr std::array<std::string_view, 17> ArchiveNameTemplates { 
                                                                                    // common archives
                                                                                      "common.MPQ"
                                                                                    , "common-2.MPQ"
                                                                                    , "expansion.MPQ"
                                                                                    , "lichking.MPQ"
                                                                                    , "patch.MPQ"
                                                                                    , "patch-{number}.MPQ"
                                                                                    , "patch-{character}.MPQ"

                                                                                    // locale-specific archives
                                                                                    , "{locale}/locale-{locale}.MPQ"
                                                                                    , "{locale}/expansion-locale-{locale}.MPQ"
                                                                                    , "{locale}/lichking-locale-{locale}.MPQ"
                                                                                    , "{locale}/patch-{locale}.MPQ"
                                                                                    , "{locale}/patch-{locale}-{number}.MPQ" 
                                                                                    , "{locale}/patch-{locale}-{character}.MPQ"
                                                                                    , "{locale}/speech-{locale}.MPQ"
                                                                                    , "{locale}/expansion-speech-{locale}.MPQ"
                                                                                    , "{locale}/lichking-speech-{locale}.MPQ"
                                                                                    , "development.MPQ"
                                                                                  };

  private:

    void initializeMPQStorage();
    void loadMPQArchive(std::string const& mpq_path);
    void initializeCASCStorage();
    void validateLocale();

    OpenMode _open_mode;
    StorageType _storage_type;
    ClientVersion _version;
    Locale _locale_mode;
    std::string _path;
    std::string _local_path;
    std::optional<std::string> _cdn_cache_path;

    // A sorted list of loaded archives. The last one is the most up-to-date one.
    std::vector<Archive::BaseArchive*> _archives;
    Listfile::Listfile _listfile;

    // sync
    std::mutex _mutex;


  };
}

#endif // BLIZZARD_ARCHIVE_CLIENT_DATA_HPP