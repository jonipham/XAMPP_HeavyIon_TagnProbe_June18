#include <XAMPPbase/EventInfo.h>
#include <SUSYTools/ISUSYObjDef_xAODTool.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/SUSYTriggerTool.h>
#include <XAMPPbase/TreeHelpers.h>
#include <xAODTrigMissingET/TrigMissingETContainer.h>
#include <xAODTrigger/EnergySumRoI.h>
namespace XAMPP {

    int TriggerInterface::m_AddOffThreshold = 0;
    bool TriggerInterface::m_SavePrescaling = false;
    std::vector<std::string> m_IdExp { "noL1", "tc_ecm", "tc_em", "loose" };
    TriggerInterface::TriggerInterface(const std::string &Name, SUSYTriggerTool* TriggerTool) :
                m_name(Name),
                m_Type(TriggerType::UnDefined),
                m_TriggerTool(TriggerTool),
                m_XAMPPInfo(nullptr),
                m_TriggerStore(nullptr),
                m_MatchingStore(nullptr),
                m_PreScalingStore(nullptr),
                m_MatchingDecorator("TrigMatch" + Name),
                m_MatchingAccessor("TrigMatch" + Name),
                m_MatchEle(false),
                m_MatchMuo(false),
                m_MatchTau(false),
                m_MatchPho(false),
                m_CalibEleStore(nullptr),
                m_CalibMuoStore(nullptr),
                m_CalibTauStore(nullptr),
                m_CalibPhoStore(nullptr),
                m_Thresholds() {
    }
    void TriggerInterface::SetMatchingThreshold(float Thresh) {
        m_AddOffThreshold = Thresh;
    }
    void TriggerInterface::SaveTriggerPrescaling(bool B) {
        m_SavePrescaling = B;
    }

    std::string TriggerInterface::name() const {
        return m_name;
    }
    std::string TriggerInterface::MatchStoreName() const {
        if (!m_MatchingStore) {
            Warning("TriggerInterface()", "Trigger has no matching requirement");
            return "Trigger";
        }
        return m_MatchingStore->name();
    }
    std::string TriggerInterface::StoreName() const {
        return m_TriggerStore->name();
    }
    StatusCode TriggerInterface::initialize(XAMPP::EventInfo* Info) {
        //Retrieve the needed tools from the TriggerTool instance via property Manager
#ifdef XAOD_STANDALONE       
        m_susytools = (*m_TriggerTool->getProperty < ToolHandle < ST::ISUSYObjDef_xAODTool >> ("SUSYTools"));

#else
        //Some strange noise is out here
        m_susytools = ToolHandle < ST::ISUSYObjDef_xAODTool > (m_TriggerTool->getProperty("SUSYTools").toString());
#endif
        m_XAMPPInfo = Info;

        //Check whether you can add The decision variable to the output
        if (!m_XAMPPInfo->NewEventVariable<char>("Trig" + name()).isSuccess()) {
            return StatusCode::FAILURE;
        }
        m_TriggerStore = m_XAMPPInfo->GetVariableStorage<char>("Trig" + name());

        //Trigger prescaling for data
        if (m_SavePrescaling && !m_XAMPPInfo->NewEventVariable<float>("PreScale" + name()).isSuccess()) {
            return StatusCode::FAILURE;
        }
        if (m_SavePrescaling) {
            m_PreScalingStore = m_XAMPPInfo->GetVariableStorage<float>("PreScale" + name());
        }
        GetMatchingThresholds();
        m_MatchEle = AssignMatching(xAOD::Type::ObjectType::Electron);
        m_MatchMuo = AssignMatching(xAOD::Type::ObjectType::Muon);
        m_MatchPho = AssignMatching(xAOD::Type::ObjectType::Photon);

        //No matching requirement need to be done -> MET trigger
        if (!NeedsTriggerMatching()) {
            m_Type = TriggerType::MET;
            return StatusCode::SUCCESS;
        }
        //Add the global trigger matching decision to the output
        if (!m_XAMPPInfo->NewEventVariable<char>("TrigMatch" + name()).isSuccess()) {
            return StatusCode::FAILURE;
        }
        m_MatchingStore = m_XAMPPInfo->GetVariableStorage<char>("TrigMatch" + name());
        //Particle matching element links
        if (NeedsElectronMatching()) {
            if (!m_XAMPPInfo->NewEventVariable<MatchedEl>("TrM_el" + name()).isSuccess()) {
                return StatusCode::FAILURE;
            } else {
                m_CalibEleStore = m_XAMPPInfo->GetVariableStorage<MatchedEl>("TrM_el" + name());
            }
        }
        if (NeedsMuonMatching()) {
            if (!m_XAMPPInfo->NewEventVariable<MatchedMuo>("TrM_mu" + name()).isSuccess()) {
                return StatusCode::FAILURE;
            } else {
                m_CalibMuoStore = m_XAMPPInfo->GetVariableStorage<MatchedMuo>("TrM_mu" + name());
            }
        }
        if (NeedsTauMatching()) {
            if (!m_XAMPPInfo->NewEventVariable<MatchedTau>("TrM_tau" + name()).isSuccess()) {
                return StatusCode::FAILURE;
            } else {
                m_CalibTauStore = m_XAMPPInfo->GetVariableStorage<MatchedTau>("TrM_tau" + name());
            }
        }
        if (NeedsPhotonMatching()) {
            if (!m_XAMPPInfo->NewEventVariable<MatchedPho>("TrM_pho" + name()).isSuccess()) {
                return StatusCode::FAILURE;
            } else {
                m_CalibPhoStore = m_XAMPPInfo->GetVariableStorage<MatchedPho>("TrM_pho" + name());
            }
        }
        return StatusCode::SUCCESS;
    }

