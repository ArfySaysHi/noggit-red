#include <MPQArchive.hpp>
#include <Exception.hpp>
#include <StormLib.h>


using namespace BlizzardArchive::Archive;

MPQArchive::MPQArchive(std::string const& path, Locale locale, Listfile::Listfile* listfile)
: BaseArchive(path, locale, listfile)
{
  if (!SFileOpenArchive(path.c_str(), 0, MPQ_OPEN_NO_LISTFILE | STREAM_FLAG_READ_ONLY, &_handle))
  {
    throw Exceptions::Archive::ArchiveOpenError("Error opening archive: " + path);
  }

  // handle listfiles
  HANDLE fh;
  if (SFileOpenFileEx(_handle, "(listfile)", 0, &fh))
  {
    size_t filesize = SFileGetFileSize(fh, nullptr); 

    std::vector<char> readbuffer(filesize);
    SFileReadFile(fh, readbuffer.data(), static_cast<DWORD>(filesize), nullptr, nullptr);
    SFileCloseFile(fh);

    listfile->initFromFileList(readbuffer);
  }
  
}

bool MPQArchive::openFile(Listfile::FileKey const& file_key, Locale locale, HANDLE* file_handle) const
{
  assert(file_key.hasFilepath());
  return SFileOpenFileEx(_handle, ClientData::normalizeFilenameWoW(file_key.filepath()).c_str(), 0, file_handle);
}

bool MPQArchive::readFile(HANDLE file_handle, char* buffer, std::size_t buf_size) const
{
  assert(file_handle);
  return SFileReadFile(file_handle, buffer, static_cast<DWORD>(buf_size), nullptr, nullptr);
}

bool MPQArchive::closeFile(HANDLE file_handle) const
{
  assert(file_handle);
  return SFileCloseFile(file_handle);
}

std::uint64_t MPQArchive::getFileSize(HANDLE file_handle) const
{
  assert(file_handle);
  return SFileGetFileSize(file_handle, nullptr);
}

bool MPQArchive::exists(Listfile::FileKey const& file_key, Locale locale) const
{
  assert(file_key.hasFilepath());
  return SFileHasFile(_handle, ClientData::normalizeFilenameWoW(file_key.filepath()).c_str());
}

MPQArchive::~MPQArchive()
{
  if (_handle)
    SFileCloseArchive(_handle);
}