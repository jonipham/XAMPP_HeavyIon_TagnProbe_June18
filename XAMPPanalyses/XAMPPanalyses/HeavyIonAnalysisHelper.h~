#ifndef XAMPPanalyses_HeavyIonAnalysisHelper_H
#define XAMPPanalyses_HeavyIonAnalysisHelper_H

#include <XAMPPbase/SUSYAnalysisHelper.h>
#include <HIEventUtils/IHICentralityTool.h>
#include <HIEventUtils/HICentralityTool.h>

namespace XAMPP {
    class HeavyIonAnalysisHelper: public SUSYAnalysisHelper {
        public:


            ASG_TOOL_CLASS(HeavyIonAnalysisHelper, XAMPP::IAnalysisHelper)
            HeavyIonAnalysisHelper(std::string myname);
            virtual ~HeavyIonAnalysisHelper() {}

            virtual StatusCode initialize();

            // overwrite AcceptEvent() in order to add ttZqqVeto
            virtual bool AcceptEvent();


        protected:
            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();
            virtual StatusCode initializeObjectTools();
            double GetTotalWeight();
            
            //ToolHandle<IHICentralityTool> m_HICentralityTool;


    };
  
}

#endif