    void TriggerInterface::NewEvent() {
        bool PassTrigger = m_susytools->IsTrigPassed(name());
        if (!m_TriggerStore->ConstStore(PassTrigger).isSuccess()) {
            return;
        }
        if (m_SavePrescaling) {
            if (!m_PreScalingStore->ConstStore(m_susytools->GetTrigPrescale(name())).isSuccess()) {
                return;
            }
        }
        if (NeedsElectronMatching()) {
            PerformUncalibratedMatching(m_TriggerTool->UnCalibElectrons());
        }
        if (NeedsMuonMatching()) {
            PerformUncalibratedMatching(m_TriggerTool->UnCalibMuons());
        }
        if (NeedsPhotonMatching()) {
            PerformUncalibratedMatching(m_TriggerTool->UnCalibPhotons());
        }
        if (NeedsTauMatching()) {
            PerformUncalibratedMatching(m_TriggerTool->UnCalibTaus());
        }
    }
    bool TriggerInterface::PassTrigger() const {
        return m_TriggerStore->GetValue();
    }
    bool TriggerInterface::AssignMatching(xAOD::Type::ObjectType T) const {
        for (auto &thrs : m_Thresholds) {
            if (thrs.Object == T) return true;
        }
        return false;
    }
    void TriggerInterface::GetMatchingThresholds() {
        std::string TriggerString = name();
        xAOD::Type::ObjectType Type = xAOD::Type::ObjectType::Other;
        //Removing expressions which are not separated by _ from the object definition
        for (auto& Exp : m_IdExp)
            TriggerString = RemoveAllExpInStr(TriggerString, Exp);
        int Multiple = 0;
        int Thrs = ExtractPtThreshold(TriggerString, Multiple, Type);
        while (Thrs != -1) {
            OfflineMatching M;
            M.PtThreshold = Thrs;
            M.Object = Type;
            m_Thresholds.push_back(M);
            if (Multiple == 0) Thrs = ExtractPtThreshold(TriggerString, Multiple, Type);
            else --Multiple;
        }
        for (auto& M : m_Thresholds) {
            if (M.Object == xAOD::Type::ObjectType::Muon && m_Thresholds.size() == 1) {
                M.PtThreshold *= 1.05;
            } else {
                M.PtThreshold += m_AddOffThreshold;
            }
        }
        SortThresholds();
    }
    int TriggerInterface::ExtractPtThreshold(std::string &TriggerString, int &M, xAOD::Type::ObjectType &T) {
        M = 0;
        size_t Pos = -1;
        //Look for single electron photon and muon trigger expressions
        if (TriggerString.find("_e") < Pos || TriggerString.find("_mu") < Pos || TriggerString.find("_g") < Pos) {
            if (TriggerString.find("_e") < TriggerString.find("_mu") && TriggerString.find("_e") < TriggerString.find("_g")) {
                Pos = TriggerString.find("_e") + 2;
                T = xAOD::Type::ObjectType::Electron;
            } else if (TriggerString.find("_g") < TriggerString.find("_mu") && TriggerString.find("_g") < TriggerString.find("_e")) {
                Pos = TriggerString.find("_g") + 2;
                T = xAOD::Type::ObjectType::Photon;
            } else {
                Pos = TriggerString.find("_mu") + 3;
                T = xAOD::Type::ObjectType::Muon;
            }
            M = 0;
        }
        //Look for di electron photon and muon trigger expressions
        if (TriggerString.find("_2e") < Pos || TriggerString.find("_2mu") < Pos || TriggerString.find("_2g") < Pos) {
            if (TriggerString.find("_2e") < TriggerString.find("_2mu") && TriggerString.find("_2e") < TriggerString.find("_2g")) {
                Pos = TriggerString.find("_2e") + 3;
                T = xAOD::Type::ObjectType::Electron;
            } else if (TriggerString.find("_2g") < TriggerString.find("_2mu") && TriggerString.find("_2g") < TriggerString.find("_2e")) {
                Pos = TriggerString.find("_2g") + 3;
                T = xAOD::Type::ObjectType::Photon;
            } else {
                Pos = TriggerString.find("_2mu") + 4;
                T = xAOD::Type::ObjectType::Muon;
            }
            M = 1;
        }
        //Look for tri electron photon and muon trigger expressions
        if (TriggerString.find("_3e") < Pos || TriggerString.find("_3mu") < Pos || TriggerString.find("_3g") < Pos) {
            if (TriggerString.find("_3e") < TriggerString.find("_3mu") && TriggerString.find("_3e") < TriggerString.find("_3g")) {
                Pos = TriggerString.find("_3e") + 3;
                T = xAOD::Type::ObjectType::Electron;
            } else if (TriggerString.find("_3g") < TriggerString.find("_3mu") && TriggerString.find("_3g") < TriggerString.find("_3e")) {
                Pos = TriggerString.find("_3g") + 3;
                T = xAOD::Type::ObjectType::Photon;
            } else {
                Pos = TriggerString.find("_3mu") + 4;
                T = xAOD::Type::ObjectType::Muon;
            }
            M = 2;
        }
        if (Pos < TriggerString.size()) {
            TriggerString = TriggerString.substr(Pos, TriggerString.size());
            float Thrs = atof(TriggerString.substr(0, TriggerString.find("_")).c_str()) * 1000;
            if (TriggerString.find("_") < TriggerString.size()) TriggerString = TriggerString.substr(TriggerString.find("_"), TriggerString.size());
            else TriggerString = "";
            return Thrs;
        }
        return -1;
    }
    void TriggerInterface::SortThresholds() {
        std::vector<OfflineMatching> UnSorted = m_Thresholds;
        std::vector<size_t> Sorted;
        m_Thresholds.clear();
        while (UnSorted.size() != m_Thresholds.size()) {
            for (size_t u = 0; u < UnSorted.size(); ++u) {
                if (IsInVector(u, Sorted)) continue;
                size_t s = u;
                for (size_t u1 = 0; u1 < UnSorted.size(); ++u1) {
                    if (u1 == u || IsInVector(u1, Sorted)) continue;
                    if (UnSorted.at(u1).PtThreshold > UnSorted.at(s).PtThreshold) s = u1;
                }
                m_Thresholds.push_back(UnSorted.at(s));
                Sorted.push_back(s);
            }
        }
        Sorted.clear();
        UnSorted.clear();
    }

