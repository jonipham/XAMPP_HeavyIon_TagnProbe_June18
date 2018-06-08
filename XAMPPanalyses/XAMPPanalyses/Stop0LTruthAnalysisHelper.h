#ifndef XAMPPbase_Stop0LTruthAnalysisHelper_H
#define XAMPPbase_Stop0LTruthAnalysisHelper_H

#include <XAMPPbase/SUSYTruthAnalysisHelper.h>

namespace XAMPP {
    class Stop0LTruthAnalysisHelper: public SUSYTruthAnalysisHelper {
        public:

            ASG_TOOL_CLASS(Stop0LTruthAnalysisHelper, XAMPP::IAnalysisHelper)
            Stop0LTruthAnalysisHelper(std::string myname);
            virtual ~Stop0LTruthAnalysisHelper(){}

        protected:
            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();
            virtual StatusCode RemoveOverlap();

            bool m_doReclustering;
            float m_CustomReclusterPt;

    };
}
#endif
