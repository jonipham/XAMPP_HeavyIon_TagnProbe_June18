#ifndef XAMPPanalyses_Stop0LTruthSelector_H
#define XAMPPanalyses_Stop0LTruthSelector_H

#include <XAMPPbase/SUSYTruthSelector.h>

namespace XAMPP {
    class Stop0LTruthSelector: public SUSYTruthSelector {
        public:

            ASG_TOOL_CLASS(Stop0LTruthSelector, XAMPP::ITruthSelector)
            Stop0LTruthSelector(std::string myname);
            virtual ~Stop0LTruthSelector() {}
            virtual StatusCode initialize();

            virtual StatusCode FillTruth(const CP::SystematicSet &systset);

        protected:
            // need to add the b-jet decorators before the overlap removal:
            virtual void InitDecorators(const xAOD::TruthParticle* T, bool Pass = true);
            virtual void InitDecorators(const xAOD::Jet* J, bool Pass);

            bool m_CheckMEPhoton;
            float m_MEphotonPtCut;
            bool m_MEPhotonCache;
    };
}
#endif
