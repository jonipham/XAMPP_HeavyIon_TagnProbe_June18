#ifndef XAMPPbase_AnalysisUtils_H
#define XAMPPbase_AnalysisUtils_H

#include <xAODBase/IParticleHelpers.h>

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertex.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTagging.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingET.h"

#include <XAMPPbase/EventStorage.h>
#include <XAMPPbase/ReconstructedParticles.h>
#include <XAMPPbase/Defs.h>

#include <vector>
#include "PATInterfaces/SystematicSet.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "TLorentzVector.h"
#include <TMath.h>
#include <iostream>
#include <fstream>

#include <AsgTools/AsgTool.h>
#include <AsgTools/ToolHandle.h>

namespace XAMPP {
    class EventInfo;

    bool ptsorter(const xAOD::IParticle* j1, const xAOD::IParticle* j2);
    //bool btagweightsorter(const xAOD::Jet* j1, const xAOD::Jet* j2);
    std::vector<std::string> GetPathResolvedFileList(const std::vector<std::string> &Files);

    StatusCode ResetOverlapDecorations(const xAOD::IParticleContainer* Container);
    StatusCode RemoveOverlap(xAOD::IParticleContainer* RemFrom, xAOD::IParticleContainer* RemWith, float dR, bool UseRapidity = true);
    StatusCode RemoveOverlap(xAOD::IParticleContainer* RemFrom, xAOD::IParticleContainer* RemWith, std::function<float(const xAOD::IParticle*, const xAOD::IParticle*)> radiusFunc, bool UseRapidity = true);

    Momentum FourMomentum(const std::string& str);
    std::string FourMomentum(Momentum Mom);

    template<typename T> bool ToolIsAffectedBySystematic(ToolHandle<T> &Handle, const CP::SystematicSet* set) {
        if (set->name().empty()) {
            return true;
        }
        for (const auto& sys : *set) {
            if (Handle->isAffectedBySystematic(sys)) return true;
        }
        return false;
    }
    int Index(const xAOD::IParticle* P);
    unsigned int GetNthPrime(unsigned int N);

    void FillVectorFromString(std::vector<std::string> & str_vector, std::string &str);
    std::string EraseWhiteSpaces(std::string str);
    std::string ReplaceExpInString(std::string str, const std::string &exp, const std::string &rep);
    std::string ToLower(const std::string &str);
    bool GetLine(std::ifstream &inf, std::string &line);

    MSG::Level setOutputLevel(int m_output_level_int);
    xAOD::IParticle* FindLeadingParticle(xAOD::IParticleContainer* Particles);
    std::string RemoveAllExpInStr(std::string Str, std::string Exp);
    float GetLeadingPt(xAOD::IParticleContainer* Particles);

    bool Overlaps(const xAOD::IParticle &P1, const xAOD::IParticle &P2, float dR, bool UseRapidity = false);
    bool Overlaps(const xAOD::IParticle* P1, const xAOD::IParticle* P2, float dR, bool UseRapidity = false);

    bool IsRecoLepton(const xAOD::IParticle &L);
    bool IsRecoLepton(const xAOD::IParticle* L);

    bool OppositeSign(const xAOD::IParticle &L, const xAOD::IParticle &L1);
    bool OppositeSign(const xAOD::IParticle* L, const xAOD::IParticle* L1);
    bool OppositeSignLead2(const xAOD::IParticleContainer *pc);

    bool SameFlavour(const xAOD::IParticle &L, const xAOD::IParticle &L1);
    bool SameFlavour(const xAOD::IParticle* L, const xAOD::IParticle* L1);

    bool OppositeFlavour(const xAOD::IParticle &L, const xAOD::IParticle &L1);
    bool OppositeFlavour(const xAOD::IParticle* L, const xAOD::IParticle* L1);

    bool IsSame(const xAOD::IParticle &P, const xAOD::IParticle &P1, bool DeRefShallowCopy = false);
    bool IsSame(const xAOD::IParticle* P, const xAOD::IParticle* P1, bool DeRefShallowCopy = false);

    bool IsNotSame(const xAOD::IParticle &P, const xAOD::IParticle &P1);
    bool IsNotSame(const xAOD::IParticle* P, const xAOD::IParticle* P1);

    bool IsSFOS(const xAOD::IParticle &L, const xAOD::IParticle &L1);
    bool IsSFOS(const xAOD::IParticle* L, const xAOD::IParticle* L1);

    bool IsDFOS(const xAOD::IParticle &L, const xAOD::IParticle &L1);
    bool IsDFOS(const xAOD::IParticle* L, const xAOD::IParticle* L1);

