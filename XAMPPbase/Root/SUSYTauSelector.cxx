#include <XAMPPbase/SUSYTauSelector.h>
#include <xAODTau/TauxAODHelpers.h>

#include <TauAnalysisTools/ITauEfficiencyCorrectionsTool.h>
#include <TauAnalysisTools/TauTruthMatchingTool.h>
#include <TauAnalysisTools/TauSelectionTool.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define SET_DUAL_TOOL( TOOLHANDLE, TOOLTYPE, TOOLNAME )                \
  ASG_SET_ANA_TOOL_TYPE(TOOLHANDLE, TOOLTYPE);                        \
  TOOLHANDLE.setName(TOOLNAME);

namespace XAMPP {

    static CharDecorator dec_passBaseID("baselineID");
    static CharDecorator dec_passSignalID("signalID");

    static CharAccessor acc_passBaseID("baselineID");
    static CharAccessor acc_passSignalID("signalID");

    SUSYTauSelector::SUSYTauSelector(std::string myname) :
                SUSYParticleSelector(myname),
                m_xAODTaus(nullptr),
                m_Taus(nullptr),
                m_TausAux(nullptr),
                m_PreTaus(nullptr),
                m_BaselineTaus(nullptr),
                m_SignalTaus(nullptr),
                m_xAODDiTaus(nullptr),
                m_DiTaus(nullptr),
                m_PreDiTaus(nullptr),
                m_BaselineDiTaus(nullptr),
                m_SignalDiTaus(nullptr),
                m_TruthMatching("TauTruthMatchingTool"),
                m_SF(),
                m_BaseTauSelectionTool(""),
                m_SignalTauSelectionTool(""),
                m_SeparateSF(false),
                m_doIdSF(true),
                m_doTrigSF(true),
                m_TriggerExp(""),
                m_minBaselineJetBDT(FLT_MIN),
                m_maxBaselineJetBDT(FLT_MAX),
                m_ignoreBaseTauIDTool(false) {
        declareProperty("SeparateSF", m_SeparateSF);
        declareProperty("ApplyIdSF", m_doIdSF);
        declareProperty("ApplyTriggerSF", m_doTrigSF);
        declareProperty("SFTrigger", m_TriggerExp);
        declareProperty("BaselineTauSelectionTool", m_BaseTauSelectionTool);
        declareProperty("SignalTauSelectionTool", m_SignalTauSelectionTool);

        //Jet BDT requirements
        declareProperty("MinBaselineJetBDT", m_minBaselineJetBDT);
        declareProperty("MaxBaselineHJetBDT", m_maxBaselineJetBDT);
        declareProperty("ignoreBaseSelectionTool", m_ignoreBaseTauIDTool);
        m_TruthMatching.declarePropertyFor(this, "TruthMatchingTool", "Tau truth matching tool");
        SetContainerKey("TauJets");
        SetObjectType(XAMPP::SelectionObject::Tau);

    }

