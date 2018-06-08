#include <XAMPPbase/SUSYPhotonSelector.h>
#include <PhotonEfficiencyCorrection/IAsgPhotonEfficiencyCorrectionTool.h>

namespace XAMPP {
    SUSYPhotonSelector::SUSYPhotonSelector(std::string myname) :
                SUSYParticleSelector(myname),
                m_xAODPhotons(nullptr),
                m_Photons(nullptr),
                m_PhotonsAux(nullptr),
                m_PrePhotons(nullptr),
                m_BaselinePhotons(),
                m_SignalPhotons(),
                m_SeparateSF(false),
                m_SF(),
                m_doRecoSF(true),
                m_doIsoSF(true) {
        declareProperty("SeparateSF", m_SeparateSF = false); //if this option is switched on the electron SF are saved spliited into ID, Reco, Isol and Trigger.
        declareProperty("ApplyIsoSF", m_doIsoSF = true);
        declareProperty("ApplyRecoSF", m_doRecoSF = true);
        SetContainerKey("Photons");
        SetObjectType(XAMPP::SelectionObject::Photon);
    }

    SUSYPhotonSelector::~SUSYPhotonSelector() {
    }

    StatusCode SUSYPhotonSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(SUSYParticleSelector::initialize());
        ToolHandle < IAsgPhotonEfficiencyCorrectionTool > RecoTool = GetCPTool<IAsgPhotonEfficiencyCorrectionTool>("PhotonEfficiencyCorrectionTool");
        ToolHandle < IAsgPhotonEfficiencyCorrectionTool > IsoTool = GetCPTool<IAsgPhotonEfficiencyCorrectionTool>("PhotonIsolationCorrectionTool");

        for (const auto set : m_systematics->GetWeightSystematics(ObjectType())) {
            PhoWeightDecorators Decors;
            Decors.Systematic = set;
            bool WriteIso = m_doIsoSF && XAMPP::ToolIsAffectedBySystematic(IsoTool, set);
            bool WriteReco = m_doRecoSF && XAMPP::ToolIsAffectedBySystematic(RecoTool, set);
            ATH_CHECK(InitSFdecorator("", WriteIso || WriteReco, set, Decors.PhoSF, Decors.TotalSF));
            ATH_CHECK(InitSFdecorator("Reco", m_SeparateSF && WriteReco, set, Decors.PhoSF_Reco, Decors.TotalSF_Reco));
            ATH_CHECK(InitSFdecorator("Isol", m_SeparateSF && WriteIso, set, Decors.PhoSF_Isol, Decors.TotalSF_Isol));
            m_SF.push_back(Decors);
        }

