#ifndef XAMPPbase_SUSYTruthSelector_H
#define XAMPPbase_SUSYTruthSelector_H

#include <XAMPPbase/ITruthSelector.h>
#include <XAMPPbase/ParticleSelector.h>
#include <XAMPPbase/AnalysisUtils.h>

namespace XAMPP {
    class SUSYTruthSelector: public ParticleSelector, virtual public ITruthSelector {
        public:
            ASG_TOOL_CLASS(SUSYTruthSelector, XAMPP::ITruthSelector)

            SUSYTruthSelector(std::string myname);
            virtual ~SUSYTruthSelector() {
            }

            virtual int GetInitialState();
            virtual StatusCode initialize();

            virtual StatusCode LoadContainers();
            virtual StatusCode InitialFill(const CP::SystematicSet &systset);
            virtual StatusCode FillTruth(const CP::SystematicSet &systset);

            virtual bool isTrueTop(const xAOD::TruthParticle * particle);
            virtual bool isTrueW(const xAOD::TruthParticle * particle);
            virtual bool isTrueZ(const xAOD::TruthParticle * particle);
            virtual bool isTrueSUSY(const xAOD::TruthParticle * particle);
            virtual bool isTrueTau(xAOD::TruthParticle * particle);
            virtual int classifyWDecays(const xAOD::TruthParticle * particle);
            virtual xAOD::TruthParticleContainer* GetTruthPreElectrons() const {
                return m_PreElectrons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthBaselineElectrons() const {
                return m_BaselineElectrons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthSignalElectrons() const {
                return m_SignalElectrons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthPreMuons() const {
                return m_PreMuons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthBaselineMuons() const {
                return m_BaselineMuons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthSignalMuons() const {
                return m_SignalMuons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthPrePhotons() const {
                return m_PrePhotons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthBaselinePhotons() const {
                return m_BaselinePhotons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthSignalPhotons() const {
                return m_SignalPhotons;
            }
            virtual xAOD::TruthParticleContainer* GetTruthPreTaus() const {
                return m_PreTaus;
            }
            virtual xAOD::TruthParticleContainer* GetTruthBaselineTaus() const {
                return m_BaselineTaus;
            }
            virtual xAOD::TruthParticleContainer* GetTruthSignalTaus() const {
                return m_SignalTaus;
            }
            virtual xAOD::TruthParticleContainer* GetTruthNeutrinos() const {
                return m_Neutrinos;
            }
            virtual xAOD::TruthParticleContainer* GetTruthPrimaryParticles() const {
                return m_InitialStatePart;
            }
            virtual xAOD::JetContainer* GetTruthPreJets() const {
                return m_PreJets;
            }
            virtual xAOD::JetContainer* GetTruthBaselineJets() const {
                return m_BaselineJets;
            }
            virtual xAOD::JetContainer* GetTruthSignalJets() const {
                return m_SignalJets;
            }
            virtual xAOD::JetContainer* GetTruthBJets() const {
                return m_BJets;
            }
            virtual xAOD::JetContainer* GetTruthLightJets() const {
                return m_LightJets;
            }
            virtual xAOD::JetContainer* GetTruthFatJets() const {
                return m_InitialFatJets;
            }
            virtual xAOD::JetContainer* GetTruthCustomJets(std::string kind);

            virtual xAOD::TruthParticleContainer * GetTruthParticles() const {
                return m_Particles;
            }
            virtual const xAOD::TruthParticleContainer* GetTruthInContainer() const {
                return m_xAODTruthParticles;
            }
            virtual StatusCode ReclusterTruthJets(const xAOD::IParticleContainer *inputJets, double Rcone, double minPtKt4 = -1., std::string PreFix = "");

        protected:
            bool isTRUTH3() const;

            virtual bool PassBaselineKinematics(const xAOD::IParticle &P) const;
            virtual bool PassSignalKinematics(const xAOD::IParticle &P) const;

            virtual bool PassSignal(const xAOD::IParticle &P) const;
            virtual bool PassBaseline(const xAOD::IParticle &P) const;
            virtual bool PassPreSelection(const xAOD::IParticle &P) const;

            virtual void InitDecorators(const xAOD::TruthParticle* T, bool Pass = true);
            virtual void InitDecorators(const xAOD::Jet* J, bool Pass);

            template<typename Cont> void InitContainer(Cont* Input, Cont* PreSel) {
                for (auto ipart : *Input) {
                    InitDecorators(ipart, PassBaselineKinematics(*ipart));
                    if (PassPreSelection(*ipart)) PreSel->push_back(ipart);
                }
                PreSel->sort(XAMPP::ptsorter);
            }
            virtual bool IsGenParticle(const xAOD::TruthParticle* Truthpart) const;
            virtual bool ConsiderParticle(xAOD::TruthParticle* particle);
            virtual bool IsBJet(const xAOD::IParticle* j);

            void FillBaselineContainer(xAOD::TruthParticleContainer* Pre, xAOD::TruthParticleContainer* Baseline);
            void FillSignalContainer(xAOD::TruthParticleContainer* Baseline, xAOD::TruthParticleContainer* Signal);

            StatusCode FillTruthParticleContainer();
            StatusCode RetrieveParticleContainer(xAOD::TruthParticleContainer* &Particles, bool FromStoreGate, const std::string& GateKey, const std::string &ViewElementsKey);
            void LoadDressedMomentum(xAOD::TruthParticle* Truth);
            bool DoTruthJets() const;

            std::string TauKey() const;
            std::string ElectronKey() const;
            std::string MuonKey() const;
            std::string PhotonKey() const;

            std::string NeutrinoKey() const;
            std::string BosonKey() const;
            std::string BSMKey() const;
            std::string TopKey() const;

        private:

            float m_BaselineMuPtCut;
            float m_BaselineMuEtaCut;

            float m_BaselineElePtCut;
            float m_BaselineEleEtaCut;

            float m_SignalMuPtCut;
            float m_SignalMuEtaCut;

            float m_SignalElePtCut;
            float m_SignalEleEtaCut;

            float m_BJetPtCut;
            float m_BJetEtaCut;
            float m_BaselineJetPtCut;
            float m_BaselineJetEtaCut;
            float m_SignalJetPtCut;
            float m_SignalJetEtaCut;

            float m_BaselinePhotonPtCut;
            float m_BaselinePhotonEtaCut;

            float m_SignalPhotonPtCut;
            float m_SignalPhotonEtaCut;

            float m_BaselineTauPtCut;
            float m_BaselineTauEtaCut;
            float m_SignalTauPtCut;
            float m_SignalTauEtaCut;

            float m_BaselineNeutrinoPtCut;
            float m_BaselineNeutrinoEtaCut;
            float m_SignalNeutrinoPtCut;
            float m_SignalNeutrinoEtaCut;

            bool m_RequirePreselFromHardProc;
            bool m_RequireBaseFromHardProc;
            bool m_RequireSignalFromHardProc;

        protected:
            const xAOD::JetContainer* m_xAODTruthJets;

            const xAOD::TruthParticleContainer* m_xAODTruthParticles;
            const xAOD::MissingETContainer* m_xAODTruthMet;
            const xAOD::TruthParticleContainer* m_xAODTruthBSM;
            const xAOD::TruthParticleContainer* m_xAODTruthBoson;
            const xAOD::TruthParticleContainer* m_xAODTruthTop;

            xAOD::TruthParticleContainer* m_InitialParticles;

            xAOD::JetContainer* m_InitialJets;
            xAOD::JetContainer* m_InitialFatJets;

            xAOD::TruthParticleContainer* m_InitialElectrons;
            xAOD::TruthParticleContainer* m_InitialMuons;
            xAOD::TruthParticleContainer* m_InitialPhotons;
            xAOD::TruthParticleContainer* m_InitialTaus;
            xAOD::TruthParticleContainer* m_InitialNeutrinos;

            xAOD::JetContainer* m_PreJets;
            xAOD::JetContainer* m_BaselineJets;
            xAOD::JetContainer* m_SignalJets;
            xAOD::JetContainer* m_BJets;
            xAOD::JetContainer* m_LightJets;

            xAOD::TruthParticleContainer* m_Particles;

            xAOD::TruthParticleContainer* m_PreElectrons;
            xAOD::TruthParticleContainer* m_BaselineElectrons;
            xAOD::TruthParticleContainer* m_SignalElectrons;
            xAOD::TruthParticleContainer* m_PreMuons;
            xAOD::TruthParticleContainer* m_BaselineMuons;
            xAOD::TruthParticleContainer* m_SignalMuons;
            xAOD::TruthParticleContainer* m_PrePhotons;
            xAOD::TruthParticleContainer* m_BaselinePhotons;
            xAOD::TruthParticleContainer* m_SignalPhotons;
            xAOD::TruthParticleContainer* m_PreTaus;
            xAOD::TruthParticleContainer* m_BaselineTaus;
            xAOD::TruthParticleContainer* m_SignalTaus;
            xAOD::TruthParticleContainer* m_Neutrinos;
            xAOD::TruthParticleContainer* m_InitialStatePart;
            private:
            bool m_doTruthJets;
            bool m_doTruthElectrons;
            bool m_doTruthMuons;
            bool m_doTruthTaus;
            bool m_doTruthPhotons;
            bool m_doTruthNeutrinos;
            bool m_doTruthParticles;
            bool m_doTruthTop;
            bool m_doTruthBoson;
            bool m_doTruthSUSY;
            bool m_isTRUTH3;

            std::string m_ElectronKey;
            std::string m_MuonKey;
            std::string m_TauKey;
            std::string m_PhotonKey;
            std::string m_NeutrinoKey;
            std::string m_BosonKey;
            std::string m_BSMKey;
            std::string m_TopKey;
    };
}
#endif
