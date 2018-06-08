#include <XAMPPbase/SUSYElectronSelector.h>
#include <XAMPPbase/AnalysisUtils.h>

#include <ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h>

#include <XAMPPbase/ToolHandleSystematics.h>
#include <SUSYTools/SUSYObjDef_xAOD.h>

namespace XAMPP {
    SUSYElectronSelector::SUSYElectronSelector(std::string myname) :
                SUSYParticleSelector(myname),
                m_BaselineD0SigCut(-1),
                m_BaselineZ0SinThetaCut(-1),
                m_SignalD0SigCut(-1),
                m_SignalZ0SinThetaCut(-1),
                m_RequireIsoSignal(true),
                m_xAODElectrons(nullptr),
                m_Electrons(nullptr),
                m_ElectronsAux(nullptr),
                m_PreElectrons(nullptr),
                m_BaselineElectrons(nullptr),
                m_SignalElectrons(nullptr),
                m_SF(),
                m_SFTools(),
                m_IdHandle(""),
                m_RecoHandle(""),
                m_IsoHandle(""),
                m_SeparateSF(false),
                m_doRecoSF(true),
                m_doIdSF(true),
                m_doTriggerSF(true),
                m_doIsoSF(true),
                m_TriggerExp(),
                m_TriggerSFConf(),
#ifdef XAOD_STANDALONE
                m_TriggerExp_str(),
                m_TrigSFConf_str(),
#endif
                m_StoreMultiTrigSF(false) {
        declareProperty("BaslineIP_d0Cut", m_BaselineD0SigCut);
        declareProperty("BaslineIP_Z0Cut", m_BaselineZ0SinThetaCut);

        declareProperty("SignalIP_d0Cut", m_SignalD0SigCut);
        declareProperty("SignalIP_Z0Cut", m_SignalZ0SinThetaCut);
        declareProperty("RequireIsoSignal", m_RequireIsoSignal);

        declareProperty("SeparateSF", m_SeparateSF); //if this option is switched on the electron SF are saved split into ID, Reco, Isol and Trigger.
        declareProperty("ApplyIsoSF", m_doIsoSF);
        declareProperty("ApplyRecoSF", m_doRecoSF);
        declareProperty("ApplyIdSF", m_doIdSF);
        declareProperty("ApplyTriggerSF", m_doTriggerSF);

        declareProperty("SFTrigger", m_TriggerExp);
        declareProperty("TriggerSFconfig", m_TriggerSFConf);

#ifdef XAOD_STANDALONE
        declareProperty("SFTriggerString", m_TriggerExp_str);
        declareProperty("TriggerSFconfigString", m_TrigSFConf_str);
#endif

        SetContainerKey("Electrons");
        SetObjectType(XAMPP::SelectionObject::Electron);

    }

    SUSYElectronSelector::~SUSYElectronSelector() {
        for (auto const& ScaleFactors : m_SF) {
            delete ScaleFactors.EleSF;
            delete ScaleFactors.EleSF_Reco;
            delete ScaleFactors.EleSF_Isol;
            delete ScaleFactors.EleSF_ID;
            for (auto T : ScaleFactors.TriggerSFs)
                delete T;
        }
    }
    StatusCode SUSYElectronSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(SUSYParticleSelector::initialize());
#ifdef XAOD_STANDALONE
        XAMPP::FillVectorFromString(m_TriggerExp,m_TriggerExp_str);
        XAMPP::FillVectorFromString(m_TriggerSFConf, m_TrigSFConf_str);
#endif

        if (m_TriggerExp.empty() && m_doTriggerSF) {
            ATH_MSG_INFO("No triggers were given... Switch trigger SF off");
            m_doTriggerSF = false;
        }
        m_StoreMultiTrigSF = m_TriggerExp.size() > 1;
        if (m_doTriggerSF) {
            ATH_CHECK(initializeTriggerSFTools());
        }
        m_IdHandle = GetCPTool<EleEffTool>("ElectronEfficiencyCorrectionTool_id");
        m_RecoHandle = GetCPTool<EleEffTool>("ElectronEfficiencyCorrectionTool_reco");
        m_IsoHandle = GetCPTool<EleEffTool>("ElectronEfficiencyCorrectionTool_iso");