    StatusCode RemoveLowMassLeptons(xAOD::IParticleContainer* Leptons, bool (*pair)(const xAOD::IParticle*, const xAOD::IParticle*), float Upper_Mll, float Lower_Mll = 0.);
    StatusCode RemoveLowMassLeptons(xAOD::IParticleContainer* Leptons, float Upper_Mll, float Lower_Mll = 0.);

    //This function calculates the invariant momenta of P1 and P2 storing it in Candidate. In addition the charge is calculated.
    //Information whether the particle is formed from to same parts and of two IsSignal objects is also stored
    void AddFourMomenta(const xAOD::IParticle* P, const xAOD::IParticle* P1, xAOD::Particle* Candidate, bool (*IsSignal)(const xAOD::IParticle*));

    //These functions loop through the IParticleContainers constructing all possible invariant momenta...
    void ConstructInvariantMomenta(xAOD::IParticleContainer* Leptons, XAMPP::IReconstructedParticles* Constructor, bool (*IsSignal)(const xAOD::IParticle*), int MaxN = -1);
    void ConstructInvariantMomenta(xAOD::IParticleContainer& Leptons, XAMPP::IReconstructedParticles* Constructor, bool (*IsSignal)(const xAOD::IParticle*), int MaxN = -1);
    void ConstructInvariantMomenta(xAOD::IParticleContainer& Leptons, xAOD::IParticleContainer::const_iterator Itr, XAMPP::IReconstructedParticles* Constructor, bool (*IsSignal)(const xAOD::IParticle*), int MaxN, const xAOD::IParticle* In = NULL);
    //The entire things with an asg::AnaToolHandle as wrapper again
    void ConstructInvariantMomenta(xAOD::IParticleContainer* Leptons, asg::AnaToolHandle<XAMPP::IReconstructedParticles> &Constructor, bool (*IsSignal)(const xAOD::IParticle*), int MaxN = -1);
    void ConstructInvariantMomenta(xAOD::IParticleContainer& Leptons, asg::AnaToolHandle<XAMPP::IReconstructedParticles> &Constructor, bool (*IsSignal)(const xAOD::IParticle*), int MaxN = -1);
    void ConstructInvariantMomenta(xAOD::IParticleContainer& Leptons, xAOD::IParticleContainer::const_iterator Itr, asg::AnaToolHandle<XAMPP::IReconstructedParticles> &Constructor, bool (*IsSignal)(const xAOD::IParticle*), int MaxN, const xAOD::IParticle* In = NULL);

    float Charge(const xAOD::IParticle* p);
    float Charge(const xAOD::IParticle& p);

    //Returns the pdgIds of the Particle
    int TypeToPdgId(const xAOD::IParticle* P);
    int TypeToPdgId(const xAOD::IParticle& P);

    //Computes the Mt between two particles or one particle and the MissingEt
    float ComputeMt(const xAOD::IParticle* v1, XAMPP::Storage<XAMPPmet>* met);
    float ComputeMt(const xAOD::IParticle& v1, XAMPP::Storage<XAMPPmet>* met);

    float ComputeMt(const xAOD::IParticle* v1, const xAOD::MissingET* met);
    float ComputeMt(const xAOD::IParticle& v1, const xAOD::MissingET& met);

    float ComputeMt(const xAOD::IParticle* v1, const xAOD::IParticle* v2);
    float ComputeMt(const xAOD::IParticle& v1, const xAOD::IParticle& v2);

    void CalculateMt(xAOD::IParticleContainer& Particles, XAMPP::Storage<xAOD::MissingET*>* dec_MET);
    void CalculateMt(const xAOD::IParticleContainer* Particles, XAMPP::Storage<xAOD::MissingET*>* dec_MET);

    float RelPt(const xAOD::IParticle* v1, const xAOD::IParticle* v2);
    float RelPt(const xAOD::IParticle& v1, const xAOD::IParticle& v2);

    //Computes the invariant mass of two to four particles
    float InvariantMass(const xAOD::IParticle& P1, const xAOD::IParticle& P2);
    float InvariantMass(const xAOD::IParticle* P1, const xAOD::IParticle* P2);
    float InvariantMass(const xAOD::IParticle& P1, const xAOD::IParticle& P2, const xAOD::IParticle& P3);
    float InvariantMass(const xAOD::IParticle* P1, const xAOD::IParticle* P2, const xAOD::IParticle* P3);
    float InvariantMass(const xAOD::IParticle& P1, const xAOD::IParticle& P2, const xAOD::IParticle& P3, const xAOD::IParticle& P4);
    float InvariantMass(const xAOD::IParticle* P1, const xAOD::IParticle* P2, const xAOD::IParticle* P3, const xAOD::IParticle* P4);

