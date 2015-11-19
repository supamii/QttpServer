#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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

#endif // CONFIGURATION_H
