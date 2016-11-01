#ifndef DESKTOPS_H
#define DESKTOPS_H

#include <action.h>

class Desktops : public qttp::Action
{
  public:

    Desktops() : Action(), desktops({
    "iMac", "inspiron", "z800"
  })
    {
    }

    const char* getName() const
    {
      return "desktop-action";
    }

    QJsonArray allDesktops()
    {
      QJsonArray list;
      for(QString desktop : desktops)
        list.append(desktop);
      return list;
    }

    void onGet(qttp::HttpData& data)
    {
      data.getResponse().getJson()["list"] = allDesktops();
    }

    void onPut(qttp::HttpData& data)
    {
      bool isSuccessful = false;
      QJsonValue type = data.getRequest().getJson()["model"];
      if(type.isString())
      {
        desktops.append(type.toString());
        isSuccessful = true;
      }
      data.getResponse().getJson()["list"] = allDesktops();
      data.getResponse().getJson()["success"] = isSuccessful;
    }

    QVector<QString> desktops;
};

#endif // DESKTOPS_H
