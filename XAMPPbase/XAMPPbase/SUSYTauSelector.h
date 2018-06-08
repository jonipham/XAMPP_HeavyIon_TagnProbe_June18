#ifndef XAMPPbase_SUSYTauSelector_H
#define XAMPPbase_SUSYTauSelector_H

#include <XAMPPbase/ITauSelector.h>
#include <XAMPPbase/SUSYParticleSelector.h>
namespace TauAnalysisTools {
    class ITauTruthMatchingTool;
    class ITauSelectionTool;
}
namespace XAMPP {
    struct TauWeightDecorators {
            const CP::SystematicSet* Systematic;

            SG::AuxElement::Decorator<double>* TauTrigSF;
            SG::AuxElement::Decorator<double>* TaudIdSF;
            SG::AuxElement::Decorator<double>* TauSF;

            XAMPP::Storage<double>* TrigSF;
            XAMPP::Storage<double>* IDSF;
            XAMPP::Storage<double>* TotalSF;
    };

    class SUSYTauSelector: public SUSYParticleSelector, virtual public ITauSelector {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYTauSelector, XAMPP::ITauSelector)

            SUSYTauSelector(std::string myname);
            virtual ~SUSYTauSelector();

            virtual StatusCode initialize();

            virtual StatusCode LoadContainers();
            virtual StatusCode InitialFill(const CP::SystematicSet &systset);
            virtual StatusCode FillTaus(const CP::SystematicSet &systset);

            virtual TauLink GetLink(const xAOD::TauJet &tau) const;
            virtual TauLink GetOrigLink(const xAOD::TauJet &tau) const;

            virtual const xAOD::TauJetContainer* GetTauContainer() const;

            virtual xAOD::TauJetContainer* GetTaus() const;
            virtual xAOD::TauJetContainer* GetPreTaus() const;
            virtual xAOD::TauJetContainer* GetSignalTaus() const;
            virtual xAOD::TauJetContainer* GetBaselineTaus() const;
            virtual xAOD::TauJetContainer* GetCustomTaus(const std::string &kind) const;

            virtual const xAOD::DiTauJetContainer* GetDiTauContainer() const;
            virtual xAOD::DiTauJetContainer* GetDiTaus() const;
            virtual xAOD::DiTauJetContainer* GetPreDiTaus() const;
            virtual xAOD::DiTauJetContainer* GetSignalDiTaus() const;
            virtual xAOD::DiTauJetContainer* GetBaselineDiTaus() const;
            virtual xAOD::DiTauJetContainer* GetCustomDiTaus(const std::string &kind) const;

            virtual StatusCode SaveScaleFactor();
        protected:

            virtual StatusCode CallSUSYTools();
            StatusCode StoreTruthClassifer(xAOD::TauJet &tau);

            const xAOD::TauJetContainer* m_xAODTaus;
            xAOD::TauJetContainer* m_Taus;
            xAOD::ShallowAuxContainer* m_TausAux;

            xAOD::TauJetContainer * m_PreTaus; // before OR
            xAOD::TauJetContainer * m_BaselineTaus; // after OR
            xAOD::TauJetContainer * m_SignalTaus;

            const xAOD::DiTauJetContainer* m_xAODDiTaus;
            xAOD::DiTauJetContainer* m_DiTaus;

            xAOD::DiTauJetContainer * m_PreDiTaus; // before OR
            xAOD::DiTauJetContainer * m_BaselineDiTaus; // after OR
            xAOD::DiTauJetContainer * m_SignalDiTaus;

            asg::AnaToolHandle<TauAnalysisTools::ITauTruthMatchingTool> m_TruthMatching;

        private:
            std::vector<TauWeightDecorators> m_SF;
            ToolHandle<TauAnalysisTools::ITauSelectionTool> m_BaseTauSelectionTool;
            ToolHandle<TauAnalysisTools::ITauSelectionTool> m_SignalTauSelectionTool;

            bool m_SeparateSF;
            bool m_doIdSF;
            bool m_doTrigSF;
            std::string m_TriggerExp;

            float m_minBaselineJetBDT;
            float m_maxBaselineJetBDT;

            bool m_ignoreBaseTauIDTool;
    };
}
#endif