        for (const auto set : m_systematics->GetWeightSystematics(ObjectType())) {
            EleWeightDecorators Decors;
            Decors.Systematic = set;
            //Have a look at the logfiles from your jobs. The affected type is specified in the name. Why copy then information?            
            bool WriteReco = m_doRecoSF && XAMPP::ToolIsAffectedBySystematic(m_RecoHandle, set);
            bool WriteIso = m_doIsoSF && XAMPP::ToolIsAffectedBySystematic(m_IsoHandle, set);
            bool WriteId = m_doIdSF && XAMPP::ToolIsAffectedBySystematic(m_IdHandle, set);
            bool WriteTrig = m_doTriggerSF && XAMPP::ToolIsAffectedBySystematic((m_SFTools.begin()->second), set);
            if (!(WriteReco || WriteIso || WriteId || WriteTrig)) {
                continue;
            }
            ATH_CHECK(InitSFdecorator("", (WriteReco || WriteIso || (WriteTrig && !m_StoreMultiTrigSF) || WriteId), set, Decors.EleSF, Decors.TotalSF));
            ATH_CHECK(InitSFdecorator("Reco", m_SeparateSF && WriteReco, set, Decors.EleSF_Reco, Decors.TotalSF_Reco));
            ATH_CHECK(InitSFdecorator("Isol", m_SeparateSF && WriteIso, set, Decors.EleSF_Isol, Decors.TotalSF_Isol));
            ATH_CHECK(InitSFdecorator("ID", m_SeparateSF && WriteId, set, Decors.EleSF_ID, Decors.TotalSF_ID));

            for (const auto& T : m_TriggerExp) {
                XAMPP::Storage<double>* TotalTrigSF;
                SG::AuxElement::Decorator<double>* PartTrigSF;
                std::string ToolKey = FindBestSFTool(T);
                if (ToolKey.empty()) {
                    ATH_MSG_WARNING("No suitable SF tool found for SF " << T);
                    continue;
                }
                ATH_CHECK(InitSFdecorator("Trig" + (m_StoreMultiTrigSF ? "_" + T : ""), (m_StoreMultiTrigSF || m_SeparateSF) && WriteTrig, set, PartTrigSF, TotalTrigSF));
                ElectronTriggerSFHandler* SFHandle = new ElectronTriggerSFHandler(T, m_susytools.getHandle(), m_SFTools.at(ToolKey), m_XAMPPInfo);
                Decors.TriggerSFs.push_back(SFHandle);
                SFHandle->SetTriggerStorage(TotalTrigSF);
                SFHandle->SetTriggerDecorator(PartTrigSF);
                if (m_StoreMultiTrigSF) ATH_CHECK(SFHandle->initialize());
                else ATH_CHECK(SFHandle->initialize(Decors));
            }
            m_SF.push_back(Decors);
        }
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYElectronSelector::LoadContainers() {
        if (!ProcessObject()) return StatusCode::SUCCESS;
        ATH_CHECK(LoadContainer(ContainerKey(), m_xAODElectrons));
        for (auto& ScaleFactors : m_SF) {
            ATH_CHECK(ScaleFactors.TotalSF->ConstStore(1.));
        }
        return StatusCode::SUCCESS;
    }
    EleLink SUSYElectronSelector::GetLink(const xAOD::Electron& el) const {
        return EleLink(*m_Electrons, el.index());
    }
    EleLink SUSYElectronSelector::GetOrigLink(const xAOD::Electron& el) const {
        const xAOD::Electron* Oel = dynamic_cast<const xAOD::Electron*>(xAOD::getOriginalObject(el));
        return EleLink(*m_xAODElectrons, Oel != NULL ? Oel->index() : el.index());
    }
    xAOD::ElectronContainer* SUSYElectronSelector::GetCustomElectrons(std::string kind) {

        xAOD::ElectronContainer* customElectrons = NULL;
        if (LoadViewElementsContainer(kind, customElectrons).isSuccess()) return customElectrons;
        return m_BaselineElectrons;

    }
    StatusCode SUSYElectronSelector::CallSUSYTools() {
        ATH_MSG_DEBUG("Calling SUSYElectronSelector::CallSUSYTools()..");
        return m_susytools->GetElectrons(m_Electrons, m_ElectronsAux, false);
    }
    StatusCode SUSYElectronSelector::InitialFill(const CP::SystematicSet &systset) {
        SetSystematics(systset);
        if (ProcessObject()) {
            ATH_CHECK(FillFromSUSYTools(m_Electrons, m_ElectronsAux, m_PreElectrons));
        } else {
            ATH_CHECK(ViewElementsContainer("container", m_Electrons));
            ATH_CHECK(ViewElementsContainer("presel", m_PreElectrons));
        }
        return StatusCode::SUCCESS;
    }

