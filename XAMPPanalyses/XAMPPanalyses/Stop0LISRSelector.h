#ifndef XAMPPanalyses_Stop0LISRSelector_H
#define XAMPPanalyses_Stop0LISRSelector_H

#include <AsgTools/AsgTool.h>
#include <AsgTools/AnaToolHandle.h>

#include <AsgTools/ToolHandle.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/EventStorage.h>
#include <XAMPPbase/AnalysisUtils.h>

#if ROOTCORE_RELEASE_SERIES == 24
    #include <RootCore/Packages.h>
    #ifdef ROOTCORE_PACKAGE_Ext_RestFrames     
        #include "RestFrames/RestFrames.hh"
    #endif
#endif


namespace XAMPP {
    class IISRSelector: virtual public asg::IAsgTool {
        public:
            ASG_TOOL_INTERFACE (IISRSelector)
            virtual StatusCode initializeISREventVariables(bool isTruth = false)=0;
            virtual StatusCode ComputeISREventVariables(xAOD::JetContainer* Jets, xAOD::IParticleContainer* Electrons, xAOD::IParticleContainer* Muons, XAMPP::Storage<XAMPPmet>* Met)=0;
            virtual ~IISRSelector() {
            }

    };

    class Stop0LISRSelector: public asg::AsgTool , virtual public IISRSelector{

        public:

            ASG_TOOL_CLASS(Stop0LISRSelector, XAMPP::IISRSelector)
            Stop0LISRSelector(std::string myname);
            virtual ~Stop0LISRSelector();
            virtual StatusCode initialize();
            virtual StatusCode initializeISREventVariables(bool isTruth = false);
            virtual StatusCode ComputeISREventVariables(xAOD::JetContainer* Jets, xAOD::IParticleContainer* Electrons, xAOD::IParticleContainer* Muons, XAMPP::Storage<XAMPPmet>* Met);
        protected:
            asg::AnaToolHandle<XAMPP::IEventInfo> m_InfoHandle;
            XAMPP::EventInfo* m_XAMPPInfo;
            XAMPP::Storage<float>* m_dec_PTISR;
            XAMPP::Storage<float>* m_dec_RISR;
            XAMPP::Storage<float>* m_dec_cosS;
            XAMPP::Storage<float>* m_dec_MS;
            XAMPP::Storage<float>* m_dec_MISR;
            XAMPP::Storage<float>* m_dec_MV;
            XAMPP::Storage<float>* m_dec_dphiCMI;
            XAMPP::Storage<float>* m_dec_dphiISRI;
            XAMPP::Storage<int>* m_dec_NbV;
            XAMPP::Storage<int>* m_dec_NbISR;
            XAMPP::Storage<int>* m_dec_NjV;
            XAMPP::Storage<int>* m_dec_NjISR;
            XAMPP::Storage<int>* m_dec_NlV;
            XAMPP::Storage<int>* m_dec_NlISR;

            XAMPP::Storage<float>* m_dec_PTCM;

            XAMPP::Storage<float>* m_dec_MW1;
            XAMPP::Storage<float>* m_dec_MW2;
            XAMPP::Storage<float>* m_dec_dphiCML1;
            XAMPP::Storage<float>* m_dec_dphiCML2;
            XAMPP::Storage<float>* m_dec_dphiSL1;
            XAMPP::Storage<float>* m_dec_dphiSL2;
            XAMPP::Storage<float>* m_dec_cosIL1;
            XAMPP::Storage<float>* m_dec_cosIL2;
            XAMPP::Storage<float>* m_dec_dRMinbl1;
            XAMPP::Storage<float>* m_dec_dRMinbl2;
            XAMPP::Storage<float>* m_dec_dphiMinbl1;
            XAMPP::Storage<float>* m_dec_dphiMinbl2;

            XAMPP::Storage<float>* m_dec_Vbjet1Pt;
            XAMPP::Storage<float>* m_dec_Vjet4Pt;

#ifdef XAOD_STANDALONE
#ifdef ROOTCORE_PACKAGE_Ext_RestFrames
            RestFrames::LabRecoFrame* LAB;
            RestFrames::DecayRecoFrame* CM;
            RestFrames::DecayRecoFrame* S;
            RestFrames::VisibleRecoFrame* ISR;
            RestFrames::VisibleRecoFrame* V;
            RestFrames::InvisibleRecoFrame* I;
            RestFrames::InvisibleGroup* INV;
            RestFrames::SetMassInvJigsaw* InvMass;
            RestFrames::CombinatoricGroup* VIS;
            RestFrames::MinMassesCombJigsaw* SplitVis;
#endif
#endif

    };
}

#endif