    bool TriggerInterface::PassTriggerMatching() {
        if (!NeedsTriggerMatching()) {
            return false;
        }
        //The matching store is already filled
        //Return whats in there
        if (m_MatchingStore->isAvailable()) {
            return m_MatchingStore->GetValue();
        }

        //The matching store needs to be filled
        if (!m_MatchingStore->Store(false)) {
            return false;
        }

        unsigned int NumMatches = 0;
        if (PassTrigger()) {
            ResetObjectMatching();
            if (NeedsElectronMatching()) {
                if (!FillLinkVector(m_TriggerTool->CalibElectrons(), m_CalibEleStore)) {
                    return false;
                }
                NumMatches += GetCalibMatches(m_CalibEleStore);
            }
            if (NeedsMuonMatching()) {
                if (!FillLinkVector(m_TriggerTool->CalibMuons(), m_CalibMuoStore)) {
                    return false;
                }
                NumMatches += GetCalibMatches(m_CalibMuoStore);
            }
            if (NeedsTauMatching()) {
                if (!FillLinkVector(m_TriggerTool->CalibTaus(), m_CalibTauStore)) {
                    return false;
                }
                NumMatches += GetCalibMatches(m_CalibTauStore);
            }
            if (NeedsPhotonMatching()) {
                if (!FillLinkVector(m_TriggerTool->CalibPhotons(), m_CalibPhoStore)) {
                    return false;
                }
                NumMatches += GetCalibMatches(m_CalibPhoStore);
            }
        }
        if (!m_MatchingStore->Store(NumMatches >= m_Thresholds.size())) {
            return false;
        }
        return m_MatchingStore->GetValue();
    }
    bool TriggerInterface::IsTriggerMatched(const xAOD::IParticle* Particle) const {
        if (!m_MatchingAccessor.isAvailable(*Particle)) {
            Warning("TriggerInterface", Form("No matching information available for %s", name().c_str()));
            PromptParticle(Particle);
            return false;
        }
        if (!m_MatchingAccessor(*Particle)) {
            return false;
        }
        for (const auto& Thrs : m_Thresholds) {
            if (Thrs.Object != Particle->type()) {
                continue;
            }
            if (Thrs.PtThreshold < Particle->pt()) {
                return true;
            }
        }
        return false;
    }
    bool TriggerInterface::NeedsElectronMatching() const {
        return m_MatchEle;
    }
    bool TriggerInterface::NeedsMuonMatching() const {
        return m_MatchMuo;
    }
    bool TriggerInterface::NeedsTauMatching() const {
        return m_MatchTau;
    }
    bool TriggerInterface::NeedsPhotonMatching() const {
        return m_MatchPho;
    }
    bool TriggerInterface::NeedsTriggerMatching() const {
        return NeedsElectronMatching() || NeedsMuonMatching() || NeedsTauMatching() || NeedsPhotonMatching();
    }
    void TriggerInterface::PerformUncalibratedMatching(const xAOD::IParticleContainer* UnCalibrated) {
        if (!UnCalibrated) return;
        for (const auto Particle : *UnCalibrated) {
            m_MatchingDecorator(*Particle) = PassTrigger() && m_susytools->IsTrigMatched(Particle, name());
        }
    }
    template<typename Container, typename ElementLink> bool TriggerInterface::FillLinkVector(Container* Particles, XAMPP::Storage<ElementLink>* LinkStore) {
        ElementLink Links;
        for (const auto P : *Particles) {
            if (IsTriggerMatched(P)) {
                Links.push_back(m_TriggerTool->GetLink(P));
            }
        }
        return LinkStore->Store(Links).isSuccess();
    }
    void TriggerInterface::ResetObjectMatching() {
        for (auto& Thrs : m_Thresholds) {
            Thrs.ObjectMatched = false;
        }
    }
    template<typename ElementLink> unsigned int TriggerInterface::GetCalibMatches(XAMPP::Storage<ElementLink>* Store) {
        unsigned int M = 0;
        for (auto & Particle : Store->GetValue()) {
            for (auto& Thrs : m_Thresholds) {
                if (Thrs.ObjectMatched || Thrs.Object != (*Particle)->type()) {
                    continue;
                }
                if (Thrs.PtThreshold < (*Particle)->pt()) {
                    Thrs.ObjectMatched = true;
                    ++M;
                    break;
                }
            }
        }
        return M;
    }
    std::string TriggerInterface::PrintMatchingThresholds() const {
        std::string Thrs = " ";
        for (const auto & Trig : m_Thresholds) {
            Thrs += "pt(";
            if (Trig.Object == xAOD::Type::ObjectType::Electron) Thrs += "El";
            else if (Trig.Object == xAOD::Type::ObjectType::Muon) Thrs += "Mu";
            else if (Trig.Object == xAOD::Type::ObjectType::Photon) Thrs += "Pho";
            Thrs += Form(")>%.2f GeV, ", (Trig.PtThreshold / 1000.));
        }
        return Thrs;
    }

