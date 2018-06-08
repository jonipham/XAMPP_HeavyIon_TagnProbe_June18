#ifndef XAMPPanalyses_Stop0LAnalysisUtils_H
#define XAMPPanalyses_Stop0LAnalysisUtils_H

#include <XAMPPbase/AnalysisUtils.h>

namespace XAMPP {
    bool isStop(const xAOD::TruthParticle& P);

    // calculate mt2 out of 2 particles and the met, the ParticleMass option overwrites the 'real' mass of P1 and P2 if needed
    float CalculateMT2(const xAOD::IParticle &P1, const xAOD::IParticle &P2, XAMPP::Storage<XAMPPmet>* met, float ParticleMass = -1);
    float CalculateMT2(const xAOD::IParticle &P1, const xAOD::IParticle &P2, const xAOD::MissingET* met, float ParticleMass = -1);



#pragma once
#ifdef DORJIGSAW
#include <memory>
#include <unordered_map>
//For some weird reasons, if SUSYObjDef_xAOD.h is included after the RestFrames stuffs, it won't compile
//#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "RestFrames/LabRecoFrame.hh"
#include "RestFrames/DecayRecoFrame.hh"
#include "RestFrames/VisibleRecoFrame.hh"
#include "RestFrames/InvisibleRecoFrame.hh"
#include "RestFrames/CombinatoricGroup.hh"
#include "RestFrames/InvisibleGroup.hh"
#include "RestFrames/SetMassInvJigsaw.hh"
#include "RestFrames/SetRapidityInvJigsaw.hh"
#include "RestFrames/ContraBoostInvJigsaw.hh"
#include "RestFrames/MinMassesCombJigsaw.hh"
#include <TObjArray.h>
#include <TTree.h>

    class RJigsaw {
        public:
            RJigsaw();
            ~RJigsaw();

            void AnalyzeEvent(const TObjArray& jets, Double_t metx, Double_t mety);

            double MDeltaR;
            bool HLT_j30_xe60_razor170;
            //bool HLT_j30_xe60_razor185;
            //bool HLT_j30_xe60_razor195;

            virtual void ConnectTree(TTree & tree, const std::string& prefix = "") {
                tree.Branch(std::string(prefix + "MDeltaR").c_str(), &MDeltaR);
                tree.Branch(std::string(prefix + "RazorTrig170").c_str(), &HLT_j30_xe60_razor170);
                //tree.Branch(std::string(prefix+"RazorTrig185").c_str(), &HLT_j30_xe60_razor185);
                //tree.Branch(std::string(prefix+"RazorTrig195").c_str(), &HLT_j30_xe60_razor195);
            }

            /// <summary>
            /// Fill the trigger bits
            /// </summary>
            /// <param name="stobjtool">SUSYTools to ask for trigger</param>
            /// <param name="tevent">Use this to retrive HLT jet/met</param>
            void FillTriggerBits(ST::SUSYObjDef_xAOD& stobjtool, xAOD::TEvent& tevent);

        protected:
            void ComputeVariable();
            // Signal-like tree
            std::unique_ptr<RestFrames::LabRecoFrame> LAB_R;
            std::unique_ptr<RestFrames::DecayRecoFrame> GG_R;
            std::unique_ptr<RestFrames::DecayRecoFrame> Ga_R;
            std::unique_ptr<RestFrames::DecayRecoFrame> Gb_R;
            std::unique_ptr<RestFrames::DecayRecoFrame> Ca_R;
            std::unique_ptr<RestFrames::DecayRecoFrame> Cb_R;
            std::unique_ptr<RestFrames::VisibleRecoFrame> V1a_R;
            std::unique_ptr<RestFrames::VisibleRecoFrame> V2a_R;
            std::unique_ptr<RestFrames::InvisibleRecoFrame> Xa_R;
            std::unique_ptr<RestFrames::VisibleRecoFrame> V1b_R;
            std::unique_ptr<RestFrames::VisibleRecoFrame> V2b_R;
            std::unique_ptr<RestFrames::InvisibleRecoFrame> Xb_R;
            std::unique_ptr<RestFrames::InvisibleGroup> INV_R;
            std::unique_ptr<RestFrames::CombinatoricGroup> VIS_R;
            std::unique_ptr<RestFrames::SetMassInvJigsaw> MinMassJigsaw_R;
            std::unique_ptr<RestFrames::SetRapidityInvJigsaw> RapidityJigsaw_R;
            std::unique_ptr<RestFrames::ContraBoostInvJigsaw> ContraBoostJigsaw_R;
            std::unique_ptr<RestFrames::MinMassesCombJigsaw> HemiJigsaw_R;
            std::unique_ptr<RestFrames::MinMassesCombJigsaw> CaHemiJigsaw_R;
            std::unique_ptr<RestFrames::MinMassesCombJigsaw> CbHemiJigsaw_R;
    };

#endif




}

#endif
