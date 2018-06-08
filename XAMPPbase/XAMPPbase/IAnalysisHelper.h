// IAnalysisHelper.h
#ifndef IAnalysisHelper_H
#define IAnalysisHelper_H

#include "AsgTools/IAsgTool.h"
#include <XAMPPbase/Defs.h>
namespace CP {
    class SystematicSet;
}

namespace XAMPP {
    class IAnalysisHelper: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (IAnalysisHelper)

        public:
            virtual bool AcceptEvent()=0;

            virtual bool EventCleaning()const =0;
            virtual bool CleanObjects(const CP::SystematicSet* systset)=0;

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode FillInitialObjects(const CP::SystematicSet* systset) =0;
            virtual StatusCode RemoveOverlap() =0;
            virtual StatusCode FillObjects(const CP::SystematicSet* systset) =0;
            virtual StatusCode CheckCutFlow(const CP::SystematicSet* systset)=0;
            virtual StatusCode DumpCxAOD()=0;
            virtual StatusCode finalize()=0;

            virtual bool CheckTrigger() =0;

            virtual StatusCode FillEvent(const CP::SystematicSet * sys) =0;

            // call GetMCXsec before GetMCFilterEff/GetMCkFactor/GetMCXsectTimesEff since it computes the SUSY::finalState
            virtual unsigned int finalState()=0;
            virtual double GetMCXsec(unsigned int mc_channel_number, unsigned int finalState = 0) =0;
            virtual double GetMCFilterEff(unsigned int mc_channel_number, unsigned int finalState = 0) =0;
            virtual double GetMCkFactor(unsigned int mc_channel_number, unsigned int finalState = 0) =0;
            virtual double GetMCXsectTimesEff(unsigned int mc_channel_number, unsigned int finalState = 0) =0;
            
            virtual ~IAnalysisHelper(){}
            
    };
}
#endif