    //############################################################################################
    //                                      SUSYTriggerTool
    //############################################################################################

    SUSYTriggerTool::SUSYTriggerTool(std::string myname) :
                AsgTool(myname),
                m_XAMPPInfo(),
                m_XAMPPInfoHandle("EventInfoHandler"),
                m_systematics("SystematicsTool"),
                m_susytools("SUSYTools"),
#ifdef XAOD_STANDALONE
                m_output_level(MSG::INFO),
#endif
                m_elec_selection("SUSYElectronSelector"),
                m_muon_selection("SUSYMuonSelector"),
                m_phot_selection("SUSYPhotonSelector"),
                m_tau_selection("SUSYTauSelector"),
                m_triggers(),
                m_met_triggers(),
                m_1lep_triggers(),
                m_2lep_triggers(),
                m_3lep_triggers(),
                m_phot_triggers(),
#ifdef XAOD_STANDALONE

                m_met_triggers_string(),
                m_1lep_triggers_string(),
                m_2lep_triggers_string(),
                m_3lep_triggers_string(),
                m_phot_triggers_string(),
#endif
                m_init(false),
                m_Pass(false),
                m_doLVL1Met(true),
                m_doCellMet(true),
                m_doMhtMet(true),
                m_doTopoClMet(true),
                m_doTopoClPufitMet(true),
                m_doTopoClPuetaMet(true),
                m_DecTrigger(nullptr),
                m_DecMatching(nullptr) {
#ifdef XAOD_STANDALONE
        declareProperty("OutputLevel", m_output_level);
#endif
        declareProperty("METTriggers", m_met_triggers);
        declareProperty("SingleLeptonTriggers", m_1lep_triggers);
        declareProperty("DiLeptonTriggers", m_2lep_triggers);
        declareProperty("TriLeptonTriggers", m_3lep_triggers);
        declareProperty("PhotonTriggers", m_phot_triggers);
#ifdef XAOD_STANDALONE
        declareProperty("METTriggersString", m_met_triggers_string);
        declareProperty("1LeptonTriggersString", m_1lep_triggers_string);
        declareProperty("2LeptonTriggersString", m_2lep_triggers_string);
        declareProperty("3LeptonTriggersString", m_3lep_triggers_string);
        declareProperty("PhotonTriggersString", m_phot_triggers_string);
#endif
        declareProperty("DisableSkimming", m_NoCut = false);
        declareProperty("OfflineThreshold", m_OfflineThrs = 1000);
        declareProperty("StoreMatchingInfo", m_StoreObjectMatching = false);
        declareProperty("WritePrescaling", m_StorePreScaling = false);
        declareProperty("MetTrigEmulation", m_MetTrigEmulation = true);

        declareProperty("ElectronSelector", m_elec_selection);
        declareProperty("MuonSelector", m_muon_selection);
        declareProperty("PhotonSelector", m_phot_selection);
        declareProperty("TauSelector", m_tau_selection);
        declareProperty("SystematicsTool", m_systematics);

        m_susytools.declarePropertyFor(this, "SUSYTools", "The SUSYTools instance");
        m_XAMPPInfoHandle.declarePropertyFor(this, "EventInfoHandler", "The XAMPP EventInfo handle");

    }
    const xAOD::ElectronContainer* SUSYTriggerTool::UnCalibElectrons() const {
        return m_elec_selection->GetElectronContainer();
    }
    const xAOD::MuonContainer* SUSYTriggerTool::UnCalibMuons() const {
        return m_muon_selection->GetMuonContainer();
    }
    const xAOD::PhotonContainer* SUSYTriggerTool::UnCalibPhotons() const {
        return m_phot_selection->GetPhotonContainer();
    }
    const xAOD::TauJetContainer*SUSYTriggerTool::UnCalibTaus() const {
        return m_tau_selection->GetTauContainer();
    }

