#include <XAMPPbase/ISystematics.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/ParticleSelector.h>
#include <PATInterfaces/SystematicSet.h>

//for jet reclustering
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetSplitter.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/JetDumper.h"
#include "JetRec/JetToolRunner.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
#include <xAODJet/JetContainer.h>
#include <xAODJet/JetAuxContainer.h>

namespace XAMPP {
    ParticleSelector::ParticleSelector(std::string myname) :
                AsgTool(myname),
#ifdef XAOD_STANDALONE
                m_output_level(MSG::INFO),
#endif
                m_baselinePt(0.),
                m_baselineEta(-1.),
                m_signalPt(0.),
                m_signalEta(-1.),
                m_ActSys(nullptr),
                m_EvInfoHandle("EventInfoHandler"),
                m_RelevantSys(),
                m_ContainerKey(),
                m_storeName(),
                m_ObjectType(XAMPP::SelectionObject::Other),
                m_init(false),
                m_PreSelDecorName("baseline"),
                m_acc_presel(),
                m_dec_presel(),
                m_BaselineDecorName("passOR"),
                m_acc_baseline(),
                m_dec_baseline(),
                m_SignalDecorName("signal"),
                m_acc_signal(),
                m_dec_signal(),
                m_IsolDecorName("isol"),
                m_acc_isol(),
                m_dec_isol(),
                m_systematics("SystematicsTool"),
                m_XAMPPInfo(0) {
#ifdef XAOD_STANDALONE
        declareProperty("OutputLevel", m_output_level);
#endif

        declareProperty("PreSelectionDecorator", m_PreSelDecorName);
        declareProperty("IsolationDecorator", m_IsolDecorName);

        declareProperty("BaselineDecorator", m_BaselineDecorName);
        declareProperty("SignalDecorator", m_SignalDecorName);

        declareProperty("BaselinePtCut", m_baselinePt = 0.);
        declareProperty("BaselineEtaCut", m_baselineEta = -1.);
        declareProperty("SignalPtCut", m_signalPt = 0.);
        declareProperty("SignalEtaCut", m_signalEta = -1.);
        declareProperty("SystematicsTool", m_systematics);

        m_EvInfoHandle.declarePropertyFor(this, "EventInfoHandler", "The XAMPP EventInfo handle");

    }

