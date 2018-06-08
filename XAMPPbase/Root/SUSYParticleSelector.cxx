#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/ISystematics.h>
#include <XAMPPbase/SUSYParticleSelector.h>

namespace XAMPP {
    SUSYParticleSelector::SUSYParticleSelector(std::string myname) :
                ParticleSelector(myname),
                m_susytools("SUSYTools") {
        m_susytools.declarePropertyFor(this, "SUSYTools", "The SUSYTools instance");
    }

    SUSYParticleSelector::~SUSYParticleSelector() {
    }
    StatusCode SUSYParticleSelector::initialize() {
        ATH_CHECK(ParticleSelector::initialize());
        ATH_CHECK(m_susytools.retrieve());
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYParticleSelector::CallSUSYTools() {
        ATH_MSG_WARNING("Calling SUSYParticleSelector::CallSUSYTools()...dummy function...");
        return StatusCode::FAILURE;
    }
    StatusCode SUSYParticleSelector::InitSFdecorator(std::string Suffix, bool Save, const CP::SystematicSet* set, SG::AuxElement::Decorator<double>* &dec, XAMPP::Storage<double>* &storage, std::string BaseName) {
        std::string VarName = ContainerKey().substr(0, 3) + "Weight" + Suffix + (set->name().size() ? "_" : "") + set->name();
        if (!BaseName.empty()) VarName = BaseName + "Weight" + Suffix + (set->name().size() ? "_" : "") + set->name();

        if (!m_XAMPPInfo->DoesVariableExist<double>(VarName)) ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>(VarName, Save, set == m_systematics->GetNominal())); //Weights are saved to the trees only the nominal gets passed to all trees
        storage = m_XAMPPInfo->GetVariableStorage<double>(VarName);
        if (!storage) return StatusCode::FAILURE;
        dec = new SG::AuxElement::Decorator<double>("effSF" + Suffix + set->name());
        return StatusCode::SUCCESS;
    }
    ST::SUSYObjDef_xAOD* SUSYParticleSelector::SUSYToolsPtr() {
        ST::SUSYObjDef_xAOD* ST = dynamic_cast<ST::SUSYObjDef_xAOD*>(m_susytools.operator->());
        return ST;
    }
}