    SUSYTauSelector::~SUSYTauSelector() {
        for (auto const& ScaleFactors : m_SF) {
            delete ScaleFactors.TauSF;
            delete ScaleFactors.TaudIdSF;
            delete ScaleFactors.TauTrigSF;

        }
    }
    TauLink SUSYTauSelector::GetLink(const xAOD::TauJet& tau) const {
        return TauLink(*m_Taus, tau.index());
    }
    TauLink SUSYTauSelector::GetOrigLink(const xAOD::TauJet& tau) const {
        const xAOD::TauJet* Otau = dynamic_cast<const xAOD::TauJet*>(xAOD::getOriginalObject(tau));
        return TauLink(*m_xAODTaus, Otau != nullptr ? Otau->index() : tau.index());
    }
    const xAOD::TauJetContainer* SUSYTauSelector::GetTauContainer() const {
        return m_xAODTaus;
    }
    xAOD::TauJetContainer* SUSYTauSelector::GetTaus() const {
        return m_Taus;
    }
    xAOD::TauJetContainer* SUSYTauSelector::GetPreTaus() const {
        return m_PreTaus;
    }
    xAOD::TauJetContainer* SUSYTauSelector::GetSignalTaus() const {
        return m_SignalTaus;
    }
    xAOD::TauJetContainer* SUSYTauSelector::GetBaselineTaus() const {
        return m_BaselineTaus;
    }
    const xAOD::DiTauJetContainer* SUSYTauSelector::GetDiTauContainer() const {
        return m_xAODDiTaus;
    }
    xAOD::DiTauJetContainer* SUSYTauSelector::GetDiTaus() const {
        return m_DiTaus;
    }
    xAOD::DiTauJetContainer* SUSYTauSelector::GetPreDiTaus() const {
        return m_PreDiTaus;
    }
    xAOD::DiTauJetContainer* SUSYTauSelector::GetSignalDiTaus() const {
        return m_SignalDiTaus;
    }
    xAOD::DiTauJetContainer* SUSYTauSelector::GetBaselineDiTaus() const {
        return m_BaselineDiTaus;
    }
    xAOD::DiTauJetContainer* SUSYTauSelector::GetCustomDiTaus(const std::string &kind) const {
        xAOD::DiTauJetContainer* customTaus = nullptr;
        if (LoadViewElementsContainer(kind, customTaus).isSuccess()) return customTaus;
        return m_BaselineDiTaus;
    }

    StatusCode SUSYTauSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(SUSYParticleSelector::initialize());
        if (!ProcessObject()) {
            return StatusCode::SUCCESS;
        }

        if (!m_BaseTauSelectionTool.isSet()) {
            m_BaseTauSelectionTool = GetCPTool<TauAnalysisTools::ITauSelectionTool>("TauSelectionToolBaseline");
        }
        if (!m_SignalTauSelectionTool.isSet()) {
            m_SignalTauSelectionTool = GetCPTool<TauAnalysisTools::ITauSelectionTool>("TauSelectionTool");
        }
        if (!m_TruthMatching.isUserConfigured()) {
            SET_DUAL_TOOL(m_TruthMatching, TauAnalysisTools::TauTruthMatchingTool, "TauTruthMatching")
        }
        ATH_CHECK(m_TruthMatching.retrieve());
        ATH_CHECK(m_SignalTauSelectionTool.retrieve());
        ATH_CHECK(m_BaseTauSelectionTool.retrieve());

