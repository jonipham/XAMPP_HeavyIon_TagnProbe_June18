#ifndef XAMPPbase_SUSYParticleSelector_H
#define XAMPPbase_SUSYParticleSelector_H

#include <SUSYTools/ISUSYObjDef_xAODTool.h>
#include <XAMPPbase/ParticleSelector.h>
#include <XAMPPbase/ISystematics.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/EventStorage.h>
#include <XAMPPbase/AnalysisUtils.h>

namespace ST {
    class SUSYObjDef_xAOD;
}
namespace XAMPP {
    class SUSYParticleSelector: public ParticleSelector {

        public:
            SUSYParticleSelector(std::string myname);
	    virtual ~SUSYParticleSelector();
            virtual StatusCode initialize();
        protected:
            asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_susytools;
            virtual StatusCode CallSUSYTools();
            StatusCode InitSFdecorator(std::string Suffix, bool Save, const CP::SystematicSet* set, SG::AuxElement::Decorator<double>* &dec, XAMPP::Storage<double>* &storage, std::string BaseName = "");
            ST::SUSYObjDef_xAOD* SUSYToolsPtr();
            template<typename Container> StatusCode LoadPreSelectedContainer(Container* &PreSelCont, const CP::SystematicSet* syst);
            template<typename Container> StatusCode FillFromSUSYTools(Container* &Cont, xAOD::ShallowAuxContainer* &AuxCont, Container* &PreSelCont);
            template<typename T> ToolHandle<T> GetCPTool(const std::string &name);

    };
}
#include <XAMPPbase/SUSYParticleSelector.ixx>
#endif

