#ifndef QTTPUTILS_H
#define QTTPUTILS_H

#include <QtCore>
#include <QFile>
#include <QJsonDocument>

namespace qttp
{

class Utils
{
  public:
    Utils();
    virtual ~Utils();
    static QJsonObject readJson(const QString& path);
};

} // End namespace qttp

#endif // QTTPUTILS_H
