#ifndef PHONES_H
#define PHONES_H

#include <action.h>

class Phones : public qttp::Action
{
  public:

    Phones() : Action(), phones({
    { "iphone", "apple" }, { "pixel", "google" }, { "nexus", "google" }, { "galaxy", "samsung" }
  })
    {
    }

    const char* getName() const
    {
      return "phone-action";
    }

    QJsonArray allPhoneModels()
    {
      QJsonArray models;
      for(QString phone : phones.keys())
        models.append(phone);
      return models;
    }

    void onGet(qttp::HttpData& data)
    {
      const QJsonObject& request = data.getRequest().getJson();
      if(request.contains("model"))
      {
        bool containsModel = phones.contains(request["model"].toString());
        if(containsModel)
        {
          QString model = request["model"].toString();
          QString make = phones[model];
          data.getResponse().getJson()[model] = make;
        }
        data.getResponse().setStatus(containsModel ?
                                     qttp::HttpStatus::OK :
                                     qttp::HttpStatus::BAD_REQUEST);
        return;
      }

      // If a model was not specified return all models.
      data.getResponse().getJson()["list"] = allPhoneModels();
    }

    void onPut(qttp::HttpData& data)
    {
      bool isSuccessful = false;

      QJsonValue model = data.getRequest().getJson()["model"];
      QJsonValue make = data.getRequest().getJson()["make"];

      // The request should contain a model and a make to insert.
      if(model.isString() && make.isString())
      {
        phones.insert(model.toString(), make.toString());
        isSuccessful = true;
      }

      // List all models after updating the list of phones;
      data.getResponse().getJson()["list"] = allPhoneModels();
      data.getResponse().getJson()["success"] = isSuccessful;

      data.getResponse().setStatus(isSuccessful ?
                                   qttp::HttpStatus::OK :
                                   qttp::HttpStatus::BAD_REQUEST);
    }

    QHash<QString, QString> phones;
};

#endif // PHONES_H
