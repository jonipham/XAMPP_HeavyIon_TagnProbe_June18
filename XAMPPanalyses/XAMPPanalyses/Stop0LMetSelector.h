#ifndef Stop0LMetSelector_H
#define Stop0LMetSelector_H

#include <XAMPPbase/SUSYMetSelector.h>

namespace XAMPP {
    class Stop0LMetSelector: public SUSYMetSelector {
        public:


            ASG_TOOL_CLASS(Stop0LMetSelector, XAMPP::IMetSelector)

            Stop0LMetSelector(std::string myname);

            virtual ~Stop0LMetSelector() {};

            virtual StatusCode initialize();

            virtual StatusCode FillMet(const CP::SystematicSet &systset);

        protected:
            xAOD::MissingETContainer* MetElInvisible;
            xAOD::MissingETAuxContainer* MetElInvisibleAux;
            xAOD::MissingETContainer* MetMuInvisible;
            xAOD::MissingETAuxContainer* MetMuInvisibleAux;
            xAOD::MissingETContainer* MetGammaInvisible;
            xAOD::MissingETAuxContainer* MetGammaInvisibleAux;


    };
}

#endif