    xAOD::ElectronContainer* SUSYTriggerTool::CalibElectrons() const {
        return m_elec_selection->GetSignalElectrons();
    }
    xAOD::MuonContainer* SUSYTriggerTool::CalibMuons() const {
        return m_muon_selection->GetSignalMuons();
    }
    xAOD::PhotonContainer* SUSYTriggerTool::CalibPhotons() const {
        return m_phot_selection->GetSignalPhotons();
    }
    xAOD::TauJetContainer* SUSYTriggerTool::CalibTaus() const {
        return m_tau_selection->GetSignalTaus();
    }
    EleLink SUSYTriggerTool::GetLink(const xAOD::Electron* el) {
        return m_elec_selection->GetLink(*el);
    }
    MuoLink SUSYTriggerTool::GetLink(const xAOD::Muon* mu) {
        return m_muon_selection->GetLink(*mu);
    }
    PhoLink SUSYTriggerTool::GetLink(const xAOD::Photon* ph) {
        return m_phot_selection->GetLink(*ph);
    }
    TauLink SUSYTriggerTool::GetLink(const xAOD::TauJet* tau) {
        return m_tau_selection->GetLink(*tau);
    }

    StatusCode SUSYTriggerTool::initialize() {

        if (m_init) {
            return StatusCode::SUCCESS;
        }
        m_init = true;
        ATH_MSG_INFO("Initialising...");
        if (m_NoCut) ATH_MSG_INFO("Disable trigger skimming");
        ATH_CHECK(m_susytools.retrieve());
        ATH_CHECK(m_XAMPPInfoHandle.retrieve());
        m_XAMPPInfo = dynamic_cast<XAMPP::EventInfo*>(m_XAMPPInfoHandle.operator->());

        if (!isData()) {
            m_StorePreScaling = false;
        }
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("Trigger", m_NoCut));
        m_DecTrigger = m_XAMPPInfo->GetVariableStorage<char>("Trigger");
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("TrigMatching"));
        m_DecMatching = m_XAMPPInfo->GetVariableStorage<char>("TrigMatching");

        TriggerInterface::SaveTriggerPrescaling(m_StorePreScaling);
        TriggerInterface::SetMatchingThreshold(m_OfflineThrs);