        ToolHandle < TauAnalysisTools::ITauEfficiencyCorrectionsTool > IdTool = GetCPTool<TauAnalysisTools::ITauEfficiencyCorrectionsTool>("TauEfficiencyCorrectionsTool");
        ToolHandle < TauAnalysisTools::ITauEfficiencyCorrectionsTool > TrigTool = GetCPTool<TauAnalysisTools::ITauEfficiencyCorrectionsTool>("TauTrigEfficiencyCorrectionsTool0");
        //Disable separation of SF if the trigger or id is not stored
        if (m_SeparateSF && ((m_doTrigSF && !m_doIdSF) || (!m_doTrigSF && m_doIdSF))) {
            m_SeparateSF = false;
        }
        for (const auto set : m_systematics->GetWeightSystematics(ObjectType())) {
            TauWeightDecorators Decors;
            Decors.Systematic = set;
            bool WriteId = m_doIdSF && XAMPP::ToolIsAffectedBySystematic(IdTool, set);
            bool WriteTrig = m_doTrigSF && XAMPP::ToolIsAffectedBySystematic(TrigTool, set);
            ATH_CHECK(InitSFdecorator("", WriteId || WriteTrig, set, Decors.TauSF, Decors.TotalSF));
            ATH_CHECK(InitSFdecorator("Trig", m_SeparateSF && WriteTrig, set, Decors.TauTrigSF, Decors.TrigSF));
            ATH_CHECK(InitSFdecorator("ID", m_SeparateSF && WriteId, set, Decors.TaudIdSF, Decors.IDSF));

            m_SF.push_back(Decors);
        }
        return StatusCode::SUCCESS;
    }

    StatusCode SUSYTauSelector::LoadContainers() {
        if (!ProcessObject()) {
            return StatusCode::SUCCESS;
        }
        for (auto& ScaleFactors : m_SF) {
            ATH_CHECK(ScaleFactors.TotalSF->ConstStore(1.));
        }
        ATH_CHECK(LoadContainer(ContainerKey(), m_xAODTaus));
        if (ProcessObject(XAMPP::SelectionObject::DiTau)) {
            ATH_CHECK(LoadContainer("DiTauJets", m_xAODDiTaus));
        }
        return StatusCode::SUCCESS;
    }

    xAOD::TauJetContainer* SUSYTauSelector::GetCustomTaus(const std::string &kind) const {
        xAOD::TauJetContainer* customTaus = nullptr;
        if (LoadViewElementsContainer(kind, customTaus).isSuccess()) return customTaus;
        return m_BaselineTaus;

    }

    StatusCode SUSYTauSelector::CallSUSYTools() {
        ATH_MSG_DEBUG("Calling SUSYTauSelector::CallSUSYTools()..");
        ATH_CHECK(m_susytools->GetTaus(m_Taus, m_TausAux, false));

        for (auto tau : *m_Taus) {
            dec_passBaseID(*tau) = m_BaseTauSelectionTool->accept(*tau);
            SetPreSelectionDecorator(*tau, m_ignoreBaseTauIDTool || acc_passBaseID(*tau));
            dec_passSignalID(*tau) = m_SignalTauSelectionTool->accept(*tau);
            SetSignalDecorator(*tau, acc_passSignalID(*tau));
        }
        return StatusCode::SUCCESS;
    }

    StatusCode SUSYTauSelector::InitialFill(const CP::SystematicSet &systset) {
        SetSystematics(systset);
        if (ProcessObject()) ATH_CHECK(FillFromSUSYTools(m_Taus, m_TausAux, m_PreTaus));
        else {
            ATH_CHECK(ViewElementsContainer("cont", m_Taus));
            ATH_CHECK(ViewElementsContainer("presel", m_PreTaus));
        }
        if (!ProcessObject(XAMPP::SelectionObject::DiTau)) {
            ATH_CHECK(ViewElementsContainer("contDiTaus", m_DiTaus));
            ATH_CHECK(ViewElementsContainer("preselDiTaus", m_PreDiTaus));
        }
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTauSelector::FillTaus(const CP::SystematicSet &systset) {

        SetSystematics(systset);
        ATH_CHECK(ViewElementsContainer("baseline", m_BaselineTaus));
        ATH_CHECK(ViewElementsContainer("signal", m_SignalTaus));
        static IntDecorator dec_NTrks("NTrks");
        for (const auto& itau : *m_PreTaus) {
            dec_NTrks(*itau) = itau->nTracks();
            if (PassBaseline(*itau)) m_BaselineTaus->push_back(itau);
            if (PassSignal(*itau)) m_SignalTaus->push_back(itau);
        }

        ATH_MSG_DEBUG("Number of all taus: " << m_Taus->size());
        ATH_MSG_DEBUG("Number of preselected taus: " << m_PreTaus->size());
        ATH_MSG_DEBUG("Number of selected baseline taus: " << m_BaselineTaus->size());
        ATH_MSG_DEBUG("Number of selected signal taus: " << m_SignalTaus->size());
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTauSelector::SaveScaleFactor() {
        if (m_SF.empty()) return StatusCode::SUCCESS;
        const CP::SystematicSet* kineSet = m_systematics->GetCurrent();
        ATH_MSG_DEBUG("Save SF of " << m_SignalTaus->size() << " taus");
        for (auto const& ScaleFactors : m_SF) {
            if (kineSet != m_systematics->GetNominal() && ScaleFactors.Systematic != m_systematics->GetNominal()) continue;
            ATH_CHECK(m_systematics->setSystematic(ScaleFactors.Systematic));
            double TotIdSF(1.), TotTrigSF(1.);
            for (const auto& itau : *m_SignalTaus) {
                // double GetSignalTauSF(const xAOD::TauJet& tau, const bool idSF = true, const bool triggerSF = true, const std::string& trigExpr = "tau25_medium1_tracktwo")
                double IdSF = m_doIdSF ? m_susytools->GetSignalTauSF(*itau, true, false) : 1.;
                double TrigSF = m_doTrigSF ? m_susytools->GetSignalTauSF(*itau, false, true, m_TriggerExp) : 1.;
                if (m_doIdSF) ScaleFactors.TaudIdSF->operator()(*itau) = IdSF;
                if (m_doTrigSF) ScaleFactors.TauTrigSF->operator()(*itau) = TrigSF;
                ScaleFactors.TauSF->operator()(*itau) = IdSF * TrigSF;
                TotIdSF *= IdSF;
                TotTrigSF *= TrigSF;
            }
            ATH_CHECK(ScaleFactors.TrigSF->Store(TotTrigSF));
            ATH_CHECK(ScaleFactors.IDSF->Store(TotIdSF));
            ATH_CHECK(ScaleFactors.TotalSF->Store(TotIdSF * TotTrigSF));
        }
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTauSelector::StoreTruthClassifer(xAOD::TauJet &tau) {
        if (!m_XAMPPInfo->GetOrigInfo()->eventType(xAOD::EventInfo::IS_SIMULATION)) return StatusCode::SUCCESS;
        //Store the Parton TruthLabelIDs first
        static IntDecorator dec_PartonID("PartonTruthLabelID");
        static IntDecorator dec_ConeID("ConeTruthLabelID");

        static SG::AuxElement::Decorator<ElementLink<xAOD::TruthParticleContainer>> dec_truthLink("truthParticleLink");
        static IntDecorator dec_truthType("truthType");
        static IntDecorator dec_truthOrigin("truthOrigin");
//        static IntDecorator dec_truthTauType("truthTauType");

        static SG::AuxElement::ConstAccessor<ElementLink<xAOD::TruthParticleContainer>> acc_truthLink("truthParticleLink");
        static IntAccessor acc_truthType("truthType");
        static IntAccessor acc_truthOrigin("truthOrigin");
        static UIntAccessor acc_classifierOrg("classifierParticleOrigin");
        static UIntAccessor acc_classifierTyp("classifierParticleType");
        static IntAccessor acc_ConeID("ConeTruthLabelID");
        static IntAccessor acc_PartonID("PartonTruthLabelID");
        m_TruthMatching->applyTruthMatch(tau);
        const xAOD::Jet* Jet = xAOD::TauHelpers::getLink < xAOD::Jet > (&tau, "truthJetLink");
        int PartonID = -1;
        int ConeID = -1;
        if (Jet) {
            if (acc_PartonID.isAvailable(*Jet)) PartonID = acc_PartonID(*Jet);
            if (acc_ConeID.isAvailable(*Jet)) ConeID = acc_ConeID(*Jet);
        }
        dec_ConeID(tau) = ConeID;
        dec_PartonID(tau) = PartonID;
        //Then do the TruthClassification
        const xAOD::TruthParticle* TruthTau = m_TruthMatching->getTruth(tau);
//        dec_truthTauType(tau) = m_TruthMatching->getTruthParticleType(tau);
        bool ClassificationFromTruth = false;
        if (TruthTau) {
            if (acc_classifierOrg.isAvailable(*TruthTau) && acc_classifierTyp.isAvailable(*TruthTau)) {
                dec_truthType(tau) = acc_classifierTyp(*TruthTau);
                dec_truthOrigin(tau) = acc_classifierOrg(*TruthTau);
                ClassificationFromTruth = true;
            }
        }
        auto tau_trk = tau.track(0);

        if (!tau_trk) {
            ATH_MSG_ERROR("No track found for the tau");
            return StatusCode::FAILURE;
        }
        //If there is no TruthClassification available obtain the origin from the associated truthParticle

        if (ClassificationFromTruth) return StatusCode::SUCCESS;
        if (acc_truthType.isAvailable(*tau_trk)) dec_truthType(tau) = acc_truthType(*tau_trk);
        if (acc_truthOrigin.isAvailable(*tau_trk)) dec_truthOrigin(tau) = acc_truthOrigin(*tau_trk);

        return StatusCode::SUCCESS;
    }

}
