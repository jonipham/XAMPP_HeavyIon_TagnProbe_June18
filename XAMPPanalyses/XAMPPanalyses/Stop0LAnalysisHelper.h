#ifndef Stop0LAnalysisHelper_H
#define Stop0LAnalysisHelper_H

#include <XAMPPbase/SUSYAnalysisHelper.h>

#if ROOTCORE_RELEASE_SERIES == 24
#include <RootCore/Packages.h>
#endif

#include <XAMPPanalyses/Stop0LISRSelector.h>

#include <JetResolution/IJERTool.h>

namespace XAMPP {
    class Stop0LAnalysisHelper: public SUSYAnalysisHelper {
        public:

            ASG_TOOL_CLASS(Stop0LAnalysisHelper, XAMPP::IAnalysisHelper)
            Stop0LAnalysisHelper(std::string myname);
            virtual ~Stop0LAnalysisHelper() {}

            virtual StatusCode initialize();

            // overwrite AcceptEvent() in order to add SUSYTools trigger check
            virtual bool AcceptEvent();


        protected:
            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();
            virtual StatusCode initializeObjectTools();
            double GetTotalWeight();
            bool m_doReclustering;
            float m_CustomReclusterPt;
            bool m_doISR;
            bool m_doTrackMet;
            bool m_doWDecays;

            ToolHandle<IJERTool> m_jerTool;
            asg::AnaToolHandle<IISRSelector> m_ISR_selection;
            asg::AnaToolHandle<IISRSelector> m_ISR_selection_truth;



    };
    class Stop0LGammaAnalysisHelper: public Stop0LAnalysisHelper {
        public:

            ASG_TOOL_CLASS(Stop0LGammaAnalysisHelper, XAMPP::IAnalysisHelper)
            Stop0LGammaAnalysisHelper(std::string myname);
            virtual ~Stop0LGammaAnalysisHelper() {}

        protected:
            virtual StatusCode initialize();
            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();
            double GetTotalWeight();
    };
}

#endif