#ifdef XAOD_STANDALONE
        XAMPP::FillVectorFromString(m_1lep_triggers, m_1lep_triggers_string);
        XAMPP::FillVectorFromString(m_2lep_triggers, m_2lep_triggers_string);
        XAMPP::FillVectorFromString(m_3lep_triggers, m_3lep_triggers_string);
        XAMPP::FillVectorFromString(m_phot_triggers, m_phot_triggers_string);
        XAMPP::FillVectorFromString(m_met_triggers, m_met_triggers_string);
#endif
        if (m_met_triggers.empty() || isData()) {
            m_MetTrigEmulation = false;
        }
        if (m_MetTrigEmulation) {
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TriggerLVL1Met"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TriggerCellMet"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TriggerMhtMet"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TriggerTopoClMet"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TriggerTopoClPufitMet"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TriggerTopoClPuetaMet"));
        }

        ATH_CHECK(FillTriggerVector(m_1lep_triggers));
        ATH_CHECK(FillTriggerVector(m_2lep_triggers));
        ATH_CHECK(FillTriggerVector(m_3lep_triggers));
        ATH_CHECK(FillTriggerVector(m_phot_triggers));
        ATH_CHECK(FillTriggerVector(m_met_triggers));

        ATH_MSG_INFO("The following triggers are set in the tool:");
        for (auto Trigger : m_triggers) {
            ATH_MSG_INFO("  - " << Trigger->name() << Trigger->PrintMatchingThresholds());
        }
#ifdef XAOD_STANDALONE
        this->msg().setLevel(XAMPP::setOutputLevel(m_output_level));
