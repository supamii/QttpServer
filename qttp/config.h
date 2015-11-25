#ifndef QTTPCONFIGURATION_H
#define QTTPCONFIGURATION_H

#include <QtCore>

namespace qttp
{

class Config
{
  public:
    Config(const QString& path);
    virtual ~Config();

  private:
    QString m_Path;
};

} // End namespace qttp

#endif // QTTPCONFIGURATION_H
