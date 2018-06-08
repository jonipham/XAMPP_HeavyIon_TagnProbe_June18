#ifndef DMbbAnalysisConfig_h
#define DMbbAnalysisConfig_h
#include <XAMPPbase/AnalysisConfig.h>

namespace XAMPP {
    class DMbbAnalysisConfig: public AnalysisConfig {

        public:
            ASG_TOOL_CLASS(DMbbAnalysisConfig, XAMPP::IAnalysisConfig)

            DMbbAnalysisConfig(std::string Analysis = "DMbb");
            virtual ~DMbbAnalysisConfig() {
            }

        protected:
            virtual StatusCode initializeCustomCuts();
    };
    class DMbbGammaAnalysisConfig: public AnalysisConfig {

        public:
            ASG_TOOL_CLASS(DMbbGammaAnalysisConfig, XAMPP::IAnalysisConfig)

            DMbbGammaAnalysisConfig(std::string Analysis = "DMbb");
            virtual ~DMbbGammaAnalysisConfig() {
            }

        protected:
            virtual StatusCode initializeCustomCuts();

    };

    class DMbbTruthAnalysisConfig: public TruthAnalysisConfig {

        public:
            ASG_TOOL_CLASS(DMbbTruthAnalysisConfig, XAMPP::IAnalysisConfig)

            DMbbTruthAnalysisConfig(std::string Analysis = "DMbbTruth");
            virtual ~DMbbTruthAnalysisConfig() {
            }

        protected:
            virtual StatusCode initializeCustomCuts();

    };
}
#endif
