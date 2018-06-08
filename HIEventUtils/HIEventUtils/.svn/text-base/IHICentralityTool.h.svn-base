// HICentralityTool.h

#ifndef IHICENTRALITYTOOL_H
#define IHICENTRALITYTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"


namespace HI {

  class IHICentralityTool : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE( HI::IHICentralityTool )

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    virtual double getCentralityPercentile() = 0;
    virtual double getCentralityPercentileUp() = 0;
    virtual double getCentralityPercentileDown() = 0;
    virtual double getCentralityEstimator() = 0;

  public:

  };
}
#endif
