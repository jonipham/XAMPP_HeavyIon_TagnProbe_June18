#ifndef XAMPPbase_SUSYJetSelector_H
#define XAMPPbase_SUSYJetSelector_H

#include <XAMPPbase/IJetSelector.h>
#include <XAMPPbase/SUSYParticleSelector.h>

namespace XAMPP {
    struct JetWeightDecorators {
            const CP::SystematicSet* Systematic;
            SG::AuxElement::Decorator<double>* JetSF;
            SG::AuxElement::Decorator<double>* JetSF_BTag;
            SG::AuxElement::Decorator<double>* JetSF_JVT;

            XAMPP::Storage<double>* TotalSF;
            XAMPP::Storage<double>* TotalSF_BTag;
            XAMPP::Storage<double>* TotalSF_JVT;
    };

    class SUSYJetSelector: public SUSYParticleSelector, virtual public IJetSelector {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYJetSelector, XAMPP::IJetSelector)

            SUSYJetSelector(std::string myname);
            virtual ~SUSYJetSelector();

            virtual StatusCode initialize();

            virtual StatusCode LoadContainers();
            virtual StatusCode InitialFill(const CP::SystematicSet &systset);
            virtual StatusCode FillJets(const CP::SystematicSet &systset);

            virtual xAOD::JetContainer* GetJets() const {
                return m_Jets;
            }
            virtual xAOD::JetContainer* GetPreJets() const {
                return m_PreJets;
            }
            virtual xAOD::JetContainer* GetBaselineJets() const {
                return m_BaselineJets;
            }
            virtual xAOD::JetContainer* GetSignalJets() const {
                return m_SignalJets;
            }
            virtual xAOD::JetContainer* GetPreFatJets(int Radius = 99) const {
                if (Radius == 4) return GetPreJets();
                return m_PreFatJets;
            }
            virtual xAOD::JetContainer* GetBJets() const {
                return m_BJets;
            }
            virtual xAOD::JetContainer* GetLightJets() const {
                return m_LightJets;
            }
            virtual xAOD::JetContainer* GetSignalJetsNoJVT() const {
                return m_PreJVTJets;
            }
            virtual xAOD::JetContainer* GetCustomJets(const std::string &kind);

            virtual StatusCode ReclusterJets(const xAOD::IParticleContainer *inputJets, double Rcone, double minPtKt4 = -1., std::string PreFix = "");
            virtual StatusCode SaveScaleFactor();
        protected:
            virtual StatusCode CalibrateJets(const std::string &Key, xAOD::JetContainer* &Container, xAOD::JetContainer* &PreSelected, const std::string &PreSelName = "", JetAlgorithm Cone = JetAlgorithm::AntiKt4);

            virtual StatusCode CalibrateJets(const std::string &Key, xAOD::JetContainer* &PreSelected, const std::string &PreSelName = "", JetAlgorithm Cone = JetAlgorithm::AntiKt4);

            virtual bool IsBadJet(const xAOD::Jet& jet) const;
            virtual bool IsBJet(const xAOD::Jet& jet) const;
            //virtual double BtagBDT(const xAOD::Jet& jet) const;

            virtual bool PassBaselineKinematics(const xAOD::IParticle &P) const;
            virtual bool PassSignalKinematics(const xAOD::IParticle &P) const;

            const xAOD::JetContainer* m_xAODJets;
            xAOD::JetContainer* m_Jets;

            xAOD::JetContainer* m_PreJets;
            xAOD::JetContainer* m_BaselineJets;
            xAOD::JetContainer* m_PreJVTJets;
            xAOD::JetContainer* m_SignalJets;

            xAOD::JetContainer* m_PreFatJets;

            xAOD::JetContainer* m_BJets;
            xAOD::JetContainer * m_LightJets;
        private:
            XAMPP::Storage<int>* m_decNBad;
            float m_bJetEtaCut;

            bool m_SeparateSF;
            std::vector<JetWeightDecorators> m_SF;
            bool m_doBTagSF;
            bool m_doJVTSF;
            bool m_doLargeRdecors;

            float m_Kt10_BaselinePt;
            float m_Kt10_BaselineEta;
            float m_Kt10_SignalPt;
            float m_Kt10_SignalEta;

            float m_Kt02_BaselinePt;
            float m_Kt02_BaselineEta;
            float m_Kt02_SignalPt;
            float m_Kt02_SignalEta;
            IntDecorator m_dec_jet_alg;
            IntAccessor m_acc_jet_alg;

    };
}
#endif