#endif

        ATH_CHECK(m_elec_selection.retrieve());
        ATH_CHECK(m_muon_selection.retrieve());
        ATH_CHECK(m_phot_selection.retrieve());
        ATH_CHECK(m_tau_selection.retrieve());

        return StatusCode::SUCCESS;
    }

    SUSYTriggerTool::~SUSYTriggerTool() {
        for (auto& Trigger : m_triggers) {
            delete Trigger;
        }
        ATH_MSG_DEBUG("Destructor called");
    }
    bool SUSYTriggerTool::CheckTrigger() {
        m_Pass = false;
        if (this->msg().level() == MSG::DEBUG) {
            const Trig::ChainGroup* Chain = m_susytools->GetTrigChainGroup(".*");
            for (auto &trig : Chain->getListOfTriggers()) {
                ATH_MSG_DEBUG("Trigger in the menue " << trig << " is fired " << (m_susytools->IsTrigPassed(trig) ? "Yes" : "No"));
            }
        }
        for (auto& Trigg : m_triggers) {
            Trigg->NewEvent();
            ATH_MSG_DEBUG("Trigger " << Trigg->name() << " has fired " << (Trigg->PassTrigger() ? "Yes" : "No") << ".");
            m_Pass = Trigg->PassTrigger() || m_Pass;
        }
        ATH_CHECK(m_DecTrigger->ConstStore(m_Pass));
        if (m_MetTrigEmulation) ATH_CHECK(MetTriggerEmulation());
        return (m_NoCut || m_Pass);

    }
    bool SUSYTriggerTool::CheckTriggerMatching() {
        bool PassMatching = false;
        for (auto& Trig : m_triggers) {
            PassMatching = Trig->PassTriggerMatching() || PassMatching;
        }
        ATH_CHECK(m_DecMatching->Store(PassMatching));
        return PassMatching;
    }
    bool SUSYTriggerTool::CheckTrigger(const std::string& trigger_name) {
        for (auto Trigg : m_triggers) {
            if (Trigg->name() == trigger_name) return Trigg->PassTrigger();
        }
        ATH_MSG_WARNING("The trigger " << trigger_name << " is unknown to the SUSYTriggerTool. Please check your configs");
        return m_susytools->IsTrigPassed(trigger_name);
    }
    StatusCode SUSYTriggerTool::FillTriggerVector(std::vector<std::string> &triggers_vector) {
        for (auto trigger : triggers_vector) {
            TriggerInterface* Trigg = new TriggerInterface(trigger, this);
            ATH_CHECK(Trigg->initialize(m_XAMPPInfo));
            m_triggers.push_back(Trigg);
        }
        triggers_vector.clear();
        return StatusCode::SUCCESS;
    }
    bool SUSYTriggerTool::IsMatchedObject(const xAOD::Electron* el, const std::string& Trig) {
        XAMPP::Storage<MatchedEl>* dec_Trig = m_XAMPPInfo->GetVariableStorage<MatchedEl>("TrM_el" + Trig);
        if (!dec_Trig) return false;
        for (const auto& TEl : dec_Trig->GetValue()) {
            if (*TEl == el) return true;
        }
        return false;
    }

    bool SUSYTriggerTool::IsMatchedObject(const xAOD::Muon* mu, const std::string & Trig) {
        XAMPP::Storage<MatchedMuo>* dec_Trig = m_XAMPPInfo->GetVariableStorage<MatchedMuo>("TrM_mu" + Trig);
        if (!dec_Trig) return false;
        for (const auto& TMu : dec_Trig->GetValue()) {
            if (*TMu == mu) return true;
        }
        return false;
    }
    bool SUSYTriggerTool::IsMatchedObject(const xAOD::Photon* ph, const std::string & Trig) {
        XAMPP::Storage<MatchedPho>* dec_Trig = m_XAMPPInfo->GetVariableStorage<MatchedPho>("TrM_ph" + Trig);
        if (!dec_Trig) return false;
        for (const auto& TPho : dec_Trig->GetValue()) {
            if (*TPho == ph) return true;
        }
        return false;
    }
    StatusCode SUSYTriggerTool::MetTriggerEmulation() {
        static XAMPP::Storage<float>* dec_TriggerLVL1Met = m_XAMPPInfo->GetVariableStorage<float>("TriggerLVL1Met");
        static XAMPP::Storage<float>* dec_TriggerCellMet = m_XAMPPInfo->GetVariableStorage<float>("TriggerCellMet");
        static XAMPP::Storage<float>* dec_TriggerMhtMet = m_XAMPPInfo->GetVariableStorage<float>("TriggerMhtMet");
        static XAMPP::Storage<float>* dec_TriggerTopoClMet = m_XAMPPInfo->GetVariableStorage<float>("TriggerTopoClMet");
        static XAMPP::Storage<float>* dec_TriggerTopoClPufitMet = m_XAMPPInfo->GetVariableStorage<float>("TriggerTopoClPufitMet");
        static XAMPP::Storage<float>* dec_TriggerTopoClPuetaMet = m_XAMPPInfo->GetVariableStorage<float>("TriggerTopoClPuetaMet");
        // the following is taken from https://twiki.cern.ch/twiki/bin/viewauth/Atlas/METTriggerEmulation
        if (m_doLVL1Met && !evtStore()->contains < xAOD::EnergySumRoI > ("LVL1EnergySumRoI")) {
            ATH_MSG_WARNING("The file does not contain the EnergySumRoI LVL1EnergySumRoI, switch off the retrieving for the rest of the job.");
            m_doLVL1Met = false;
        }
        float l1_met = -1.;
        if (m_doLVL1Met) {
            const xAOD::EnergySumRoI* l1MetObject = nullptr;
            ATH_CHECK(evtStore()->retrieve(l1MetObject, "LVL1EnergySumRoI"));
            float l1_mex = l1MetObject->exMiss();
            float l1_mey = l1MetObject->eyMiss();
            l1_met = sqrt(l1_mex * l1_mex + l1_mey * l1_mey);
        }
        ATH_CHECK(dec_TriggerLVL1Met->ConstStore(l1_met));
        float hltMet = -1.;
        ATH_CHECK(GetHLTMet(hltMet, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET", m_doCellMet));
        ATH_CHECK(dec_TriggerCellMet->ConstStore(hltMet));
        ATH_CHECK(GetHLTMet(hltMet, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht", m_doMhtMet));
        ATH_CHECK(dec_TriggerMhtMet->ConstStore(hltMet));
        ATH_CHECK(GetHLTMet(hltMet, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl", m_doTopoClMet));
        ATH_CHECK(dec_TriggerTopoClMet->ConstStore(hltMet));
        ATH_CHECK(GetHLTMet(hltMet, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC", m_doTopoClPufitMet));
        ATH_CHECK(dec_TriggerTopoClPufitMet->ConstStore(hltMet));
        ATH_CHECK(GetHLTMet(hltMet, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS", m_doTopoClPuetaMet));
        ATH_CHECK(dec_TriggerTopoClPuetaMet->ConstStore(hltMet));
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTriggerTool::GetHLTMet(float &met, const std::string &containerName, bool &doContainer) {
        if (!doContainer) {
            met = -1;
            return StatusCode::SUCCESS;
        }
        if (!m_susytools->IsTrigPassed("L1_XE50")) {
            met = -1;
            return StatusCode::SUCCESS;
        }
        if (!evtStore()->contains < xAOD::TrigMissingETContainer > (containerName)) {
            ATH_MSG_WARNING("The file does not contain the TrigMissingETContainer " << containerName << ", switch off the retrieving for the rest of the job.");
            met = -1.;
            doContainer = false;
            return StatusCode::SUCCESS;
        }
        const xAOD::TrigMissingETContainer* hltCont = nullptr;
        ATH_CHECK(evtStore()->retrieve(hltCont, containerName));
        float ex = hltCont->front()->ex();
        float ey = hltCont->front()->ey();
        met = sqrt(ex * ex + ey * ey);
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTriggerTool::SaveObjectMatching(ParticleStorage* Storage, xAOD::Type::ObjectType Type) {
        if (!m_StoreObjectMatching) {
            ATH_MSG_INFO("Tool not configured to store the matching decsion per object");
            return StatusCode::SUCCESS;
        }
        for (auto& Trig : m_triggers) {
            if (Trig->NeedsElectronMatching() && Type == xAOD::Type::ObjectType::Electron) ATH_CHECK(Storage->SaveVariable<char>(Trig->MatchStoreName()));
            if (Trig->NeedsMuonMatching() && Type == xAOD::Type::ObjectType::Muon) ATH_CHECK(Storage->SaveVariable<char>(Trig->MatchStoreName()));
            if (Trig->NeedsTauMatching() && Type == xAOD::Type::ObjectType::Tau) ATH_CHECK(Storage->SaveVariable<char>(Trig->MatchStoreName()));
            if (Trig->NeedsPhotonMatching() && Type == xAOD::Type::ObjectType::Photon) ATH_CHECK(Storage->SaveVariable<char>(Trig->MatchStoreName()));

        }
        return StatusCode::SUCCESS;
    }
    bool SUSYTriggerTool::isData() const {
        return m_systematics->isData();
    }
}
