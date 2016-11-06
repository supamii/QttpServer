#ifndef TABLETS_H
#define TABLETS_H

#include <action.h>

class Tablets : public qttp::Action
{
  public:

    Tablets() : Action(), tablets({
    "iPad", "surface", "kindle"
  })
    {
    }

    const char* getName() const
    {
      return "tablet-action-name";
    }

    std::set<qttp::HttpPath> getRoutes() const
    {
      return {
               { qttp::HttpMethod::GET, "tablets" },
               { qttp::HttpMethod::GET, "tablets/:model" },
               { qttp::HttpMethod::PUT, "tablets" },
      };
    }

    QJsonArray allTablets()
    {
      QJsonArray list;
      for(QString tablet : tablets)
        list.push_back(tablet);
      return list;
    }

    void onGet(qttp::HttpData& data)
    {
      if(data.getRequest().getJson().contains("model"))
      {
        QString model = data.getRequest().getJson()["model"].toString();
        if(tablets.contains(model))
        {
          data.getResponse().getJson()[model] = "info";
          return;
        }
        else
        {
          data.getResponse().setStatus(qttp::HttpStatus::BAD_REQUEST);
        }
      }
      data.getResponse().getJson()["list"] = allTablets();
    }

    void onPut(qttp::HttpData& data)
    {
      bool hasDataToStore = data.getRequest().getJson().contains("model");
      if(hasDataToStore)
      {
        tablets.push_back(data.getRequest().getJson()["model"].toString());
      }
      data.getResponse().getJson()["list"] = allTablets();

      data.getResponse().setStatus(hasDataToStore ?
                                   qttp::HttpStatus::OK :
                                   qttp::HttpStatus::BAD_REQUEST);
    }

    QStringList tablets;
};

#endif // TABLETS_H