    StatusCode SUSYElectronSelector::LoadSelection(const CP::SystematicSet &systset) {
        ATH_CHECK(LoadPreSelectedContainer(m_PreElectrons, &systset));
        SetSystematics(systset);
        ATH_CHECK(LoadViewElementsContainer("baseline", m_BaselineElectrons));
        ATH_CHECK(LoadViewElementsContainer("signal", m_SignalElectrons));
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYElectronSelector::FillElectrons(const CP::SystematicSet &systset) {

        SetSystematics(systset);
        ATH_CHECK(ViewElementsContainer("baseline", m_BaselineElectrons));
        ATH_CHECK(ViewElementsContainer("signal", m_SignalElectrons));

        for (const auto& ielec : *m_PreElectrons) {
            if (PassBaseline(*ielec)) m_BaselineElectrons->push_back(ielec);
            else continue;
            if (PassSignal(*ielec)) m_SignalElectrons->push_back(ielec);
        }

        ATH_MSG_DEBUG("Number of all electrons: " << m_Electrons->size());
        ATH_MSG_DEBUG("Number of preselected electrons: " << m_PreElectrons->size());
        ATH_MSG_DEBUG("Number of selected baseline electrons: " << m_BaselineElectrons->size());
        ATH_MSG_DEBUG("Number of selected signal electrons: " << m_SignalElectrons->size());

        return StatusCode::SUCCESS;
    }
    bool SUSYElectronSelector::PassBaseline(const xAOD::IParticle &P) const {
        if (!ParticleSelector::PassBaseline(P)) return false;
        static SG::AuxElement::ConstAccessor<float> dec_z0sinTheta("z0sinTheta");
        static SG::AuxElement::ConstAccessor<float> dec_d0sig("d0sig");
        if (m_BaselineZ0SinThetaCut >= 0. && fabs(dec_z0sinTheta(P)) >= m_BaselineZ0SinThetaCut) return false;
        if (m_BaselineD0SigCut >= 0. && fabs(dec_d0sig(P)) >= m_BaselineD0SigCut) return false;
        return true;
    }
    bool SUSYElectronSelector::PassSignal(const xAOD::IParticle &P) const {
          if (!ParticleSelector::PassSignal(P)) return false;
          static FloatAccessor acc_z0sinTheta("z0sinTheta");
          static FloatAccessor acc_d0sig("d0sig");
          if (m_SignalZ0SinThetaCut >= 0. && fabs(acc_z0sinTheta(P)) >= m_SignalZ0SinThetaCut) return false;
          if (m_SignalD0SigCut >= 0. && fabs(acc_d0sig(P)) >= m_SignalD0SigCut) return false;
          if (m_RequireIsoSignal && !PassIsolation(P)) return false;
          return true;
      }

    StatusCode SUSYElectronSelector::SaveScaleFactor() {
        if (m_SF.empty()) return StatusCode::SUCCESS;
        const CP::SystematicSet* kineSet = m_systematics->GetCurrent();
        ATH_MSG_DEBUG("Save SF of " << m_SignalElectrons->size() << " electrons");

        //m_susytools->GetSignalElecSF(const xAOD::Electron& el, const bool recoSF, const bool idSF, const bool triggerSF, const bool isoSF, const std::string& trigExpr)
        for (auto const& ScaleFactors : m_SF) {
            if (kineSet != m_systematics->GetNominal() && ScaleFactors.Systematic != m_systematics->GetNominal()) continue;
            ATH_CHECK(m_systematics->setSystematic(ScaleFactors.Systematic));
            double TotRecoSF(1.), TotIdSF(1.), TotIsoSF(1.);
            //Check whether you want to store TriggerSFs for more than one Trigger. Then first Set the TriggerSF to one
            for (const auto& ielec : *m_SignalElectrons) {
                ATH_CHECK(SaveElectronSF(*ielec, m_RecoHandle, ScaleFactors.EleSF_Reco, TotRecoSF, m_doRecoSF));
                ATH_CHECK(SaveElectronSF(*ielec, m_IsoHandle, ScaleFactors.EleSF_Isol, TotIsoSF, m_doIsoSF));
                ATH_CHECK(SaveElectronSF(*ielec, m_IdHandle, ScaleFactors.EleSF_ID, TotIdSF, m_doIdSF));
                for (const auto& Trigger : ScaleFactors.TriggerSFs) {
                    ATH_CHECK(Trigger->SaveTriggerSF(ielec));
                }
                DecorateTotalElectronSF(*ielec, ScaleFactors);
            }
            ATH_CHECK(ScaleFactors.TotalSF_Reco->Store(TotRecoSF));
            ATH_CHECK(ScaleFactors.TotalSF_Isol->Store(TotIsoSF));
            ATH_CHECK(ScaleFactors.TotalSF_ID->Store(TotIdSF));
            ATH_CHECK(ScaleFactors.TotalSF->Store(TotRecoSF * TotIdSF * TotIsoSF));
            for (auto & Trig : ScaleFactors.TriggerSFs)
                ATH_CHECK(Trig->ApplyTriggerSF());
        }
        return StatusCode::SUCCESS;
    }
    void SUSYElectronSelector::DecorateTotalElectronSF(const xAOD::Electron& el, const EleWeightDecorators& Decors) {
        Decors.EleSF->operator()(el) = Decors.EleSF_Reco->operator()(el) * Decors.EleSF_ID->operator()(el) * Decors.EleSF_Isol->operator()(el);
    }
    StatusCode SUSYElectronSelector::SaveElectronSF(const xAOD::Electron& el, EleEffToolHandle &SFtool, SG::AuxElement::Decorator<double>* SFdecor, double &TotSF, bool doSF) {
        double SF = 1.;
        if (doSF && SFtool->getEfficiencyScaleFactor(el, SF) == CP::CorrectionCode::Error) {
            return StatusCode::FAILURE;
        }
        TotSF *= SF;
        SFdecor->operator()(el) = SF;
        return StatusCode::SUCCESS;

    }
    StatusCode SUSYElectronSelector::initializeTriggerSFTools() {
        LoadSFToolFromSUSYTools(SUSYToolsPtr()->TrigSingleLep(), "ElectronEfficiencyCorrectionTool_trig_singleLep");
        LoadSFToolFromSUSYTools(SUSYToolsPtr()->TrigDiLep(), "ElectronEfficiencyCorrectionTool_trig_diLep");
        LoadSFToolFromSUSYTools(SUSYToolsPtr()->TrigMixLep(), "ElectronEfficiencyCorrectionTool_trig_mixLep");

        m_TriggerSFConf.push_back(SUSYToolsPtr()->TrigSingleLep());
        m_TriggerSFConf.push_back(SUSYToolsPtr()->TrigDiLep());
        m_TriggerSFConf.push_back(SUSYToolsPtr()->TrigMixLep());

        std::string triggerEleIso = TString(*SUSYToolsPtr()->getProperty < std::string > ("EleIso")).ReplaceAll("GradientLoose", "Gradient").ReplaceAll("FixedCutTight", "Gradient").Data();
        std::string eleId = TString(*SUSYToolsPtr()->getProperty < std::string > ("EleId")).ReplaceAll("AndBLayer", "BLayer").ReplaceAll("LLH", "").Data();

        for (auto& SF : m_TriggerSFConf) {
            if (m_SFTools.find(SF) != m_SFTools.end()) {
                ATH_MSG_DEBUG("Configuration " << SF << " has already been created by SUSYTools");
                continue;
            }
            asg::AnaToolHandle<EleEffTool> NewTool("ElectronTriggerSF " + std::to_string(m_SFTools.size()));
            ASG_SET_ANA_TOOL_TYPE(NewTool, AsgElectronEfficiencyCorrectionTool);
            ATH_MSG_INFO("Setup new SF tool using " << SF << " as config");
            ATH_CHECK(NewTool.setProperty("IdKey", eleId));
            ATH_CHECK(NewTool.setProperty("IsoKey", triggerEleIso));
            ATH_CHECK(NewTool.setProperty("TriggerKey", SF));
            ATH_CHECK(NewTool.initialize());
            m_SFTools.insert(TrigSFTool(SF, NewTool.getHandle()));
            ToolHandleSystematics<EleEffTool>* SystHandle = new ToolHandleSystematics<EleEffTool>(NewTool);
            ATH_CHECK(SystHandle->initialize());
        }
        return StatusCode::SUCCESS;
    }
    void SUSYElectronSelector::LoadSFToolFromSUSYTools(const std::string &Key, const std::string& PropertyName) {
        if (m_SFTools.find(Key) != m_SFTools.end()) {
            ATH_MSG_WARNING("There is already a Trigger SF tool using " << Key);
            return;
        }
        m_SFTools.insert(TrigSFTool(Key, GetCPTool<EleEffTool>(PropertyName)));
    }
    std::string SUSYElectronSelector::FindBestSFTool(const std::string& TriggerSF) {
        std::string Config;

        std::vector<std::string> ExtractedTriggers = SUSYToolsPtr()->GetTriggerOR(TriggerSF);
        unsigned int GlobalMatches = 0;
        for (auto& SFconfig : m_SFTools) {
            std::vector<std::string> Tokens15, Tokens16;
            SUSYToolsPtr()->GetTriggerTokens(SFconfig.first, Tokens15, Tokens16);

            unsigned int Found = 0;
            for (auto& T : ExtractedTriggers) {
                if (IsInVector(T, Tokens16) || IsInVector(T, Tokens15)) ++Found;
            }
            if (Found > GlobalMatches) {
                if (!Config.empty()) ATH_MSG_DEBUG("Found already " << Config << " matching to trigger SF expression " << TriggerSF << ". Will replace it by " << SFconfig.first);
                Config = SFconfig.first;
                GlobalMatches = Found;
            }
        }
        if (!GlobalMatches) {
            if (TriggerSF.find("2e") == 0 || TriggerSF.find("_2e") != std::string::npos) {
                std::string DiTriggerSF = ReplaceExpInString(TriggerSF, "_2e", "_e");
                if (DiTriggerSF.find("2e") == 0) DiTriggerSF = DiTriggerSF.substr(1, std::string::npos);
                return FindBestSFTool(DiTriggerSF);
            }
        }
        return Config;
    }

    //#################################################################################################
    //                                  ElectronTriggerSFHandler
    //#################################################################################################
    ElectronTriggerSFHandler::ElectronTriggerSFHandler(const std::string &Trigger, const ToolHandle<ST::ISUSYObjDef_xAODTool> &SUSYTools, EleEffToolHandle& TriggerSF, XAMPP::EventInfo* Info) :
                m_TrigStr(Trigger),
                m_TriggerSFTool(TriggerSF),
                m_XAMPPInfo(Info),
                m_susytools(SUSYTools),
                m_Triggers(),
                m_TotTrigSF(NULL),
                m_TotSF(NULL),
                m_decTrigSF(NULL),
                m_decSF(NULL) {

    }
    StatusCode ElectronTriggerSFHandler::initialize(const EleWeightDecorators& Decors) {
        m_TotSF = Decors.TotalSF;
        m_decSF = Decors.EleSF;
        return initialize();
    }
    bool ElectronTriggerSFHandler::RetrieveMatchers(const std::string &Trigger) {
        if (!m_XAMPPInfo->DoesVariableExist<char>("Trig" + Trigger) || !m_XAMPPInfo->DoesVariableExist<MatchedEl>("TrM_el" + Trigger)) return false;
        m_Triggers.push_back(EleTrigMatcher(m_XAMPPInfo->GetVariableStorage<char>("Trig" + Trigger), m_XAMPPInfo->GetVariableStorage<MatchedEl>("TrM_el" + Trigger)));
        return true;
    }
    StatusCode ElectronTriggerSFHandler::initialize() {
        ST::SUSYObjDef_xAOD* ST = dynamic_cast<ST::SUSYObjDef_xAOD*>(m_susytools.operator->());

        std::vector<std::string> SingleT = ST->GetTriggerOR(m_TrigStr);
        for (auto& T : SingleT) {
            if (!RetrieveMatchers(T)) {
                if (T.find("L1EM10") != std::string::npos && !RetrieveMatchers(ReplaceExpInString(T, "L1EM10", "L12EM10"))) {
                    Warning("ElectronTriggerSFHandler()", "Could not find the trigger %s, but you want to apply a SF on it", T.c_str());
                }
            }
        }
        if (m_Triggers.empty()) {
            Error("ElectronTriggerSFHandler()", "None of the triggers you gave is part of the trigger selection");
            return StatusCode::FAILURE;
        }
        if (!m_TotTrigSF) {
            Error("ElectronTriggerSFHandler()", "No trigger SF storage found.");
            return StatusCode::FAILURE;
        }
        if (!m_decTrigSF) {
            Error("ElectronTriggerSFHandler()", "No trigger SF electron decorator found.");
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    StatusCode ElectronTriggerSFHandler::SaveTriggerSF(const xAOD::Electron* El) {
        //Check if the trigger has fired at all and if it is already matched
        double SF = 1.;
        if (IsTriggerMachted(El)) {
            if (m_TriggerSFTool->getEfficiencyScaleFactor(*El, SF) == CP::CorrectionCode::Error) {
                return StatusCode::FAILURE;
            }
        }
        m_decTrigSF->operator()(*El) = SF;
        if (m_decSF) m_decSF->operator()(*El) = m_decSF->operator()(*El) * SF;
        if (m_TotTrigSF->isAvailable()) SF *= m_TotTrigSF->GetValue();
        if (!m_TotTrigSF->Store(SF).isSuccess()) {
            Error("ElectronTriggerSFHandler", "Failed to save the total trigger SF %s", m_TrigStr.c_str());
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    ElectronTriggerSFHandler::~ElectronTriggerSFHandler() {
        if (m_decTrigSF) delete m_decTrigSF;
    }

    bool ElectronTriggerSFHandler::IsTriggerMachted(const xAOD::Electron* El) {
        for (auto& T : m_Triggers) {
            if (IsTriggerMachted(El, T)) return true;
        }
        return false;
    }
    bool ElectronTriggerSFHandler::IsTriggerMachted(const xAOD::Electron* El, ElectronTriggerSFHandler::EleTrigMatcher& M) {
        if (!M.Trigger->GetValue()) return false;
        for (const auto& MatchedEl : M.Matched->GetValue()) {
            if (*MatchedEl == El) return true;
        }
        return false;
    }

    StatusCode ElectronTriggerSFHandler::ApplyTriggerSF() {
        if (!m_TotTrigSF->isAvailable() && !m_TotTrigSF->Store(1.).isSuccess()) {
            return StatusCode::FAILURE;
        }
        if (m_TotSF) return m_TotSF->Store(m_TotSF->GetValue() * m_TotTrigSF->GetValue());
        return StatusCode::SUCCESS;
    }
    void ElectronTriggerSFHandler::SetTriggerStorage(XAMPP::Storage<double>* store) {
        if (store) m_TotTrigSF = store;
    }
    void ElectronTriggerSFHandler::SetTriggerDecorator(SG::AuxElement::Decorator<double>* decor) {
        if (decor) m_decTrigSF = decor;
    }

}