        return StatusCode::SUCCESS;
    }

    StatusCode SUSYPhotonSelector::LoadContainers() {
        if (!ProcessObject()) return StatusCode::SUCCESS;
        ATH_CHECK(LoadContainer(ContainerKey(), m_xAODPhotons));
        for (auto& ScaleFactors : m_SF) {
            ATH_CHECK(ScaleFactors.TotalSF->ConstStore(1.));
        }

        return StatusCode::SUCCESS;
    }
    PhoLink SUSYPhotonSelector::GetLink(const xAOD::Photon& ph) const {
        return PhoLink(*m_Photons, ph.index());
    }
    PhoLink SUSYPhotonSelector::GetOrigLink(const xAOD::Photon& ph) const {
        const xAOD::Photon* Oph = dynamic_cast<const xAOD::Photon*>(xAOD::getOriginalObject(ph));
        return PhoLink(*m_xAODPhotons, Oph != nullptr ? Oph->index() : ph.index());
    }
    xAOD::PhotonContainer* SUSYPhotonSelector::GetCustomPhotons(std::string kind) {

        xAOD::PhotonContainer* customPhotons = nullptr;
        if (LoadViewElementsContainer(kind, customPhotons).isSuccess()) return customPhotons;
        return m_BaselinePhotons;

    }

    StatusCode SUSYPhotonSelector::CallSUSYTools() {
        ATH_MSG_DEBUG("Calling SUSYPhotonSelector::CallSUSYTools()..");
        return m_susytools->GetPhotons(m_Photons, m_PhotonsAux, false);
    }

    StatusCode SUSYPhotonSelector::InitialFill(const CP::SystematicSet &systset) {
        SetSystematics(systset);
        if (ProcessObject()) ATH_CHECK(FillFromSUSYTools(m_Photons, m_PhotonsAux, m_PrePhotons));
        else {
            ATH_CHECK(ViewElementsContainer("cont", m_Photons));
            ATH_CHECK(ViewElementsContainer("presel", m_PrePhotons));
        }
        return StatusCode::SUCCESS;
    }

    StatusCode SUSYPhotonSelector::FillPhotons(const CP::SystematicSet &systset) {

        SetSystematics(systset);
        ATH_CHECK(ViewElementsContainer("baseline", m_BaselinePhotons));
        ATH_CHECK(ViewElementsContainer("signal", m_SignalPhotons));
        for (const auto& iphot : *m_PrePhotons) {
            if (PassBaseline(*iphot)) m_BaselinePhotons->push_back(iphot);
            if (PassSignal(*iphot)) m_SignalPhotons->push_back(iphot);
        }

        ATH_MSG_DEBUG("Number of all photons: " << m_Photons->size());
        ATH_MSG_DEBUG("Number of preselected photons: " << m_PrePhotons->size());
        ATH_MSG_DEBUG("Number of selected baseline photons: " << m_BaselinePhotons->size());
        ATH_MSG_DEBUG("Number of selected signal photons: " << m_SignalPhotons->size());

        return StatusCode::SUCCESS;
    }
    bool SUSYPhotonSelector::PassSignal(const xAOD::IParticle &P) const {
        return ParticleSelector::PassSignal(P) && PassIsolation(P);
    }
    StatusCode SUSYPhotonSelector::SaveScaleFactor() {
        if (m_SF.empty()) return StatusCode::SUCCESS;
        // double SUSYObjDef_xAOD::GetSignalPhotonSF(const xAOD::Photon& ph, const bool effSF, const bool isoSF)
        ATH_MSG_DEBUG("Save SF of " << m_SignalPhotons->size() << " photons");
        const CP::SystematicSet* kineSet = m_systematics->GetCurrent();
        for (auto const& ScaleFactors : m_SF) {
            if (kineSet != m_systematics->GetNominal() && ScaleFactors.Systematic != m_systematics->GetNominal()) continue;
            ATH_CHECK(m_systematics->setSystematic(ScaleFactors.Systematic));
            double TotRecoSF(1.), TotIsoSF(1.);
            for (const auto& iphoton : *m_SignalPhotons) {
                double RecoSF = (m_doRecoSF ? m_susytools->GetSignalPhotonSF(*iphoton, m_doRecoSF, false) : 1.);
                double IsoSF = (m_doIsoSF ? m_susytools->GetSignalPhotonSF(*iphoton, false, m_doIsoSF) : 1.);
                if (m_doRecoSF) (*ScaleFactors.PhoSF_Reco)(*iphoton) = RecoSF;
                if (m_doIsoSF) (*ScaleFactors.PhoSF_Isol)(*iphoton) = IsoSF;
                (*ScaleFactors.PhoSF)(*iphoton) = RecoSF * IsoSF;
                TotRecoSF *= RecoSF;
                TotIsoSF *= IsoSF;
            }
            ATH_CHECK(ScaleFactors.TotalSF_Reco->Store(TotRecoSF));
            ATH_CHECK(ScaleFactors.TotalSF_Isol->Store(TotIsoSF));
            ATH_CHECK(ScaleFactors.TotalSF->Store(TotRecoSF * TotIsoSF));
        }
        return StatusCode::SUCCESS;
    }
}

