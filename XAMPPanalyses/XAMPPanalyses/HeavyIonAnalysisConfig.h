#ifndef HeavyIonAnalysisConfig_h
#define HeavyIonAnalysisConfig_h
#include <XAMPPbase/AnalysisConfig.h>

namespace XAMPP {
    class HeavyIonAnalysisConfig: public AnalysisConfig {

        public:
            ASG_TOOL_CLASS(HeavyIonAnalysisConfig, XAMPP::IAnalysisConfig)

            HeavyIonAnalysisConfig(std::string Analysis = "HeavyIon");
            virtual ~HeavyIonAnalysisConfig() {
            }

        protected:
            virtual StatusCode initializeCustomCuts();

    };
}
#endif
