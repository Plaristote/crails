#include <Utils/directory.hpp>

using namespace std;

bool Filesystem::FileContent(const string& filepath, string& out)
{
  ifstream file(filepath.c_str());

  if (file.is_open())
  {
    long           size, begin, end;
    char*          raw;

    begin       = file.tellg();
    file.seekg(0, std::ios::end);
    end         = file.tellg();
    file.seekg(0, std::ios::beg);
    size        = end - begin;
    raw         = new char[size + 1];
    file.read(raw, size);
    file.close();
    out = raw;
    delete[] raw;
    return (true);
  }
  return (false);
}

bool Filesystem::FileCopy(const string& from, const string& dest)
{
  ifstream stream_src(from.c_str(),  std::ios::binary);
  ofstream stream_dest(dest.c_str(), std::ios::binary);

  if (stream_src.is_open() && stream_dest.is_open())
  {
    long           begin, end;
    long           size;
    char*          raw;

    begin       = stream_src.tellg();
    stream_src.seekg(0, std::ios::end);
    end         = stream_src.tellg();
    stream_src.seekg(0, std::ios::beg);
    size        = end - begin;
    raw         = new char[size + 1];
    stream_src.read(raw, size);
    stream_src.close();
    raw[size]   = 0;
    stream_dest << raw;
    delete[] raw;
    return (true);
  }
  return (false);
}

bool Filesystem::WriteToFile(const string& file, const string& to_write)
{
  ofstream stream_dest(file.c_str());
  
  if (stream_dest.is_open())
  {
    stream_dest.write(to_write.c_str(), to_write.size());
    return (true);
  }
  return (false);
}

#ifndef _WIN32 // UNIX

bool Filesystem::FileExists(const string& file)
{
  return (!(::access(file.c_str(), F_OK)));
}

bool Filesystem::FileMove(const string& from, const string& to)
{
  return (!(::rename(from.c_str(), to.c_str())));
}

bool Filesystem::FileRemove(const string& file)
{
  return (!(::unlink(file.c_str())));
}

std::string Directory::Current(void)
{
  char buf[256];

  return (getcwd(buf, 256));
}

bool Directory::MakeDir(const string& str)
{
  return (mkdir(str.c_str() , S_IRWXU | S_IRWXG | S_IRWXO) == 0);
}

bool Directory::RemoveDir(const string& str)
{
  return (rmdir(str.c_str()) == 0);
}

bool Directory::OpenDir(const string& str)
{
  DIR*      dir;
  Dirent*   ent;

  dir = opendir(str.c_str());
  if (dir != NULL)
  {
    _dirEntries.clear();
    _dirName    = str;
    /* print all the files and directories within directory */
    while ((ent = readdir(dir)) != NULL)
      _dirEntries.push_back(*ent);
    closedir (dir);
  }
  return (dir != 0);
}

#else // WINDOWS
# include <io.h>

bool Filesystem::FileExists(const string& file)
{
  return (!(_access(file.c_str(), 0)));
}

bool Filesystem::FileMove(const string& from, const string& to)
{
  return (MoveFile(from.c_str(), to.c_str()));
}

bool Filesystem::FileRemove(const string& file)
{
  return (DeleteFile(file.c_str()));
}

std::string Directory::Current(void)
{
  char buf[256];
  
  return (GetCurrentDirectory(buf, 256));
}

bool Directory::MakeDir(const string& str)
{
  return (CreateDirectory(str.c_str(), NULL));
}

bool Directory::RemoveDir(const string& str)
{
  return (_rmdir(str.c_str()) == 0);
}

bool Directory::OpenDir(const string& str)
{
  WIN32_FIND_DATA ffd;
  HANDLE          dir = FindFirstFile(str.c_str(), &ffd);

  if (dir != INVALID_HANDLE_VALUE)
  {
    _dirEntries.clear();
    _dirName = str;
    do
    {
      Dirent dirent;

      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      { dirent.d_type = DT_DIR; }
      else
      { dirent.d_type = DT_REG; }
      dirent.d_name = ffd.cFileName;
      _dirEntries.push_back(dirent);
    } while (FindNextFile(dir, &ffd) != 0);
    FindClose(dir);
  }
  return (dir != INVALID_HANDLE_VALUE);
}
#endif
