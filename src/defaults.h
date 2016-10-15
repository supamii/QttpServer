#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "qttp_global.h"
#include "action.h"

namespace qttp
{

class OptionsPreprocessor : public Processor
{
  public:
    OptionsPreprocessor();
    const char* getName() const;
    void preprocess(HttpData& data);
};

}
#endif // DEFAULTS_H
