#ifndef DESKTOPS_H
#define DESKTOPS_H

#include <action.h>

class Desktops : public qttp::Action
{
  public:

    QStringList desktops;

    Desktops() : Action(), desktops({
    "iMac", "inspiron", "z800"
  })
    {
    }

    const char* getName() const
    {
      return "desktop-action";
    }

    void onGet(qttp::HttpData& data)
    {
      static const QHash<QString, QString> info = {{"iMac", "apple"}, {"inspiron", "dell"}, {"z800", "hp"}};
      QJsonValue type = data.getRequest().getJson()["model"];
      if(type.isString() && info.contains(type.toString()))
      {
        data.getResponse().getJson()["maker"] = info[type.toString()];
        return;
      }
      qttp::HttpResponse& response = data.getResponse();
      QJsonObject& json = response.getJson();
      json["list"] = QJsonArray::fromStringList(desktops);
    }

    void onPut(qttp::HttpData& data)
    {
      bool isSuccessful = false;
      const QJsonObject& request = data.getRequest().getJson();
      QJsonValue type = request["model"];
      if(type.isString())
      {
        desktops.append(type.toString());
        isSuccessful = true;
      }

      QJsonObject& response = data.getResponse().getJson();
      response["list"] = QJsonArray::fromStringList(desktops);
      response["success"] = isSuccessful;

      data.getResponse().setStatus(isSuccessful ?
                                   qttp::HttpStatus::OK :
                                   qttp::HttpStatus::BAD_REQUEST);
    }
};

#endif // DESKTOPS_H
