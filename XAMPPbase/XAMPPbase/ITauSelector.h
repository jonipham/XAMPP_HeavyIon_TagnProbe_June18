#ifndef XAMPPbase_ITauSelector_H
#define XAMPPbase_ITauSelector_H

#include <AsgTools/IAsgTool.h>

#include <xAODTau/TauJet.h>
#include <xAODTau/TauJetContainer.h>
#include <xAODTau/DiTauJet.h>
#include <xAODTau/DiTauJetContainer.h>

namespace CP {
    class SystematicSet;
}

namespace XAMPP {
    typedef ElementLink<xAOD::TauJetContainer> TauLink;

    class ITauSelector: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (ITauSelector)

        public:

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode InitialFill(const CP::SystematicSet &systset) =0;
            virtual StatusCode FillTaus(const CP::SystematicSet &systset) =0;

            virtual TauLink GetLink(const xAOD::TauJet &tau) const=0;
            virtual TauLink GetOrigLink(const xAOD::TauJet &tau) const=0;

            virtual const xAOD::TauJetContainer* GetTauContainer() const=0;

            virtual xAOD::TauJetContainer* GetTaus() const =0;
            virtual xAOD::TauJetContainer* GetPreTaus() const =0;
            virtual xAOD::TauJetContainer* GetSignalTaus() const =0;
            virtual xAOD::TauJetContainer* GetBaselineTaus() const =0;
            virtual xAOD::TauJetContainer* GetCustomTaus(const std::string &kind) const =0;

            virtual const xAOD::DiTauJetContainer* GetDiTauContainer() const=0;

            virtual xAOD::DiTauJetContainer* GetDiTaus() const =0;
            virtual xAOD::DiTauJetContainer* GetPreDiTaus() const =0;
            virtual xAOD::DiTauJetContainer* GetSignalDiTaus() const =0;
            virtual xAOD::DiTauJetContainer* GetBaselineDiTaus() const =0;
            virtual xAOD::DiTauJetContainer* GetCustomDiTaus(const std::string &kind) const =0;

            virtual StatusCode SaveScaleFactor()=0;
            virtual ~ITauSelector(){}
    };
}
#endif
