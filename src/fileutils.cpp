#include "fileutils.h"

using namespace std;
using namespace qttp;

FileUtils::FileUtils() :
  m_Directory(),
  m_Files(),
  m_Dirs(),
  m_MostRecentlyUsed(nullptr),
  m_MRU(),
  m_LastRecentlyUsed(nullptr),
  m_LRU()
{
}

FileUtils::FileUtils(QString directory) :
  m_Directory(directory),
  m_Files(),
  m_Dirs(),
  m_MostRecentlyUsed(nullptr),
  m_MRU(),
  m_LastRecentlyUsed(nullptr),
  m_LRU()
{
}

void FileUtils::populateFiles()
{
  populateFiles(m_Directory);
}

void FileUtils::populateFiles(QDir directory)
{
  if(directory.exists())
  {
    m_Dirs.insert(directory.absolutePath());

    auto files = directory.entryList(QDir::Files);
    for(auto & file : files)
    {
      // TODO: Add any suffixes that make sense.
      QString path = QDir::cleanPath(directory.absoluteFilePath(file));
      m_Files.insert(path, FileInfo(path, 0));
    }
    auto dirs = directory.entryList(QDir::Dirs);
    for(auto & dir : dirs)
    {
      if(dir == "." || dir == "..")
      {
        continue;
      }
      populateFiles(directory.absoluteFilePath(dir));
    }
  }
}

bool FileUtils::hasFile(const QString& file) const
{
  if(m_MostRecentlyUsed != nullptr && m_MostRecentlyUsed->path == file)
  {
    return true;
  }
  if(m_LastRecentlyUsed != nullptr && m_LastRecentlyUsed->path == file)
  {
    return true;
  }
  return m_Files.contains(file);
}

bool FileUtils::hasDir(const QString& dir) const
{
  return m_Dirs.contains(dir);
}

bool FileUtils::loadFile(const QString& file, QByteArray& data) const
{
  if(m_MostRecentlyUsed != nullptr && m_MostRecentlyUsed->path == file)
  {
    m_MostRecentlyUsed->hits++;
    data = m_MRU;
    return true;
  }
  if(m_LastRecentlyUsed != nullptr && m_LastRecentlyUsed->path == file)
  {
    m_LastRecentlyUsed->hits++;
    data = m_LRU;
    return true;
  }
  auto iter = m_Files.find(file);
  if(iter != m_Files.end())
  {
    QFile file;
    file.setFileName(iter.value().path);
    // TODO: FILE NOT OPENED NOR CLOSED AND NOT INCREMENTED.
    data = file.readAll();
    return true;
  }
  return false;
}

QString FileUtils::determineContentType(const QString &urlPath)
{
  QString contentType = "text/html";

  if(urlPath.endsWith(".js", Qt::CaseInsensitive))
  {
    contentType = "text/javascript";
  }
  else if(urlPath.endsWith(".html", Qt::CaseInsensitive) ||
          urlPath.endsWith(".htm", Qt::CaseInsensitive))
  {
    // No-op. Meant to help reduce constantly checking for other types.
  }
  else if(urlPath.endsWith(".json", Qt::CaseInsensitive))
  {
    contentType = "application/javascript";
  }
  else if(urlPath.endsWith(".css", Qt::CaseInsensitive) ||
          urlPath.endsWith(".less", Qt::CaseInsensitive))
  {
    contentType = "text/css";
  }
  else if(urlPath.endsWith(".png", Qt::CaseInsensitive) ||
          urlPath.endsWith(".gif", Qt::CaseInsensitive) ||
          urlPath.endsWith(".jpg", Qt::CaseInsensitive) ||
          urlPath.endsWith(".jpeg", Qt::CaseInsensitive) ||
          urlPath.endsWith(".ico", Qt::CaseInsensitive))
  {
    contentType = "image";
  }
  else if(urlPath.endsWith(".svg", Qt::CaseInsensitive))
  {
    contentType = "image/svg+xml";
  }
  else if(urlPath.endsWith(".xml", Qt::CaseInsensitive))
  {
    contentType = "application/xml";
  }
  else if(urlPath.endsWith(".pdf", Qt::CaseInsensitive))
  {
    // TODO: Support PDF, zip, tar files
  }
  else if(urlPath.endsWith(".zip", Qt::CaseInsensitive))
  {
    // TODO: Support PDF, zip, tar files
  }
  else if(urlPath.endsWith(".tar", Qt::CaseInsensitive))
  {
    // TODO: Support PDF, zip, tar files
  }
  else if(urlPath.endsWith(".gz", Qt::CaseInsensitive))
  {
    // TODO: Support PDF, zip, tar files
  }

  // Again, relying on RVO here.
  return contentType;
}