    ParticleSelector::~ParticleSelector() {
        ATH_MSG_DEBUG("Destructor called");
    }
    StatusCode ParticleSelector::CreateAuxElements(std::string &name, SelectionAccessor& acc, SelectionDecorator& dec) {
        if (name.empty()) {
            ATH_MSG_ERROR("You cannot have an empty decorator name");
            return StatusCode::FAILURE;
        }
        acc = SelectionAccessor(new CharAccessor(name));
        dec = SelectionDecorator(new CharDecorator(name));
        name.clear();
        return StatusCode::SUCCESS;
    }
    XAMPP::SelectionObject ParticleSelector::ObjectType() const {
        return m_ObjectType;
    }
    bool ParticleSelector::ProcessObject(XAMPP::SelectionObject T) const {
        return m_systematics->ProcessObject(T);
    }
    bool ParticleSelector::ProcessObject() const {
        return ProcessObject(ObjectType());
    }
    StatusCode ParticleSelector::initialize() {
        ATH_MSG_INFO("Initialising...");
        if (isInitialized()) {
            ATH_MSG_WARNING("The tool is already initialized");
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(m_systematics.retrieve());
        if (!m_systematics->SystematicsFixed()) {
            ATH_MSG_ERROR("Not all systematics are initialized yet");
            return StatusCode::SUCCESS;
        }
        if (m_ObjectType != XAMPP::SelectionObject::Other) {
            m_RelevantSys = m_systematics->GetKinematicSystematics(m_ObjectType);
        }
        if (m_RelevantSys.empty()) {
            ATH_MSG_ERROR("Not even the nominal systematic set was given...");
            return StatusCode::FAILURE;
        }

        ATH_CHECK(CreateAuxElements(m_PreSelDecorName, m_acc_presel, m_dec_presel));
        ATH_CHECK(CreateAuxElements(m_BaselineDecorName, m_acc_baseline, m_dec_baseline));

        ATH_CHECK(CreateAuxElements(m_SignalDecorName, m_acc_signal, m_dec_signal));
        ATH_CHECK(CreateAuxElements(m_IsolDecorName, m_acc_isol, m_dec_isol));

        ATH_CHECK(m_EvInfoHandle.retrieve());
        m_XAMPPInfo = dynamic_cast<XAMPP::EventInfo*>(m_EvInfoHandle.operator->());

#ifdef XAOD_STANDALONE
        this->msg().setLevel(XAMPP::setOutputLevel(m_output_level));
#endif
        m_init = true;
        return StatusCode::SUCCESS;
    }
    bool ParticleSelector::isInitialized() const {
        return m_init;
    }
    void ParticleSelector::SetSystematics(const CP::SystematicSet *Set) {
        if (!m_XAMPPInfo->SetSystematic(Set).isSuccess()) ATH_MSG_WARNING("Could not set the systematic " << Set->name() << " to EventInfo");
        if (m_ActSys == Set) return;
        m_ActSys = Set;
        m_storeName = name() + SystName();
    }
    void ParticleSelector::SetContainerKey(const std::string &Key) {
        if (Key.empty() || !m_ContainerKey.empty()) {
            ATH_MSG_ERROR("Could not update the container key '" << m_ContainerKey << "' to '" << Key << "'");
            return;
        }
        m_ContainerKey = Key;
    }
    void ParticleSelector::SetObjectType(SelectionObject Type) {
        if (Type == SelectionObject::Other || m_ObjectType != SelectionObject::Other) {
            ATH_MSG_ERROR("Could not set the selection type of the selector");
        } else {
            m_ObjectType = Type;
        }
    }
    const std::string& ParticleSelector::StoreName() const {
        return m_storeName;
    }
    const std::string& ParticleSelector::ContainerKey() const {
        return m_ContainerKey;
    }
    std::string ParticleSelector::SystName(bool InclUnderScore) const {
        if (!m_ActSys) {
            ATH_MSG_WARNING("No current systematic defined");
            return "";
        }
        if (m_ActSys->name().empty() || !InclUnderScore) {
            return m_ActSys->name();
        }
        return "_" + m_ActSys->name();
    }

    void ParticleSelector::SetSystematics(const CP::SystematicSet & Set) {
        SetSystematics(&Set);
    }
    bool ParticleSelector::SystematicAffects(const CP::SystematicSet* Set) const {
        for (const auto& iSet : m_RelevantSys)
            if (iSet == Set) return true;
        return false;
    }
    bool ParticleSelector::SystematicAffects(const CP::SystematicSet &Set) const {
        return SystematicAffects(&Set);
    }
    bool ParticleSelector::PassBaselineKinematics(const xAOD::IParticle* P) const {
        return PassBaselineKinematics(*P);
    }
    bool ParticleSelector::PassSignalKinematics(const xAOD::IParticle* P) const {
        return PassSignalKinematics(*P);
    }
    bool ParticleSelector::PassBaselineKinematics(const xAOD::IParticle &P) const {
        return (P.pt() > m_baselinePt && (m_baselineEta < 0. || fabs(P.eta()) < m_baselineEta));
    }
    bool ParticleSelector::PassSignalKinematics(const xAOD::IParticle &P) const {
        return (P.pt() > m_signalPt && (m_signalEta < 0. || fabs(P.eta()) < m_signalEta));
    }
    bool ParticleSelector::PassPreSelection(const xAOD::IParticle &P) const {
        if (!m_acc_presel->isAvailable(P)) {
            ATH_MSG_ERROR("Preselection decoration is not set");
            PromptParticle(P);
            return false;
        }
        return PassBaselineKinematics(P) && m_acc_presel->operator()(P);
    }
    bool ParticleSelector::PassPreSelection(const xAOD::IParticle* P) const {
        return PassPreSelection(*P);
    }
    bool ParticleSelector::PassIsolation(const xAOD::IParticle &P) const {
        return m_acc_isol->operator()(P);
    }
    bool ParticleSelector::PassIsolation(const xAOD::IParticle* P) const {
        return PassIsolation(*P);
    }
    bool ParticleSelector::PassBaseline(const xAOD::IParticle &P) const {
        if (!m_acc_baseline->isAvailable(P)) {
            ATH_MSG_ERROR("Baseline decoration is not set");
            PromptParticle(P);
            return false;
        }
        return m_acc_baseline->operator()(P) && PassPreSelection(P);
    }
    bool ParticleSelector::PassBaseline(const xAOD::IParticle* P) const {
        return PassBaseline(*P);
    }
    bool ParticleSelector::PassSignal(const xAOD::IParticle &P) const {
        if (!m_acc_signal->isAvailable(P)) {
            ATH_MSG_ERROR("Signal decoration is not set");
            PromptParticle(P);
            return false;
        }
        return m_acc_signal->operator()(P) && PassBaseline(P) && PassSignalKinematics(P);
    }
    bool ParticleSelector::PassSignal(const xAOD::IParticle* P) const {
        return PassSignal(*P);
    }
    void ParticleSelector::SetPreSelectionDecorator(const xAOD::IParticle& P, bool Pass) const {
        m_dec_presel->operator()(P) = Pass;
    }
    void ParticleSelector::SetBaselineDecorator(const xAOD::IParticle& P, bool Pass) const {
        m_dec_baseline->operator()(P) = Pass;
    }
    void ParticleSelector::SetSignalDecorator(const xAOD::IParticle& P, bool Pass) const {
        m_dec_signal->operator()(P) = Pass;
    }
    void ParticleSelector::SetSelectionDecorators(const xAOD::IParticle&P, bool Pass) const {
        SetSignalDecorator(P, Pass);
        SetPreSelectionDecorator(P, Pass);
        SetBaselineDecorator(P, Pass);
    }

    StatusCode ParticleSelector::ReclusterJets(const xAOD::IParticleContainer *inputJets, double Rcone, double minPtKt4, std::string PreFix) {
        if (m_systematics->AffectsOnlyMET(m_systematics->GetCurrent())) return StatusCode::SUCCESS;
        ATH_MSG_DEBUG("Starting Jet Reclustering with Rcone = " << Rcone);
        static SG::AuxElement::Decorator<int> dec_constituents("constituents");
        xAOD::JetContainer* FatJets = new xAOD::JetContainer();
        xAOD::JetAuxContainer* FatJetsAux = new xAOD::JetAuxContainer();
        FatJets->setStore(FatJetsAux);

        if (minPtKt4 == -1.) minPtKt4 = m_signalPt; // using signal pt cut if not set from outside

        std::vector<fastjet::PseudoJet> v_pseudoJets;
        for (const auto& ijet : *inputJets) {
            if (ijet->p4().Pt() < minPtKt4) continue;
            fastjet::PseudoJet myjet(ijet->p4().Px(), ijet->p4().Py(), ijet->p4().Pz(), ijet->p4().E());
            v_pseudoJets.push_back(myjet);
        }
        ATH_MSG_DEBUG("Vector of pseudo jets filled ");

        fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, Rcone);
        ATH_MSG_DEBUG("Jet Definition set ");

        // Execute the clustering algorithm and retrieve the output
        fastjet::ClusterSequence cs(v_pseudoJets, jet_def);
        ATH_MSG_DEBUG("Cluster sequence done");
        std::vector<fastjet::PseudoJet> cs_result = fastjet::sorted_by_pt(cs.inclusive_jets());
        ATH_MSG_DEBUG("Vector of pseudojets done and sorted");

        for (fastjet::PseudoJet& pj : cs_result) {
            xAOD::Jet* myJet = new xAOD::Jet();
            FatJets->push_back(myJet);
            xAOD::JetFourMom_t FourVec;
            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> >::Scalar E = pj.e();
            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> >::Scalar Px = pj.px();
            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> >::Scalar Py = pj.py();
            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> >::Scalar Pz = pj.pz();
            FourVec.SetPxPyPzE(Px, Py, Pz, E);
            myJet->setJetP4(FourVec);
            dec_constituents(*myJet) = pj.constituents().size();
        }
        v_pseudoJets.clear();
        cs_result.clear();

        // set the name of the FatJet container in order to access them via GetCustomJets(std::string kind)
        std::string store = Form("%s%.1f%s", (PreFix + "FatJet").c_str(), Rcone, StoreName().c_str());
        ATH_CHECK(evtStore()->record(FatJets, store));
        ATH_CHECK(evtStore()->record(FatJetsAux, store + "Aux."));

        ATH_MSG_DEBUG("JetReclustering done with PtThreshold [MeV] = " << minPtKt4 << " and R = " << Rcone);
        return StatusCode::SUCCESS;
    }
}
