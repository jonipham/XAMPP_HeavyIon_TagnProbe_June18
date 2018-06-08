#ifndef XAMPPbase_SUSYParticleSelector_IXX
#define XAMPPbase_SUSYParticleSelector_IXX

#include <XAMPPbase/SUSYParticleSelector.h>
#include <XAMPPbase/ISystematics.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <SUSYTools/SUSYObjDef_xAOD.h>
namespace XAMPP {

    template<typename Container> StatusCode SUSYParticleSelector::LoadPreSelectedContainer(Container* &PreSelCont, const CP::SystematicSet* syst) {
        ATH_CHECK(LoadViewElementsContainer("presel", PreSelCont, !SystematicAffects(*syst)));
        return StatusCode::SUCCESS;
    }
    template<typename Container> StatusCode SUSYParticleSelector::FillFromSUSYTools(Container* &Cont, xAOD::ShallowAuxContainer* &AuxCont, Container* &PreSelCont) {
        ParticleSelector::LinkStatus Status = CreateContainerLinks(ContainerKey(), Cont, AuxCont);
        if (Status == ParticleSelector::LinkStatus::Created) {
            ATH_CHECK(CallSUSYTools());
            ATH_CHECK(ViewElementsContainer("presel", PreSelCont));
            for (const auto& ipart : *Cont)
                if (PassPreSelection(*ipart)) PreSelCont->push_back(ipart);
            PreSelCont->sort(XAMPP::ptsorter);
            return StatusCode::SUCCESS;
        } else if (Status == ParticleSelector::LinkStatus::Loaded) {
            ATH_CHECK(LoadViewElementsContainer("presel", PreSelCont, true));
            return StatusCode::SUCCESS;
        } else if (Status == ParticleSelector::LinkStatus::Failed) {
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    template<typename T> ToolHandle<T> SUSYParticleSelector::GetCPTool(const std::string &name) {
#ifdef XAOD_STANDALONE    
        ToolHandle<T> handle;
        const ToolHandle<T>* HandlePtr = SUSYToolsPtr()->getProperty < ToolHandle < T >> (name);
        if (!HandlePtr) {
            ATH_MSG_ERROR("Failed to retrieve CP tool " << name);
        } else {
            handle = (*HandlePtr);
        }
        return handle;
#else
        return ToolHandle < T > (SUSYToolsPtr()->getProperty(name).toString());
#endif 
    }

}
#endif

