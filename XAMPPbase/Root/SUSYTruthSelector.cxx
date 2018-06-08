#include <SUSYTools/SUSYObjDef_xAOD.h>
#include <SUSYTools/SUSYCrossSection.h>

#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/IMetSelector.h>
#include <XAMPPbase/SUSYTruthSelector.h>
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

namespace XAMPP {
    SUSYTruthSelector::SUSYTruthSelector(std::string myname) :
                ParticleSelector(myname),
                m_BaselineMuPtCut(-1.),
                m_BaselineMuEtaCut(-1.),
                m_BaselineElePtCut(-1.),
                m_BaselineEleEtaCut(-1.),
                m_SignalMuPtCut(-1.),
                m_SignalMuEtaCut(-1.),
                m_SignalElePtCut(-1.),
                m_SignalEleEtaCut(-1.),
                m_BJetPtCut(-1.),
                m_BJetEtaCut(-1.),
                m_BaselineJetPtCut(-1.),
                m_BaselineJetEtaCut(-1.),
                m_SignalJetPtCut(-1.),
                m_SignalJetEtaCut(-1.),
                m_BaselinePhotonPtCut(-1.),
                m_BaselinePhotonEtaCut(-1.),
                m_SignalPhotonPtCut(-1.),
                m_SignalPhotonEtaCut(-1.),
                m_BaselineTauPtCut(-1),
                m_BaselineTauEtaCut(-1),
                m_SignalTauPtCut(-1),
                m_SignalTauEtaCut(-1),
                m_BaselineNeutrinoPtCut(-1),
                m_BaselineNeutrinoEtaCut(-1),
                m_SignalNeutrinoPtCut(-1),
                m_SignalNeutrinoEtaCut(-1),
                m_RequirePreselFromHardProc(false),
                m_RequireBaseFromHardProc(false),
                m_RequireSignalFromHardProc(false),
                m_xAODTruthJets(nullptr),
                m_xAODTruthParticles(nullptr),
                m_xAODTruthMet(nullptr),
                m_xAODTruthBSM(nullptr),
                m_xAODTruthBoson(nullptr),
                m_xAODTruthTop(nullptr),
                m_InitialParticles(nullptr),
                m_InitialJets(nullptr),
                m_InitialFatJets(nullptr),
                m_InitialElectrons(nullptr),
                m_InitialMuons(nullptr),
                m_InitialPhotons(nullptr),
                m_InitialTaus(nullptr),
                m_InitialNeutrinos(nullptr),
                m_PreJets(nullptr),
                m_BaselineJets(nullptr),
                m_SignalJets(nullptr),
                m_BJets(nullptr),
                m_LightJets(nullptr),
                m_Particles(nullptr),
                m_PreElectrons(nullptr),
                m_BaselineElectrons(nullptr),
                m_SignalElectrons(nullptr),
                m_PreMuons(nullptr),
                m_BaselineMuons(nullptr),
                m_SignalMuons(nullptr),
                m_PrePhotons(nullptr),
                m_BaselinePhotons(nullptr),
                m_SignalPhotons(nullptr),
                m_PreTaus(nullptr),
                m_BaselineTaus(nullptr),
                m_SignalTaus(nullptr),
                m_Neutrinos(nullptr),
                m_InitialStatePart(nullptr),
                m_doTruthJets(false),
                m_doTruthElectrons(false),
                m_doTruthMuons(false),
                m_doTruthTaus(false),
                m_doTruthPhotons(false),
                m_doTruthNeutrinos(false),
                m_doTruthParticles(false),
                m_isTRUTH3(false),
                m_ElectronKey(""),
                m_MuonKey(""),
                m_TauKey(""),
                m_PhotonKey(""),
                m_NeutrinoKey(""),
                m_BosonKey(""),
                m_BSMKey(""),
                m_TopKey("") {
        SetContainerKey("TruthParticles");
        SetObjectType(XAMPP::SelectionObject::TruthParticle);
        //Kinematic properties of the particles
        declareProperty("TruthBaselineMuPtCut", m_BaselineMuPtCut);
        declareProperty("TruthBaselineMuEtaCut", m_BaselineMuEtaCut);
        declareProperty("TruthSignalMuPtCut", m_SignalMuPtCut);
        declareProperty("TruthSignalMuEtaCut", m_SignalMuEtaCut);

        declareProperty("TruthBaselineElePtCut", m_BaselineElePtCut);
        declareProperty("TruthBaselineEleEtaCut", m_BaselineEleEtaCut);
        declareProperty("TruthSignalElePtCut", m_SignalElePtCut);
        declareProperty("TruthSignalEleEtaCut", m_SignalEleEtaCut);

        declareProperty("TruthBaselinePhPtCut", m_BaselinePhotonPtCut);
        declareProperty("TruthBaselinePhEtaCut", m_BaselinePhotonEtaCut);
        declareProperty("TruthSignalPhPhCut", m_SignalPhotonPtCut);
        declareProperty("TruthSignalPhEtaCut", m_SignalPhotonEtaCut);

        declareProperty("TruthBaselineTauPtCut", m_BaselineTauPtCut);
        declareProperty("TruthBaselineTauEtaCut", m_BaselineTauEtaCut);
        declareProperty("TruthSignalTauPtCut", m_SignalTauPtCut);
        declareProperty("TruthSignalTauEtaCut", m_SignalTauEtaCut);

        declareProperty("TruthBaselineNeutrinoPtCut", m_BaselineNeutrinoPtCut);
        declareProperty("TruthBaselineNeutrinoEtaCut", m_BaselineNeutrinoEtaCut);
        declareProperty("TruthSignalNeutrinoPtCut", m_SignalNeutrinoPtCut);
        declareProperty("TruthSignalNeutrinoEtaCut", m_SignalNeutrinoEtaCut);

        declareProperty("TruthBaselineJetPtCut", m_BaselineJetPtCut);
        declareProperty("TruthBaselineJetEtaCut", m_BaselineJetEtaCut);
        declareProperty("TruthSignalJetPtCut", m_SignalJetPtCut);
        declareProperty("TruthSignalJetEtaCut", m_SignalJetEtaCut);

        declareProperty("TruthBJetPtCut", m_BJetPtCut);
        declareProperty("TruthBJetEtaCut", m_BJetEtaCut);

        declareProperty("PreselectionHardProcess", m_RequirePreselFromHardProc);
        declareProperty("BaselineHardProcess", m_RequireBaseFromHardProc);
        declareProperty("SignalHardProcess", m_RequireSignalFromHardProc);

        //Run Options
        declareProperty("ElectronContainer", m_ElectronKey);
        declareProperty("MuonContainer", m_MuonKey);
        declareProperty("TauContainer", m_TauKey);
        declareProperty("PhotonContainer", m_PhotonKey);
        declareProperty("NeutrinoContainer", m_NeutrinoKey);
        declareProperty("BosonContainer", m_BosonKey);
        declareProperty("BSMContainer", m_BSMKey);
        declareProperty("TopContainer", m_TopKey);

        declareProperty("doTruthParticles", m_doTruthParticles);
        declareProperty("doJets", m_doTruthJets);
        declareProperty("isTRUTH3", m_isTRUTH3);

    }

