// IMuonSelector.h

#ifndef XAMPPBASE_IMuonSelector_H
#define XAMPPBASE_IMuonSelector_H

#include <AsgTools/IAsgTool.h>

#include <xAODMuon/Muon.h>
#include <xAODMuon/MuonContainer.h>

namespace CP {
    class SystematicSet;
}

namespace XAMPP {
    typedef ElementLink<xAOD::MuonContainer> MuoLink;

    class IMuonSelector: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (IMuonSelector)

        public:

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode InitialFill(const CP::SystematicSet &systset) =0;
            virtual StatusCode FillMuons(const CP::SystematicSet &systset) =0;
            virtual StatusCode LoadSelection(const CP::SystematicSet &systset)=0;

            virtual MuoLink GetLink(const xAOD::Muon &mu) const=0;
            virtual MuoLink GetOrigLink(const xAOD::Muon &mu) const=0;
            virtual const xAOD::MuonContainer* GetMuonContainer() const=0;

            virtual xAOD::MuonContainer* GetMuons() const =0;
            virtual xAOD::MuonContainer* GetPreMuons() const =0;
            virtual xAOD::MuonContainer* GetSignalMuons() const =0;
            virtual xAOD::MuonContainer* GetBaselineMuons() const =0;
            virtual xAOD::MuonContainer* GetCustomMuons(std::string kind) =0;

            virtual StatusCode SaveScaleFactor()=0;
            virtual ~IMuonSelector(){
            }

    };
}
#endif
