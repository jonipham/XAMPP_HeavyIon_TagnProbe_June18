#ifndef XAMPPbase_IPhotonSelector_H
#define XAMPPbase_IPhotonSelector_H

#include <AsgTools/IAsgTool.h>

#include <xAODEgamma/Photon.h>
#include <xAODEgamma/PhotonContainer.h>

namespace CP {
    class SystematicSet;
}

namespace XAMPP {

    typedef ElementLink<xAOD::PhotonContainer> PhoLink;
    class IPhotonSelector: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (IPhotonSelector)

        public:

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode InitialFill(const CP::SystematicSet &systset) =0;
            virtual StatusCode FillPhotons(const CP::SystematicSet &systset) =0;

            virtual PhoLink GetLink(const xAOD::Photon &ph) const=0;
            virtual PhoLink GetOrigLink(const xAOD::Photon &ph) const=0;
            virtual const xAOD::PhotonContainer* GetPhotonContainer() const=0;

            virtual xAOD::PhotonContainer* GetPhotons() const =0;

            virtual xAOD::PhotonContainer* GetPrePhotons() const =0;
            virtual xAOD::PhotonContainer* GetSignalPhotons() const =0;
            virtual xAOD::PhotonContainer* GetBaselinePhotons() const =0;
            virtual xAOD::PhotonContainer* GetCustomPhotons(std::string kind) =0;
            virtual StatusCode SaveScaleFactor()=0;
            virtual ~IPhotonSelector(){}

    };
}
#endif
