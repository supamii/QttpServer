#ifndef QTTP_GLOBAL_H
#define QTTP_GLOBAL_H

#include <QtCore>
#include <QtNetwork>
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QEvent>
#include <QDateTime>
#include <QProcessEnvironment>
#include <QJsonObject>

#include <http.h>
#include <functional>

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <unordered_map>
#include <thread>
#include <initializer_list>

#if defined(QTTP_LIBRARY)
#  define QTTPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTTPSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace qttp
{

/// Quick a dirty mechanism to assert ptrs within the initializer list!
template<class T> class Assert
{
  public:
    Assert(T* ptr)
    {
      Q_ASSERT(ptr != nullptr);
    }
};
}
#endif // QTTP_GLOBAL_H
