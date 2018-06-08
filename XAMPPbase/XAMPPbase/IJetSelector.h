#ifndef XAMPPbase_IJetSelector_H
#define XAMPPbase_IJetSelector_H

#include <AsgTools/IAsgTool.h>
#include <xAODJet/JetContainer.h>
#include <xAODJet/JetAuxContainer.h>

namespace CP {
    class SystematicSet;
}

namespace XAMPP {
    class IJetSelector: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (IJetSelector)

        public:

            virtual StatusCode LoadContainers() =0;
            virtual StatusCode InitialFill(const CP::SystematicSet &systset) =0;
            virtual StatusCode FillJets(const CP::SystematicSet &systset) =0;

            virtual xAOD::JetContainer* GetJets() const =0;
            virtual xAOD::JetContainer* GetPreJets() const =0;
            virtual xAOD::JetContainer* GetBaselineJets() const =0;
            virtual xAOD::JetContainer* GetSignalJets() const =0;
            virtual xAOD::JetContainer* GetPreFatJets(int Radius = 99) const = 0;
            virtual xAOD::JetContainer* GetBJets() const =0;
            virtual xAOD::JetContainer* GetLightJets() const =0;
            virtual xAOD::JetContainer* GetCustomJets(const std::string &kind) =0;

            virtual StatusCode ReclusterJets(const xAOD::IParticleContainer *inputJets, double Rcone, double minPtKt4 = -1., std::string PreFix = "")=0;
            virtual StatusCode SaveScaleFactor()=0;
            virtual ~IJetSelector(){
            }

    };
}
#endif