    StatusCode SUSYTruthSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(ParticleSelector::initialize());
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<XAMPPmet>("TruthMET", true, false)); //TruthMET only in the nominal

        m_doTruthElectrons = !m_ElectronKey.empty();
        m_doTruthMuons = !m_MuonKey.empty();
        m_doTruthTaus = !m_TauKey.empty();
        m_doTruthPhotons = !m_PhotonKey.empty();
        m_doTruthNeutrinos = !m_NeutrinoKey.empty();
        m_doTruthTop = !m_TopKey.empty();
        m_doTruthBoson = !m_BosonKey.empty();
        m_doTruthSUSY = !m_BSMKey.empty();

        return StatusCode::SUCCESS;
    }

    StatusCode SUSYTruthSelector::LoadContainers() {

        ATH_CHECK(LoadContainer("MET_Truth", m_xAODTruthMet));
        if (m_doTruthParticles) ATH_CHECK(LoadContainer(ContainerKey(), m_xAODTruthParticles));
        if (m_doTruthJets) ATH_CHECK(LoadContainer("AntiKt4TruthJets", m_xAODTruthJets));
        //These containers may be available in the derivations
//        if (m_doTruthTop) ATH_CHECK(LoadContainer(m_TopKey, m_xAODTruthTop));
        if (!m_BSMKey.empty()) ATH_CHECK(LoadContainer(m_BSMKey, m_xAODTruthBSM));
        if (!m_BosonKey.empty()) ATH_CHECK(LoadContainer(m_BosonKey, m_xAODTruthBoson));

        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTruthSelector::InitialFill(const CP::SystematicSet &systset) {

        if (!SystematicAffects(systset)) return StatusCode::SUCCESS;
        SetSystematics(systset);
        static XAMPP::Storage<XAMPPmet>* s_MET = m_XAMPPInfo->GetVariableStorage<XAMPPmet>("TruthMET");
        xAOD::MissingET* MET = XAMPP::GetMET_obj("NonInt", const_cast<xAOD::MissingETContainer*>(m_xAODTruthMet));
        ATH_CHECK(s_MET->Store(MET));

        if (m_doTruthParticles && CreateContainerLinks(ContainerKey(), m_InitialParticles) == LinkStatus::Failed) return StatusCode::FAILURE;
        if (m_doTruthJets){
            if(CreateContainerLinks("AntiKt4TruthJets", m_InitialJets) == LinkStatus::Failed) return StatusCode::FAILURE;
        } else ATH_CHECK(ViewElementsContainer("PreKt4InitJets", m_InitialJets));

        ATH_CHECK(FillTruthParticleContainer());

        ATH_CHECK(ViewElementsContainer("preJets", m_PreJets));
        ATH_CHECK(ViewElementsContainer("baselineJets", m_BaselineJets));
        ATH_CHECK(ViewElementsContainer("signalTrJets", m_SignalJets));
        ATH_CHECK(ViewElementsContainer("bjetJets", m_BJets));
        ATH_CHECK(ViewElementsContainer("lightJets", m_LightJets));

        ATH_CHECK(ViewElementsContainer("preElec", m_PreElectrons));
        ATH_CHECK(ViewElementsContainer("baselineElec", m_BaselineElectrons));
        ATH_CHECK(ViewElementsContainer("signalElec", m_SignalElectrons));
        ATH_CHECK(ViewElementsContainer("preMuon", m_PreMuons));
        ATH_CHECK(ViewElementsContainer("baselineMuon", m_BaselineMuons));
        ATH_CHECK(ViewElementsContainer("signalMuon", m_SignalMuons));
        ATH_CHECK(ViewElementsContainer("prePhoton", m_PrePhotons));
        ATH_CHECK(ViewElementsContainer("baselinePhoton", m_BaselinePhotons));
        ATH_CHECK(ViewElementsContainer("signalPhoton", m_SignalPhotons));
        ATH_CHECK(ViewElementsContainer("preTau", m_PreTaus));
        ATH_CHECK(ViewElementsContainer("baselineTau", m_BaselineTaus));
        ATH_CHECK(ViewElementsContainer("signalTau", m_SignalTaus));
        ATH_CHECK(ViewElementsContainer("signalNeut", m_Neutrinos));

        InitContainer(m_InitialJets, m_PreJets);
        InitContainer(m_InitialElectrons, m_PreElectrons);
        InitContainer(m_InitialMuons, m_PreMuons);
        InitContainer(m_InitialPhotons, m_PrePhotons);
        InitContainer(m_InitialTaus, m_PreTaus);
        InitContainer(m_InitialNeutrinos, m_Neutrinos);

        return StatusCode::SUCCESS;
    }
    void SUSYTruthSelector::InitDecorators(const xAOD::TruthParticle* T, bool Pass) {
        static FloatDecorator dec_Charge("charge");
        dec_Charge(*T) = T->charge();
        SetSelectionDecorators(*T, Pass);
    }
    void SUSYTruthSelector::InitDecorators(const xAOD::Jet* J, bool Pass) {
        SetSelectionDecorators(*J, Pass);
    }

    bool SUSYTruthSelector::IsGenParticle(const xAOD::TruthParticle* particle) const {
        //The barcode requirement only selects particles generated by the generator NOT by GEANT4 https://svnweb.cern.ch/trac/atlasoff/browser/Generators/TruthUtils/trunk/TruthUtils/TruthParticleHelpers.h
        return (!particle || particle->isGenSpecific() || particle->barcode() >= 200000);
    }
    bool SUSYTruthSelector::ConsiderParticle(xAOD::TruthParticle* particle) {
        //final state particle
        if (IsGenParticle(particle)) return false;
        else if (particle->status() == 1) {
            if (particle->isLepton() || particle->isPhoton()) return true;
        } else if (particle->isHiggs()) return true;
        else if (isTrueSUSY(particle)) return true;
        else if (isTrueZ(particle)) return true;
        else if (isTrueW(particle)) return true;
        else if (isTrueTop(particle)) return true;
        else if (isTrueTau(particle)) return true;
        return false;
    }

    StatusCode SUSYTruthSelector::FillTruthParticleContainer() {
        //Sort everything out from the truth particle container
        ATH_CHECK(ViewElementsContainer("skimmedTruth", m_Particles));

        if (m_doTruthParticles) {
            ATH_CHECK(ViewElementsContainer("initalElec", m_InitialElectrons));
            ATH_CHECK(ViewElementsContainer("initalMuon", m_InitialMuons));
            ATH_CHECK(ViewElementsContainer("initalPhotons", m_InitialPhotons));
            ATH_CHECK(ViewElementsContainer("initalTaus", m_InitialTaus));
            ATH_CHECK(ViewElementsContainer("initalNeutrinos", m_InitialNeutrinos));
            ATH_CHECK(ViewElementsContainer("InitialState", m_InitialStatePart));
            for (const auto& particle : *m_InitialParticles) {
                if (ConsiderParticle(particle)) {
                    InitDecorators(particle, true);
                    m_Particles->push_back(particle);
                    if (particle->isElectron()) m_InitialElectrons->push_back(particle);
                    else if (particle->isMuon()) m_InitialMuons->push_back(particle);
                    else if (particle->isPhoton()) m_InitialPhotons->push_back(particle);
                    else if (particle->isNeutrino()) m_InitialNeutrinos->push_back(particle);
                    else if (particle->isTau()) m_InitialTaus->push_back(particle);
                    else m_InitialStatePart->push_back(particle);
                }
            }
        }
        //Okay we've no Truth Particles. Let's try the dedicated containers
        else {
            ATH_CHECK(RetrieveParticleContainer(m_InitialElectrons, m_doTruthElectrons, ElectronKey(), "initialElec"));
            ATH_CHECK(RetrieveParticleContainer(m_InitialMuons, m_doTruthMuons, MuonKey(), "initialMuon"));
            ATH_CHECK(RetrieveParticleContainer(m_InitialPhotons, m_doTruthPhotons, PhotonKey(), "initialPhoton"));
            ATH_CHECK(RetrieveParticleContainer(m_InitialTaus, m_doTruthTaus, TauKey(), "initialTaus"));
            ATH_CHECK(RetrieveParticleContainer(m_InitialNeutrinos, m_doTruthNeutrinos, NeutrinoKey(), "initalNeutrinos"));
            ATH_CHECK(RetrieveParticleContainer(m_InitialStatePart, false, BSMKey(), "InitialState"));
        }
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYTruthSelector::RetrieveParticleContainer(xAOD::TruthParticleContainer* &Particles, bool FromStoreGate, const std::string& GateKey, const std::string &ViewElementsKey) {
        if (FromStoreGate) {
            if (CreateContainerLinks(GateKey, Particles) == LinkStatus::Failed) return StatusCode::FAILURE;
            else return StatusCode::SUCCESS;
        } else ATH_CHECK(ViewElementsContainer(ViewElementsKey, Particles));

        return StatusCode::SUCCESS;
    }

    StatusCode SUSYTruthSelector::FillTruth(const CP::SystematicSet &systset) {

        if (!SystematicAffects(systset)) return StatusCode::SUCCESS;
        SetSystematics(systset);

        for (const auto& truj_it : *m_PreJets) {
            if (!PassBaseline(*truj_it)) continue;
            m_BaselineJets->push_back(truj_it);
            if (!PassSignal(*truj_it)) continue;
            m_SignalJets->push_back(truj_it);
            if (IsBJet(truj_it)) m_BJets->push_back(truj_it);
            else m_LightJets->push_back(truj_it);
        }

        FillBaselineContainer(m_PreElectrons, m_BaselineElectrons);
        FillBaselineContainer(m_PreMuons, m_BaselineMuons);
        FillBaselineContainer(m_PrePhotons, m_BaselinePhotons);
        FillBaselineContainer(m_PreTaus, m_BaselineTaus);

        FillSignalContainer(m_BaselineElectrons, m_SignalElectrons);
        FillSignalContainer(m_BaselineMuons, m_SignalMuons);
        FillSignalContainer(m_BaselinePhotons, m_SignalPhotons);
        FillSignalContainer(m_BaselineTaus, m_SignalTaus);
        return StatusCode::SUCCESS;
    }
    bool SUSYTruthSelector::IsBJet(const xAOD::IParticle* j) {
        static IntAccessor acc_HardonTruthLabel("HadronConeExclTruthLabelID");
        static IntAccessor acc_ConeTruthLabel("ConeTruthLabelID");
        static IntAccessor acc_PartonTruthLabel("PartonTruthLabelID");
        static IntAccessor acc_GhostBHadron("GhostBHadronsFinalCount");
        static IntAccessor acc_GhostCHadron("GhostCHadronsFinalCount");

        static CharDecorator dec_bjet("bjet");

        if (j->type() != xAOD::Type::ObjectType::Jet) {
            ATH_MSG_WARNING("The IsBJet function is meant for jets, return false!");
            return false;
        }
        int flavor = 0;
        if (acc_HardonTruthLabel.isAvailable(*j)) flavor = acc_HardonTruthLabel(*j);
        else if (acc_ConeTruthLabel.isAvailable(*j)) flavor = acc_ConeTruthLabel(*j);
        else if (acc_PartonTruthLabel.isAvailable(*j)) flavor = abs(acc_PartonTruthLabel(*j));
        else if (acc_GhostBHadron.isAvailable(*j)) {
            if (acc_GhostBHadron(*j) > 0) flavor = 5;
            else if (acc_GhostCHadron(*j) > 0) flavor = 4;
            else flavor = 1;
        }
        bool isB = (flavor == 5) && PassSignal(*j);
        dec_bjet(*j) = isB;
        return isB;
    }

    xAOD::JetContainer* SUSYTruthSelector::GetTruthCustomJets(std::string kind) {
        xAOD::JetContainer* customJets = nullptr;
        if (LoadViewElementsContainer(kind, customJets).isSuccess()) return customJets;
        return m_BaselineJets;
    }

    StatusCode SUSYTruthSelector::ReclusterTruthJets(const xAOD::IParticleContainer *inputJets, double Rcone, double minPtKt4, std::string PreFix) {
        return ParticleSelector::ReclusterJets(inputJets, Rcone, minPtKt4, PreFix);
    }

    void SUSYTruthSelector::FillBaselineContainer(xAOD::TruthParticleContainer* Pre, xAOD::TruthParticleContainer* Baseline) {
        for (const auto it : *Pre) {
            if (!PassBaseline(*it)) continue;
            Baseline->push_back(it);
        }
    }
    void SUSYTruthSelector::FillSignalContainer(xAOD::TruthParticleContainer* Baseline, xAOD::TruthParticleContainer* Signal) {
        for (const auto it : *Baseline) {
            if (!PassSignal(*it)) continue;
            Signal->push_back(it);
        }
    }

    bool SUSYTruthSelector::isTrueTop(const xAOD::TruthParticle * particle) {

        if (!particle->isTop() || !particle->hasDecayVtx() || particle->isGenSpecific()) return false;
        unsigned int nW(0), nb(0);
        // Find the Top children
        for (size_t c = 0; c < particle->nChildren(); ++c) {
            const xAOD::TruthParticle* child = particle->decayVtx()->outgoingParticle(c);
            if (!child) continue;
            if (child->isTop()) return false;
            else if (child->isW()) {
                ATH_MSG_DEBUG("Found a W child of the top, pdgID = " << child->pdgId());
                ++nW;
            } else if (child->absPdgId() == 5) {
                ATH_MSG_DEBUG("Found a b-quark child of the top, pdgID = " << child->pdgId());
                ++nb;
            }
        }
        ATH_MSG_DEBUG("Found n_b-quark = " << nb << "and nW = " << nW << " children of the top.");
        return (nb == 1 && nW == 1);
    }

    int SUSYTruthSelector::classifyWDecays(const xAOD::TruthParticle * particle) {

        if (!particle->isW() || !particle->hasDecayVtx() || particle->isGenSpecific()) return WDecayModes::Unspecified;
        unsigned int nq(0), ne(0), nm(0), nthad(0), ntlep(0), nnu(0);
        // Find the W children
        for (size_t c = 0; c < particle->nChildren(); ++c) {
            const xAOD::TruthParticle* child = particle->decayVtx()->outgoingParticle(c);
            if (!child) continue;
            if (child->isW()) return WDecayModes::Unspecified;
            if (child->isQuark()) {
                ATH_MSG_DEBUG("Found a quark child of the W, pdgID = " << child->pdgId());
                ++nq;
            }
            if (child->isElectron()) {
                ATH_MSG_DEBUG("Found an electron child of the W, pdgID = " << child->pdgId());
                ++ne;
            }
            if (child->isMuon()) {
                ATH_MSG_DEBUG("Found a muon child of the W, pdgID = " << child->pdgId());
                ++nm;
            }
//            if (isTrueTau(child)) {
//                ATH_MSG_DEBUG("Found a tau child of the W, pdgID = " << child->pdgId());
//                static SG::AuxElement::ConstAcccessor<char> acc_TauDecay("isHadronic");
//                if (!acc_TauDecay(*child)) ++ntlep;
//                else ++nthad;
//            }
            if (child->isNeutrino()) {
                ATH_MSG_DEBUG("Found a neutrino child of the W, pdgID = " << child->pdgId());
                ++nnu;
            }

            ATH_MSG_DEBUG("nq = " << nq << " ne = " << ne << " nm = " << nm << " ntlep = " << ntlep << " nthad = " << nthad << " nnu = " << nnu << "children of the W");
        }
        if (nq == 2) return WDecayModes::Hadronic;
        if (ne == 1 && nnu == 1) return WDecayModes::ElecNeut;
        if (nm == 1 && nnu == 1) return WDecayModes::MuonNeut;
        if (ntlep == 1 && nnu == 1) return WDecayModes::HadTauNeut;
        if (nthad == 1 && nnu == 1) return WDecayModes::LepTauNeut;
        else return WDecayModes::Unspecified;
    }

//This function defines true taus as hadronically decaying taus
    bool SUSYTruthSelector::isTrueTau(xAOD::TruthParticle * particle) {
        if (!particle->isTau() || !particle->hasDecayVtx() || particle->isGenSpecific()) return false;
        static SG::AuxElement::Decorator<char> dec_TauDecay("isHadronic");
        dec_TauDecay(*particle) = true;
        //This decorator is used to choose whether
        static SG::AuxElement::Decorator<float> dec_InitPt("InitPt");
        static SG::AuxElement::Decorator<char> dec_VisPtInit("VisPtInit");
        const xAOD::TruthParticle* Neutrino = nullptr;
        int nC = 0;
        for (size_t c = 0; c < particle->nChildren(); ++c) {
            ++nC;
            const xAOD::TruthParticle* child = particle->decayVtx()->outgoingParticle(c);
            if (!child) continue;
            if (child->isTau()) return false;
            if (child->isChLepton()) dec_TauDecay(*particle) = false;
            if (child->isNeutrino()) Neutrino = child;
        }
        if (Neutrino && (!dec_VisPtInit.isAvailable(*particle) || !dec_VisPtInit(*particle))) {
            dec_VisPtInit(*particle) = true;
            TLorentzVector VisTau = particle->p4() - Neutrino->p4();
            dec_InitPt(*particle) = particle->pt();
            particle->setPx(VisTau.Px());
            particle->setPy(VisTau.Py());
            particle->setPz(VisTau.Pz());
            particle->setE(VisTau.E());
        }
        return (Neutrino != nullptr && nC > 1);
    }

    bool SUSYTruthSelector::isTrueW(const xAOD::TruthParticle * particle) {
//        if (!particle->isW() || !particle->hasDecayVtx() || particle->isGenSpecific()) return false;
//
//        unsigned int nq(0), nl(0), nnu(0);
//        // Find the W children
//        for (size_t c = 0; c < particle->nChildren(); ++c) {
//            const xAOD::TruthParticle* child = particle->decayVtx()->outgoingParticle(c);
//            if (!child) continue;
//            if (child->isW()) return false;
//            else if (child->isQuark()) {
//                ATH_MSG_DEBUG("Found a quark child of the W, pdgID = " << child->pdgId());
//                ++nq;
//            } else if (child->isChLepton()) {
//                ATH_MSG_DEBUG("Found a charged lepton child of the W, pdgID = " << child->pdgId());
//                ++nl;
//            } else if (child->isNeutrino()) {
//                ATH_MSG_DEBUG("Found a neutrino child of the W, pdgID = " << child->pdgId());
//                ++nnu;
//            }
//        }
//        ATH_MSG_DEBUG("Found n_quarks = " << nq << ", n_chargedLep = " << nl << ", and n_neutrinos = " << nnu << " children of the W.");
//        return (nq == 2 || (nl == 1 && nnu == 1));
        return classifyWDecays(particle) != WDecayModes::Unspecified;
    }

    bool SUSYTruthSelector::isTrueZ(const xAOD::TruthParticle * particle) {
        if (!particle->isZ() || !particle->hasDecayVtx() || particle->isGenSpecific()) return false;
        int nC(0), pdgid_C1(0);
        // Find the Z children
        for (size_t c = 0; c < particle->nChildren(); ++c) {
            const xAOD::TruthParticle* child = particle->decayVtx()->outgoingParticle(c);
            if (!child) continue;
            if (child->isZ()) return false;
            else if (child->isQuark() || child->isChLepton() || child->isNeutrino()) {
                ATH_MSG_DEBUG("Found a fermion child of the Z, pdgID = " << child->pdgId());
                if (pdgid_C1 == 0) {
                    ++nC;
                    pdgid_C1 = child->absPdgId();
                } else if (child->absPdgId() == pdgid_C1) ++nC;
            }
        }
        return nC == 2;
    }

    bool SUSYTruthSelector::isTrueSUSY(const xAOD::TruthParticle * particle) {
        if (!XAMPP::isSparticle(*particle) || particle->isGenSpecific() || (particle != XAMPP::GetLastChainLink(particle))) return false;
        return true;
    }
    int SUSYTruthSelector::GetInitialState() {
        int State(0), pdgId1(0), pdgId2(0);
        const xAOD::TruthParticleContainer* C = nullptr;
        if (m_xAODTruthParticles) C = m_xAODTruthParticles;
        else if (m_xAODTruthBSM) C = m_xAODTruthBSM;
        else if (m_xAODTruthBoson) C = m_xAODTruthBoson;
        else if (m_xAODTruthTop) C = m_xAODTruthTop;
        if (!C) {
            ATH_MSG_WARNING("No input container found to read the final state from. Return 0");
            return State;
        }
        if (!C->empty() && !ST::SUSYObjDef_xAOD::FindSusyHardProc(C, pdgId1, pdgId2, isTRUTH3())) ATH_MSG_WARNING("Could not determine the initial SUSY process. May be its background");
        else if ((pdgId1) || (pdgId2)) State = SUSY::finalState(pdgId1, pdgId2);
        return State;
    }
    bool SUSYTruthSelector::isTRUTH3() const {
        return m_isTRUTH3;
    }
    void SUSYTruthSelector::LoadDressedMomentum(xAOD::TruthParticle* Truth) {
        if (!Truth->isChLepton() || Truth->isTau()) {
            ATH_MSG_DEBUG("The truth particle with pt: " << Truth->pt() / 1.e3 << " GeV, eta:" << Truth->eta() << ", phi: " << Truth->phi() << ", pdgId:" << Truth->pdgId());
            return;
        }
        FloatAccessor acc_e("e_dressed");
        FloatAccessor acc_pt("pt_dressed");
        FloatAccessor acc_eta("eta_dressed");
        FloatAccessor acc_phi("phi_dressed");

        SG::AuxElement::ConstAccessor<char> acc_MomLoad("loaded_p4");
        SG::AuxElement::Decorator<char> dec_MomLoad("loaded_p4");
        if (!acc_MomLoad.isAvailable(*Truth) || acc_MomLoad(*Truth) != 2) {
            if (acc_e.isAvailable(*Truth)) {
                dec_MomLoad(*Truth) = 2;
                TLorentzVector V;
                V.SetPtEtaPhiE(acc_pt(*Truth), acc_eta(*Truth), acc_phi(*Truth), acc_e(*Truth));
                Truth->setPx(V.Px());
                Truth->setPy(V.Py());
                Truth->setPz(V.Pz());
                Truth->setE(V.E());
            }
        }
    }
    bool SUSYTruthSelector::PassBaselineKinematics(const xAOD::IParticle &P) const {
        if (P.type() == xAOD::Type::ObjectType::Jet) {
            return P.pt() > m_BaselineJetPtCut && (m_BaselineJetEtaCut < 0. || fabs(P.eta()) < m_BaselineJetEtaCut);
        } else if (P.type() == xAOD::Type::ObjectType::TruthParticle) {
            unsigned int pdgId = fabs(TypeToPdgId(P));
            //Electrons
            if (pdgId == 11) return P.pt() > m_BaselineElePtCut && (m_BaselineEleEtaCut < 0. || fabs(P.eta()) < m_BaselineEleEtaCut);
            //Muons
            else if (pdgId == 13) return P.pt() > m_BaselineMuPtCut && (m_BaselineMuEtaCut < 0. || fabs(P.eta()) < m_BaselineMuEtaCut);
            //Taus
            else if (pdgId == 15) return P.pt() > m_BaselineTauPtCut && (m_BaselineTauEtaCut < 0. || fabs(P.eta()) < m_BaselineTauEtaCut);
            //Neutrinos
            else if (pdgId == 12 || pdgId == 14 || pdgId == 16) return P.pt() > m_BaselineNeutrinoPtCut && (m_BaselineNeutrinoEtaCut < 0. || fabs(P.eta()) < m_BaselineNeutrinoEtaCut);
            //Photons
            else if (pdgId == 22) return P.pt() > m_BaselinePhotonPtCut && (m_BaselinePhotonEtaCut < 0. || fabs(P.eta()) < m_BaselinePhotonEtaCut);
        }
        return ParticleSelector::PassBaselineKinematics(P);
    }
    bool SUSYTruthSelector::PassSignalKinematics(const xAOD::IParticle &P) const {
        if (P.type() == xAOD::Type::ObjectType::Jet) {
            return P.pt() > m_SignalJetPtCut && (m_SignalJetEtaCut < 0. || fabs(P.eta()) < m_SignalJetEtaCut);
        } else if (P.type() == xAOD::Type::ObjectType::TruthParticle) {
            unsigned int pdgId = fabs(TypeToPdgId(P));
            //Electrons
            if (pdgId == 11) return P.pt() > m_SignalElePtCut && (m_SignalEleEtaCut < 0. || fabs(P.eta()) < m_SignalEleEtaCut);
            //Muons
            else if (pdgId == 13) return P.pt() > m_SignalMuPtCut && (m_SignalMuEtaCut < 0. || fabs(P.eta()) < m_SignalMuEtaCut);
            //Taus
            else if (pdgId == 15) return P.pt() > m_SignalTauPtCut && (m_SignalTauEtaCut < 0. || fabs(P.eta()) < m_SignalTauEtaCut);
            //Neutrinos
            else if (pdgId == 12 || pdgId == 14 || pdgId == 16) return P.pt() > m_SignalNeutrinoPtCut && (m_SignalNeutrinoEtaCut < 0. || fabs(P.eta()) < m_SignalNeutrinoEtaCut);
            //Photons
            else if (pdgId == 22) return P.pt() > m_SignalPhotonPtCut && (m_SignalPhotonEtaCut < 0. || fabs(P.eta()) < m_SignalPhotonEtaCut);
        }
        return ParticleSelector::PassSignalKinematics(P);
    }
    bool SUSYTruthSelector::PassSignal(const xAOD::IParticle &P) const {
        if (!ParticleSelector::PassSignal(P)) return false;
        if (P.type() == xAOD::Type::ObjectType::Jet || !m_RequireSignalFromHardProc) return true;
        const xAOD::TruthParticle* truthPart = dynamic_cast<const xAOD::TruthParticle*>(&P);
        if (!truthPart) PromptParticle(truthPart, "What is that for a particle???");
        else return isParticleFromHardProcess(truthPart);
        return false;
    }
    bool SUSYTruthSelector::PassBaseline(const xAOD::IParticle &P) const {
        if (!ParticleSelector::PassBaseline(P)) return false;
        if (P.type() == xAOD::Type::ObjectType::Jet || !m_RequireBaseFromHardProc) return true;
        //Check if the particle originates from the HardProcess or is a fake
        const xAOD::TruthParticle* truthPart = dynamic_cast<const xAOD::TruthParticle*>(&P);
        if (!truthPart) PromptParticle(truthPart, "What is that for a particle???");
        else return isParticleFromHardProcess(truthPart);
        return false;
    }
    bool SUSYTruthSelector::PassPreSelection(const xAOD::IParticle &P) const {
        if (!ParticleSelector::PassPreSelection(P)) return false;
        if (P.type() == xAOD::Type::ObjectType::Jet || !m_RequirePreselFromHardProc) return true;
        //Check if the particle originates from the HardProcess or is a fake
        const xAOD::TruthParticle* truthPart = dynamic_cast<const xAOD::TruthParticle*>(&P);
        if (!truthPart) PromptParticle(truthPart, "What is that for a particle???");
        else return isParticleFromHardProcess(truthPart);
        return false;
    }
    bool SUSYTruthSelector::DoTruthJets() const {
        return m_doTruthJets;
    }
    std::string SUSYTruthSelector::TauKey() const {
        return m_TauKey;
    }
    std::string SUSYTruthSelector::ElectronKey() const {
        return m_ElectronKey;
    }
    std::string SUSYTruthSelector::MuonKey() const {
        return m_MuonKey;
    }
    std::string SUSYTruthSelector::PhotonKey() const {
        return m_PhotonKey;
    }
    std::string SUSYTruthSelector::NeutrinoKey() const {
        return m_NeutrinoKey;
    }
    std::string SUSYTruthSelector::BosonKey() const {
        return m_BosonKey;
    }
    std::string SUSYTruthSelector::BSMKey() const {
        return m_BSMKey;
    }
    std::string SUSYTruthSelector::TopKey() const {
        return m_TopKey;
    }
}
