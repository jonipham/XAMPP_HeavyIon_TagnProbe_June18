#ifndef XAMPPbase_IElectronSelector_H
#define XAMPPbase_IElectronSelector_H

#include <AsgTools/IAsgTool.h>

#include <xAODEgamma/Electron.h>
#include <xAODEgamma/ElectronContainer.h>

namespace CP {
    class SystematicSet;
}
namespace XAMPP {
    typedef ElementLink<xAOD::ElectronContainer> EleLink;

    class IElectronSelector: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (IElectronSelector)

        public:

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode InitialFill(const CP::SystematicSet &systset) =0;
            virtual StatusCode FillElectrons(const CP::SystematicSet &systset) =0;

            virtual StatusCode LoadSelection(const CP::SystematicSet &systset) =0;

            virtual EleLink GetLink(const xAOD::Electron &el) const=0;
            virtual EleLink GetOrigLink(const xAOD::Electron &el) const=0;

            virtual const xAOD::ElectronContainer* GetElectronContainer() const=0;
            virtual xAOD::ElectronContainer* GetElectrons() const =0;

            virtual xAOD::ElectronContainer* GetPreElectrons() const =0;
            virtual xAOD::ElectronContainer* GetSignalElectrons() const =0;
            virtual xAOD::ElectronContainer* GetBaselineElectrons() const =0;

            virtual xAOD::ElectronContainer* GetCustomElectrons(std::string kind) =0;
            virtual StatusCode SaveScaleFactor()=0;
            virtual ~IElectronSelector(){
            }

    };
}
#endif
