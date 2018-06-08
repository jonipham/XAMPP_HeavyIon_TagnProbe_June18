#include <XAMPPbase/Cuts.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/HistoBase.h>
#include <XAMPPbase/HistoHelpers.h>

#include <PATInterfaces/SystematicSet.h>
#include <XAMPPbase/AnalysisConfig.h>

#include <vector>
#include <iostream>
#include "TMath.h"

#define GeVToMeV 1000.0

namespace XAMPP {

    CutFlow::CutFlow() :
                    m_AnaCuts(),
                    m_Name(""),
                    m_Hash(-1) {

    }
    CutFlow::CutFlow(const std::string &name) :
                    CutFlow() {
        setName(name);
    }
    CutFlow::~CutFlow() {
        m_AnaCuts.clear();
        m_Name.clear();
    }

    void CutFlow::push_back(Cut* cut) {
        if (cut) m_AnaCuts.push_back(cut);
    }
    int CutFlow::hash() const {
        return m_Hash;
    }

    const std::string& CutFlow::name() const {
        return m_Name;
    }
    void CutFlow::setName(const std::string &N) {
        if (N.empty()) return;
        m_Name = N;
        m_Hash = std::hash<std::string>()(N);
    }
    CutFlow& CutFlow::operator=(const CutFlow &cutflow) {
        // self-assignment guard
        if (this == &cutflow) return *this;

        // otherwise do the copy
        m_AnaCuts.reserve(cutflow.m_AnaCuts.size());
        std::copy(cutflow.m_AnaCuts.begin(), cutflow.m_AnaCuts.end(), std::back_inserter(m_AnaCuts));
        m_Name = cutflow.m_Name;
        m_Hash = cutflow.m_Hash;
        // return the existing object this operator can be chained
        return *this;
    }
    const CutRow& CutFlow::GetCuts() const {
        return m_AnaCuts;
    }
    AnalysisConfig::AnalysisConfig(std::string Analysis) :
                    AsgTool(Analysis),
                    m_XAMPPInfo(NULL),
#ifdef XAOD_STANDALONE
                    m_treeName(Analysis),
                    m_output_level(MSG::INFO),
#else
                    m_treeName("XAMPPTree"),
#endif
                    m_ActiveCutflows(),
                    m_StandardCuts(),
                    m_CutFlows(),
                    m_DefinedCuts(),
                    m_CutFlowHistos(),
                    m_init(false),
                    m_InfoHandle("EventInfoHandler"),
                    m_systematics("SystematicsTool") {
#ifdef XAOD_STANDALONE
        declareProperty("OutputLevel", m_output_level);
        declareProperty("ActiveCutflowsString", m_ActiveCutflowsString);
#endif
        m_InfoHandle.declarePropertyFor(this, "EventInfoHandler", "The XAMPPInfo Handler");
        declareProperty("TreeName", m_treeName);
        declareProperty("ActiveCutflows", m_ActiveCutflows);
        declareProperty("SystematicsTool", m_systematics);
    }
    StatusCode AnalysisConfig::initialize() {
        if (m_init) {
            return StatusCode::SUCCESS;
        }

        ATH_CHECK(m_InfoHandle.retrieve());
        m_XAMPPInfo = dynamic_cast<XAMPP::EventInfo*>(m_InfoHandle.operator->());
#ifdef XAOD_STANDALONE
        XAMPP::FillVectorFromString(m_ActiveCutflows, m_ActiveCutflowsString);
#endif

        ATH_CHECK(initializeStandardCuts());
        ATH_CHECK(initializeCustomCuts());
#ifdef XAOD_STANDALONE
        this->msg().setLevel(XAMPP::setOutputLevel(m_output_level));
#endif
        if (!m_CutFlows.size()) {
            ATH_MSG_ERROR("No CutFlow to run on was selected! Exiting...");
            return StatusCode::FAILURE;
        }
        m_init = true;
        return StatusCode::SUCCESS;
    }
    StatusCode AnalysisConfig::initializeStandardCuts() {
        Cut* cut = NewSkimmingCut("PassGRL", Cut::CutType::CutChar);
        if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<char>("PassGRL"), 1, Cut::Relation::Equal)) return StatusCode::FAILURE;
        m_StandardCuts.push_back(cut);
        cut = NewSkimmingCut("passLArTile", Cut::CutType::CutChar);
        if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<char>("passLArTile"), 1, Cut::Relation::Equal)) return StatusCode::FAILURE;
        m_StandardCuts.push_back(cut);
        cut = NewSkimmingCut("Trigger", Cut::CutType::CutChar);
        if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<char>("Trigger"), 1, Cut::Relation::Equal)) return StatusCode::FAILURE;
        m_StandardCuts.push_back(cut);
        cut = NewSkimmingCut("HasVtx", Cut::CutType::CutChar);
        if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<char>("HasVtx"), 1, Cut::Relation::Equal)) return StatusCode::FAILURE;
        m_StandardCuts.push_back(cut);
        //cut = NewSkimmingCut("BadJet", Cut::CutType::CutInt);
        //if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<int>("BadJet"), 0, Cut::Relation::Equal)) return StatusCode::FAILURE;
        //m_StandardCuts.push_back(cut);
        cut = NewSkimmingCut("CosmicMuon", Cut::CutType::CutInt);
        if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<int>("CosmicMuon"), 0, Cut::Relation::Equal)) return StatusCode::FAILURE;
        m_StandardCuts.push_back(cut);
        cut = NewSkimmingCut("BadMuon", Cut::CutType::CutInt);
        if (!cut->initialize(m_XAMPPInfo->GetVariableStorage<int>("BadMuon"), 0, Cut::Relation::Equal)) return StatusCode::FAILURE;
        m_StandardCuts.push_back(cut);

        return StatusCode::SUCCESS;
    }
    StatusCode AnalysisConfig::initializeCustomCuts() {
        CutFlow MyCutFlow("MyCutFlow"); // Lets give our cutflow a proper name.. Be creative

        //For your further cut flow please overload your initalize function and append the cuts to the vector m_CutFlows
        Cut* cut = NewSkimmingCut(">= 1 lepton", Cut::CutType::CutInt); //Define a new cut which affects the dumping of the Ntuple
        //This is actually a second method to initialize your cut with  the name of the event variable and the relations
        // For int and chars >= relations are translated to > Value -1 for float the >= and <= are just < and > relations
        if (!cut->initialize("N_SignalLeptons", ">=1")) return StatusCode::FAILURE;
        //Now lets combine this Cut with another one
        Cut* cut2 = NewCutFlowCut("< 2 jets", Cut::CutType::CutInt); // This cut only affects the cutflow histogram
        if (!cut2->initialize("N_Jets", "<2")) return StatusCode::FAILURE;
        // >=1 lepton and <2 jets cuts are now combined into one cut with OR
        Cut* CombCut = cut2->combine(cut, Cut::Combine::OR);
        MyCutFlow.push_back(CombCut);

        //XAMPP also supports now the direct cut on scalar variables of the n-th particle
        // In order to do so create a new CutFlow/Skimming cut
        Cut* cut3 = NewCutFlowCut("Lep pt > 50 GeV", Cut::CutType::CutPartFloat);
        //There are CutPartFloat/CutPartInt and CutPartChar available
        //Now initialize the cut as follows <ContainerName> <Variable>[Idx]
        if (!cut3->initialize("Elec pt[0]", ">=50000")) return StatusCode::FAILURE;
        MyCutFlow.push_back(cut3);
        // normally the initialize function returns a pointer to itself, but since this cut is combined with another
        // cut it appends to that and returns a NULL pointer -> so you do not need to push back

        //Append the CutFlow if you have added ALL cuts to  the CutFlows....
        ATH_CHECK(AddToCutFlows(MyCutFlow));

        return StatusCode::SUCCESS;
    }

    IHistoVariable* AnalysisConfig::FindCutFlowHisto(const CP::SystematicSet* Set, const CutFlow* Flow) const {
        std::map<SystSelectionPair, IHistoVariable*>::const_iterator Itr = m_CutFlowHistos.find(SystSelectionPair(Set, Flow));
        if (Itr != m_CutFlowHistos.end()) {
            return Itr->second;
        }
        return NULL;
    }

    bool AnalysisConfig::PassStandardCuts(unsigned int &N) const {
        N = 1;
        for (auto & cut : m_StandardCuts) {
            if (!cut->ApplyCut(true)) {
                ATH_MSG_DEBUG("Cut " << cut->GetName() << " failed... return false");
                return false;
            }
            ++N;
        }
        return true;
    }

    bool AnalysisConfig::ApplyCuts(CutKind K) {
        ATH_MSG_DEBUG("ApplyCuts method called " << K);
        bool PassAtLeastOne = false;
        //Standard cuts are only relevant for the CutFlow application. Usually
        //these events are already skimmed during the loop
        unsigned int StandardBin = 1;
        bool PassStandard = K != CutKind::CutFlowHisto || PassStandardCuts(StandardBin);
        for (auto & Row : m_CutFlows) {
            unsigned int Bin = StandardBin;
            IHistoVariable* Histo = FindCutFlowHisto(m_XAMPPInfo->GetSystematic(), &Row);
            if (!PassStandard) {
                //Only for cutflow filling thats the case
                if (!Histo->FillBin(Bin)) {
                    return false;
                }
                continue;
            }
            bool PassRow = true;
            for (auto&cut : Row.GetCuts()) {
                if (!cut->ApplyCut(K == CutKind::CutFlowHisto)) {
                    ATH_MSG_DEBUG("CutFlow: " << Row.name() << " -- Cut " << cut->GetName() << " failed in event " << m_XAMPPInfo->eventNumber() << "... return false");
                    PassRow = false;
                    break;
                }
                ++Bin;
            }
            if (PassRow) ++ Bin;
            if (K == CutKind::CutFlowHisto && Histo) Histo->FillBin(Bin);
            if (PassRow) {
                ATH_MSG_DEBUG("CutFlow:" << Row.name() << "succeeded");
                if (K == CutKind::Output) {
                    ATH_MSG_DEBUG("Dump the event");
                    return true;
                } else {

                }
            }
            PassAtLeastOne |= PassRow;
        }
        ATH_MSG_DEBUG("The event passed the selection requirements " << (int) PassAtLeastOne);
        return PassAtLeastOne;
    }
    AnalysisConfig::~AnalysisConfig() {
        m_StandardCuts.clear();
        m_CutFlows.clear();

    }

    std::vector<std::string> AnalysisConfig::GetCutNames(int hash) const {
        std::vector<std::string> Names;
        for (const auto & cut : m_StandardCuts)
            Names.push_back(cut->GetName());
        for (const auto & row : m_CutFlows) {
            if (row.hash() != hash) continue;
            for (auto & cut : row.GetCuts())
                Names.push_back(cut->GetName());
        }
        return Names;
    }
    std::string AnalysisConfig::TreeName() const {
        return m_treeName;
    }

    bool AnalysisConfig::RegisterHistoBase(HistoBase* Base) {
        for (auto& CutFlow : m_CutFlows) {
            if (!Base->RegisterCutFlow(CutFlow)) {
                return false;
            }
            CutFlowHistoVariable* Hist = Base->CreateCutFlowHisto(CutFlow);
            if (!Hist) {
                ATH_MSG_DEBUG("Cutflow writting seems to be disabled ");
                continue;
            }
            if (!Hist->InitializeCutFlow(GetCutNames(CutFlow.hash()))) {
                return false;
            }
            m_CutFlowHistos.insert(std::pair<SystSelectionPair, IHistoVariable*>(SystSelectionPair(Base->SystSet(), &CutFlow), Hist));
        }
        return true;
    }
    StatusCode AnalysisConfig::AddToCutFlows(CutFlow& cf) {
        if (!isActive(cf)) {
            ATH_MSG_INFO("The current cutflow " << cf.name() << " is disabled. It will be skipped");
            return StatusCode::SUCCESS;
        }
        for (auto CutFlow : m_CutFlows) {
            if (CutFlow.name() == cf.name()) {
                ATH_MSG_ERROR("You have defined the current cutflow " << cf.name() << " twice");
                return StatusCode::FAILURE;
            }
        }
        for (auto Cut : cf.GetCuts())
            if (!Cut->IsInitialized()) return StatusCode::FAILURE;

        ATH_MSG_INFO("Adding CutFlow '" << cf.name() << "' with the following cuts:");
        m_CutFlows.push_back(cf);
        for (auto& cut : m_StandardCuts)
            ATH_MSG_INFO("- " << cut->GetName());
        for (auto& cut : cf.GetCuts()) {
            ATH_MSG_INFO("- " << cut->GetName());
            if (!IsInVector(cut, m_DefinedCuts)) {
                m_DefinedCuts.push_back(cut);
            }
        }
        return StatusCode::SUCCESS;
    }

    bool AnalysisConfig::isActive(CutFlow& cf) const {
        if (m_ActiveCutflows.empty()) return true;
        for (auto cfname : m_ActiveCutflows)
            if (cfname == cf.name()) return true;
        return false;
    }
    Cut* AnalysisConfig::NewCut(const std::string&Name, Cut::CutType T, bool IsSkimming) {
        Cut* C = new Cut(m_XAMPPInfo, Name, T, IsSkimming);
        m_DefinedCuts.push_back(C);
        return C;
    }
    Cut* AnalysisConfig::NewSkimmingCut(const std::string& Name, Cut::CutType T) {
        return NewCut(Name, T, true);
    }
    Cut* AnalysisConfig::NewCutFlowCut(const std::string& Name, Cut::CutType T) {
        return NewCut(Name, T, false);
    }
    unsigned int  AnalysisConfig::NumActiveCutFlows() const{
        return m_ActiveCutflows.size();
        }
    TruthAnalysisConfig::TruthAnalysisConfig(std::string Analysis) :
                    AnalysisConfig(Analysis) {
    }

    StatusCode TruthAnalysisConfig::initializeStandardCuts() {
        return StatusCode::SUCCESS;
    }
    bool AnalysisConfig::isData() const {
        return m_systematics->isData();
    }

}