    int GetZVeto(xAOD::IParticleContainer &Particles, float Z_Window);
    int GetZVeto(xAOD::IParticleContainer* Particles, float Z_Window);
    int GetZVeto(xAOD::IParticleContainer* Electrons, xAOD::IParticleContainer* Muons, float Z_Window = 1.e4);

    float CalculateHt(xAOD::IParticleContainer* Container, float MinPt = 0.);
    float CalculateJetHt(xAOD::JetContainer* Jets, float MinJetPt = 0.);

    float CalculateLeptonHt(xAOD::ElectronContainer* Electrons, xAOD::MuonContainer* Muons, xAOD::TauJetContainer* Taus = NULL);
    float CalculatePhotonHt(xAOD::PhotonContainer* Photons);

    const xAOD::IParticle* GetClosestParticle(xAOD::IParticleContainer* Cont, XAMPP::Storage<XAMPPmet>* met, xAOD::IParticleContainer* Exclude = NULL);
    const xAOD::IParticle* GetClosestParticle(xAOD::IParticleContainer* Cont, xAOD::MissingET* met, xAOD::IParticleContainer* Exclude = NULL);

    const xAOD::IParticle* GetFarestParticle(xAOD::IParticleContainer* Cont, XAMPP::Storage<XAMPPmet>* met, xAOD::IParticleContainer* Exclude = NULL);
    const xAOD::IParticle* GetFarestParticle(xAOD::IParticleContainer* Cont, xAOD::MissingET* met, xAOD::IParticleContainer* Exclude = NULL);

    const xAOD::IParticle* GetClosestParticle(xAOD::IParticleContainer* Cont, const xAOD::IParticle* P1, xAOD::IParticleContainer* Exclude = NULL);
    const xAOD::IParticle* GetFarestParticle(xAOD::IParticleContainer* Cont, const xAOD::IParticle* P1, xAOD::IParticleContainer* Exclude = NULL);

    const xAOD::IParticle* GetClosestParticle(const xAOD::IParticleContainer* LookIn, const xAOD::IParticle* RefPart, bool UseRapidity = false, const xAOD::IParticleContainer* RefContainer = NULL);

    float ComputeMtMin(xAOD::IParticleContainer* Collection, XAMPP::Storage<XAMPPmet>* met);
    float ComputeMtMin(xAOD::IParticleContainer* Collection, xAOD::MissingET* met);
    float ComputeMtMin(xAOD::IParticleContainer* Collection, const xAOD::IParticle* P1);
    float ComputeMtMax(xAOD::IParticleContainer* Collection, XAMPP::Storage<XAMPPmet>* met);
    float ComputeMtMax(xAOD::IParticleContainer* Collection, xAOD::MissingET* met);
    float ComputeMtMax(xAOD::IParticleContainer* Collection, const xAOD::IParticle* P1);
    float ComputeDPhiMin(xAOD::JetContainer* Jets, XAMPP::Storage<XAMPPmet>* met, unsigned int NJetsToUse = 0);
    float ComputeDPhiMin(xAOD::JetContainer* Jets, xAOD::MissingET* met, unsigned int NJetsToUse = 0);
    bool HasTauCandidate(xAOD::JetContainer* Jets, xAOD::MissingET* met, float &MtTauCand, int &Ntracks);
    float ComputeMinMt(xAOD::JetContainer* Jets, xAOD::MissingET* met);

    float ComputeJetAngularSep(xAOD::JetContainer* Jets);

    float ComputeAbsDeltaPhi(xAOD::MissingET* met1, xAOD::MissingET* met2);
    float ComputeDeltaPhi(xAOD::MissingET* met1, xAOD::MissingET* met2);

    float ComputeDeltaPhi(XAMPP::Storage<XAMPPmet>* met1, XAMPP::Storage<XAMPPmet>* met2);
    float ComputeAbsDeltaPhi(XAMPP::Storage<XAMPPmet>* met1, XAMPP::Storage<XAMPPmet>* met2);

    bool RecoZfromLeps(xAOD::IParticleContainer* Lep, xAOD::Particle* RecoZ, float MassWindow = 1.e4);

    void GetTopCandidates(xAOD::JetContainer* BJets, xAOD::JetContainer* LightJets, xAOD::Particle* W1Cand, xAOD::Particle* W2Cand, xAOD::Particle* Top1Cand, xAOD::Particle* Top2Cand);

