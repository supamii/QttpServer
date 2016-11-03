#ifndef FILELOOKUP_H
#define FILELOOKUP_H

#include "qttp_global.h"

namespace qttp
{

/**
 * @brief Introduced to prevent issues serving directly from the file system.
 */
class QTTPSHARED_EXPORT FileUtils
{
  public:

    FileUtils();
    FileUtils(QString directory);

    /**
     * @brief Search for all files and build a look-up list.
     */
    void populateFiles();
    void populateFiles(QDir dir);

QTTP_PRIVATE:
    void populateFilesInternal(QDir dir);

  public:

    bool hasFile(const QString& file) const;
    bool hasDir(const QString& dir) const;

    /**
     * @brief This will not only retrieve the file associated with the path,
     * but it should also help cache items in memory - LRU/MRU.
     */
    bool loadFile(const QString& file, QByteArray& data) const;

    static QString determineContentType(const QString& urlPath);

QTTP_PRIVATE:

    class FileInfo
    {
      public:
        FileInfo()
        {
        }
        // TODO: Move constructor?
        FileInfo(const QString& p, quint32 h) :
          path(p), hits(h)
        {
        }
        QString path;
        quint32 hits;
    };

    QDir m_Directory;
    QHash<QString, FileInfo> m_Files;
    QSet<QString> m_Dirs;
    FileInfo* m_MostRecentlyUsed;
    QByteArray m_MRU;
    FileInfo* m_LastRecentlyUsed;
    QByteArray m_LRU;

};

}

#endif // FILELOOKUP_H
