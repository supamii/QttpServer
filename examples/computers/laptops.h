#ifndef LAPTOPS_H
#define LAPTOPS_H

#include <action.h>

class Laptops : public qttp::Action
{
  public:

    Laptops() : Action(), laptops({
    "macbook", "thinkpad", "chromebook"
  })
    {
    }

    const char* getName() const
    {
      return "laptops-action-name";
    }

    const char* getDescription() const
    {
      return "End-point for laptops";
    }

    const char* getSummary() const
    {
      return "Laptops!";
    }

    QStringList getTags() const
    {
      return { "computers", "laptops" };
    }

    std::vector<qttp::Input> getInputs() const
    {
      return {
               qttp::Input("model", {{ qttp::HttpMethod::GET, "laptops/:model" },
                                     { qttp::HttpMethod::GET, "laptops/:model/:color" }}),
               qttp::Input("color", {{ qttp::HttpMethod::GET, "laptops/:model/:color" }}),
               qttp::Input("alllaptops", {{ qttp::HttpMethod::ALL, "laptops" }})
      };
    }

    std::set<qttp::HttpPath> getRoutes() const
    {
      return {
               { qttp::HttpMethod::GET, "laptops" },
               { qttp::HttpMethod::GET, "laptops/:model" },
               { qttp::HttpMethod::GET, "laptops/:model/:color" },
               { qttp::HttpMethod::PUT, "laptops" },
      };
    }

    QJsonArray allLaptops()
    {
      QJsonArray list;
      for(QString laptop : laptops)
        list.push_back(laptop);
      return list;
    }

    void onGet(qttp::HttpData& data)
    {
      if(data.getRequest().getJson().contains("model"))
      {
        QString model = data.getRequest().getJson()["model"].toString();
        if(laptops.contains(model))
        {
          if(data.getRequest().getJson().contains("color"))
          {
            data.getResponse().getJson()["color"] = "always beige!";
            return;
          }

          data.getResponse().getJson()[model] = "info";
          return;
        }
        else
        {
          data.getResponse().setStatus(qttp::HttpStatus::BAD_REQUEST);
        }
      }
      data.getResponse().getJson()["list"] = allLaptops();
    }

    void onPut(qttp::HttpData& data)
    {
      bool hasDataToStore = data.getRequest().getJson().contains("model");
      if(hasDataToStore)
      {
        laptops.push_back(data.getRequest().getJson()["model"].toString());
      }
      data.getResponse().getJson()["list"] = allLaptops();

      data.getResponse().setStatus(hasDataToStore ?
                                   qttp::HttpStatus::OK :
                                   qttp::HttpStatus::BAD_REQUEST);
    }

    QStringList laptops;
};
#endif // LAPTOPS_H