    void GetTopCandidatesDRB4(xAOD::JetContainer* BJets, xAOD::JetContainer* LightJets, xAOD::Particle* W1CandDRB4, xAOD::Particle* W2CandDRB4, xAOD::Particle* Top1CandDRB4, xAOD::Particle* Top2CandDRB4);

    void GetTopCandidatesMinMass(xAOD::JetContainer* BJets, xAOD::JetContainer* LightJets, xAOD::Particle* W1CandMinMass, xAOD::Particle* W2CandMinMass, xAOD::Particle* Top1CandMinMass, xAOD::Particle* Top2CandMinMass);

    bool IsBJet(const xAOD::IParticle* P);

    float GetChi2FromTopConstruction(float WMass, float TopMass);
    bool HasCommonConstructingParticles(const xAOD::IParticle* First, const xAOD::IParticle* Second);

    xAOD::Particle* GetTop(const xAOD::IParticle* W, const xAOD::Jet* B, asg::AnaToolHandle<XAMPP::IReconstructedParticles>& Constructor);
    xAOD::Particle* GetTop(const xAOD::IParticle* W, const xAOD::Jet* B, XAMPP::IReconstructedParticles* Constructor);

    StatusCode GetTopCandidatesChi2(xAOD::JetContainer* SignalJets, const xAOD::JetContainer* BJets, asg::AnaToolHandle<XAMPP::IReconstructedParticles> &Constructor);
    StatusCode GetTopCandidatesChi2(xAOD::JetContainer* SignalJets, const xAOD::JetContainer* BJets, XAMPP::IReconstructedParticles* Constructor);

    xAOD::MissingET* GetMET_obj(std::string Name, xAOD::MissingETContainer* Cont);

    void PromptParticle(const xAOD::IParticle* Part, std::string AddInfo = "");
    void PromptParticle(const xAOD::IParticle& Part, std::string AddInfo = "");

    template<typename T> bool IsInContainer(const xAOD::IParticle& P, T& Container) {
        return IsInContainer(&P, Container);
    }
    template<typename T> bool IsInContainer(const xAOD::IParticle* P, T& Container) {
        typename T::const_iterator Check;
        for (Check = Container.begin(); Check != Container.end(); ++Check)
            if (IsSame(*P, **Check)) return true;
        return false;
    }
    template<typename T> bool IsInVector(const T& Ele, const std::vector<T> &Vec) {
        for (auto& In : Vec) {
            if (Ele == In) return true;
        }
        return false;
    }
    template<typename T> void CopyVector(const std::vector<T> & From, std::vector<T> &To, bool Clear = true) {
        if (Clear) To.clear();
        if (To.capacity() < From.size()) To.reserve(From.size() + To.capacity());
        for (auto& Ele : From) {
            if (!IsInVector(To, Ele)) To.push_back(Ele);
        }
        To.shrink_to_fit();
    }
    bool IsChargedFlipped(const xAOD::Electron* el);
    //Truth particle functions
    bool isSparticle(const xAOD::TruthParticle& P);
    bool isSparticle(const xAOD::TruthParticle* P);

    bool isGluon(const xAOD::TruthParticle& P);
    bool isGluon(const xAOD::TruthParticle* P);

    bool isEWboson(const xAOD::TruthParticle& P);
    bool isEWboson(const xAOD::TruthParticle* P);
    const xAOD::TruthParticle* getTruthMatchedParticle(const xAOD::IParticle& P);

    int getParticleTruthOrigin(const xAOD::IParticle& P);
    int getParticleTruthOrigin(const xAOD::IParticle* P);

    int getParticleTruthType(const xAOD::IParticle& P);
    int getParticleTruthType(const xAOD::IParticle* P);

    const xAOD::TruthParticle* GetFirstChainLink(const xAOD::TruthParticle* TruthPart);
    const xAOD::TruthParticle* GetLastChainLink(const xAOD::TruthParticle* TruthPart);
    bool isParticleFromHardProcess(const xAOD::TruthParticle* TruthPart);

    //RPV-LLE Decay Functions 
    LLE_Decay GetFinalRPVState(const xAOD::TruthParticle* LSP);
    int GetLLE_RPVCoupling(const xAOD::TruthParticle* LSP);

    bool IsInOutGoing(const xAOD::TruthParticle* P);

    float MinDeltaR(const xAOD::IParticle* p, const xAOD::IParticleContainer* pc);

}
#endif
