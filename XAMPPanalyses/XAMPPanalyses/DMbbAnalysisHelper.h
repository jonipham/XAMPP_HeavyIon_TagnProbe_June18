#ifndef DMbbAnalysisHelper_H
#define DMbbAnalysisHelper_H

#include <XAMPPbase/SUSYAnalysisHelper.h>

#include <JetResolution/IJERTool.h>

namespace XAMPP {
    class DMbbAnalysisHelper: public SUSYAnalysisHelper {
        public:
            ASG_TOOL_CLASS(DMbbAnalysisHelper, XAMPP::IAnalysisHelper)
            DMbbAnalysisHelper(std::string myname);
            virtual ~DMbbAnalysisHelper() {
            }

            virtual StatusCode initialize();

            // overwrite AcceptEvent() in order to add ttZqqVeto
            virtual bool AcceptEvent();

        protected:
            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();
            virtual StatusCode initializeObjectTools();
            double GetTotalWeight();
            bool m_doTrackMet;

            ToolHandle<IJERTool> m_jerTool;

    };
    class DMbbGammaAnalysisHelper: public DMbbAnalysisHelper {
        public:
            ASG_TOOL_CLASS(DMbbGammaAnalysisHelper, XAMPP::IAnalysisHelper)

            DMbbGammaAnalysisHelper(std::string myname);
            virtual ~DMbbGammaAnalysisHelper() {
            }

        protected:
            virtual StatusCode initialize();
            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();
            double GetTotalWeight();
    };
}

#endif
