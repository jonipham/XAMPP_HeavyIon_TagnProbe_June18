#ifndef Stop0LAnalysisConfig_h
#define Stop0LAnalysisConfig_h
#include <XAMPPbase/AnalysisConfig.h>

namespace XAMPP {
    class Stop0LAnalysisConfig: public AnalysisConfig {

    public:
        ASG_TOOL_CLASS(Stop0LAnalysisConfig, XAMPP::IAnalysisConfig)
        Stop0LAnalysisConfig(std::string Analysis = "Stop0L");
        virtual ~Stop0LAnalysisConfig() {
        }

    protected:
        virtual StatusCode initializeCustomCuts();
        bool m_doISR;
        bool m_doTrackMet;
        bool m_doReclustering;
    };
    class Stop0LGammaAnalysisConfig: public AnalysisConfig {

    public:
        ASG_TOOL_CLASS(Stop0LGammaAnalysisConfig, XAMPP::IAnalysisConfig)
        Stop0LGammaAnalysisConfig(std::string Analysis = "Stop0L");
        virtual ~Stop0LGammaAnalysisConfig() {
        }
        ;
    protected:
        virtual StatusCode initializeCustomCuts();

    };

    class Stop0LTruthAnalysisConfig: public TruthAnalysisConfig {

    public:
        ASG_TOOL_CLASS(Stop0LTruthAnalysisConfig, XAMPP::IAnalysisConfig)
        Stop0LTruthAnalysisConfig(std::string Analysis = "Stop0LTruth");
        virtual ~Stop0LTruthAnalysisConfig() {
        }
    protected:
        virtual StatusCode initializeCustomCuts();
        bool m_doReclustering;
    };
}
#endif
