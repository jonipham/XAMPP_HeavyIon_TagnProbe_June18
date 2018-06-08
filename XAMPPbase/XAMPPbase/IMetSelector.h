// IMetSelection.h

#ifndef XAMPPbase_IMetSelection_H
#define XAMPPbase_IMetSelection_H

#include <AsgTools/IAsgTool.h>

#include <xAODMissingET/MissingETContainer.h>
#include <xAODMissingET/MissingETAuxContainer.h>

namespace CP {
    class SystematicSet;
}

namespace XAMPP {
    class IMetSelector: virtual public asg::IAsgTool {
        public:
            ASG_TOOL_INTERFACE (IMetSelector)

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode FillMet(const CP::SystematicSet &systset) =0;

            virtual xAOD::MissingETContainer* GetCustomMet(std::string kind) =0;
            virtual ~IMetSelector(){}
    };
}
#endif
