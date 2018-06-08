#include <XAMPPbase/AnalysisUtils.h>

#include <XAMPPbase/AnalysisConfig.h>
#include <XAMPPbase/ISystematics.h>

#include <XAMPPbase/EventInfo.h>

#include <XAMPPbase/HistoBase.h>
#include <XAMPPbase/HistoHelpers.h>
#include <iostream>
#include <vector>

#ifndef XAOD_STANDALONE
#include <GaudiKernel/ITHistSvc.h>
#endif

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
namespace XAMPP {
    HistoBase::HistoBase(const CP::SystematicSet * set) :
                m_set(set),
                m_XAMPPInfo(0),
                m_systematics(0),
                m_config(""),
#ifdef XAOD_STANDALONE
                m_outFile(0),
#else
                m_histSvc("THistSvc", RemoveAllExpInStr("HistoBase" + set->name(), "_")),
#endif

                m_init(false),
                m_doHistos(false),
                m_WriteCutFlow(true),
                m_CurrentWeight(1.),
                m_CurrentWeight2(1.),
                m_EventWeights(),
                m_Variables(),
                m_CutFlowHistos(),
                m_CutFlows(),
                m_EvNumber(-1) {
    }
    HistoBase::~HistoBase() {
        for (auto& Var : m_Variables) {
            delete Var;
        }
        for (auto& Var : m_CutFlowHistos) {
            delete Var;
        }
    }
    void HistoBase::SetAnalysisConfig(const ToolHandle<XAMPP::IAnalysisConfig>&config) {
        if (!m_init) m_config = config;
    }
    void HistoBase::SetEventInfoHandler(const XAMPP::EventInfo* Info) {
        if (!m_init) m_XAMPPInfo = Info;
    }
    void HistoBase::SetEventInfoHandler(const ToolHandle<XAMPP::IEventInfo>& EvInfo) {
        SetEventInfoHandler(dynamic_cast<const XAMPP::EventInfo*>(EvInfo.operator->()));
    }
    void HistoBase::SetSystematicsTool(const ToolHandle<XAMPP::ISystematics>& Syst) {
        if (!m_init) m_systematics = Syst.operator->();
    }
    void HistoBase::UpdateWeight() {
        if (isData()) return;
        if (m_EvNumber == m_XAMPPInfo->eventNumber()) {
            return;
        }
        m_EvNumber = m_XAMPPInfo->eventNumber();
        m_CurrentWeight = 1.;
        for (auto& Storage : m_EventWeights) {
            m_CurrentWeight *= Storage->GetValue();
        }
        m_CurrentWeight2 = m_CurrentWeight * m_CurrentWeight;
    }
    bool HistoBase::isData() const {
        if (!m_init) {
            Error("HistoBase::isData()", "The tool is not initialized yet");
            return false;
        }
        return m_systematics->isData();
    }
    unsigned int HistoBase::Identifier() const {
        if (!m_init) {
            Error("HistoBase::Identifier()", "The tool is not initialized yet");
            return -1;
        }
        if (!isData()) {
            return m_XAMPPInfo->mcChannelNumber();
        }
        return m_XAMPPInfo->runNumber();
    }
    double HistoBase::GetWeight() const {
        return m_CurrentWeight;
    }
    double HistoBase::GetWeight2() const {
        return m_CurrentWeight2;
    }
    bool HistoBase::HistoInOutput(TH1* Histo, const std::string &FullPath) {
        if (!Histo) {
            Error("HistoBase::HistoInOutput()", "No histo given");
            return false;
        }
        if (FullPath.empty() || FullPath.rfind("/") == FullPath.size() - 1) {
            Error("HistoBase::HistoInOutput()", "Empty names are not allowed");
            return false;
        }
        std::string Name = FullPath.rfind("/") != std::string::npos ? FullPath.substr(FullPath.rfind("/") + 1) : FullPath;
#ifdef XAOD_STANDALONE
        m_outFile->cd();
        TDirectory* Dir = FindDirectory(FullPath, m_outFile->GetDirectory(""));
        if (Dir->Get(Name.c_str())) {
            Error("HistoBase::HistoInOutput()", "There already exists an object %s", Name.c_str());
            return false;
        } else {
            Histo->SetName(Name.c_str());
            Histo->SetDirectory(Dir);
        }
#else
        Histo->SetName(Name.c_str());
        return m_histSvc->regHist("/XAMPP/" + FullPath, Histo).isSuccess();
#endif

        return true;
    }
#ifdef XAOD_STANDALONE
    TDirectory* HistoBase::FindDirectory(const std::string &FullPath, TDirectory* In) {
        In->cd();
        if (FullPath.find("/") != std::string::npos) {
            std::string Dir = FullPath.substr(0, FullPath.find("/"));
            if (!In->GetDirectory(Dir.c_str())) {
                In->mkdir(Dir.c_str());
            }
            return FindDirectory(FullPath.substr(FullPath.find("/")+1), In->GetDirectory(Dir.c_str()));
        }
        return In;
    }
#endif
    std::string HistoBase::name() const {
        if (m_set->name().empty()) return "Nominal";
        return m_set->name();
    }
    StatusCode HistoBase::InitializeHistos() {
        if (m_init) {
            Error("HistoBase::InitializeHistos()", "The HistoBase is already initialized");
            return StatusCode::FAILURE;
        }
        m_init = true;
        if (!m_set) {
            Error("HistoBase::InitializeHistos()", "Invalid systematic");
            return StatusCode::FAILURE;
        }
        if (!m_XAMPPInfo) {
            Error("HistoBase::InitializeHistos()", "No XAMPPInfo object could be found");
            return StatusCode::FAILURE;
        }
        if (!m_systematics) {
            Error("HistoBase::InitializeHistos()", "Could not retrieve the systematics tool");
            return StatusCode::FAILURE;
        }
#ifdef XAOD_STANDALONE
        if (!m_outFile || !m_outFile->IsOpen()) {
            Error("HistoBase::InitializeHistos()", "No valid output file");
            return StatusCode::FAILURE;
        }
#endif
        if (!m_config->RegisterHistoBase(this)) {
            Error("HistoBase::InitializeHistos()", " Could not register to the AnalysisConfig %s", m_config->name().c_str());
            return StatusCode::FAILURE;
        }
        //Used Weights
        std::vector<std::string> Weights = {"GenWeight", "EleWeight", "MuoWeight", "PhoWeight", "TauWeight", "JetWeight", "muWeight"};
        for (auto&w : Weights) {
            AppendWeight(w);
        }
        if (!m_doHistos) {
            Info("HistoBase::InitializeHistos()", "Histo writting disabled");
            return StatusCode::SUCCESS;
        }
        return StatusCode::SUCCESS;
    }
    bool HistoBase::FinalizeHistos() {
        for (auto& Hist : m_Variables)
            if (!Hist->Finalize()) return false;
        for (auto& Hist : m_CutFlowHistos)
            if (!Hist->Finalize()) return false;
        return true;
    }
    StatusCode HistoBase::FillHistos() {
        UpdateWeight();

        return StatusCode::SUCCESS;
    }

    void HistoBase::AppendWeight(const std::string &Name) {
        if (isData() || !m_XAMPPInfo->DoesVariableExist<double>(Name)) {
            return;
        }
        XAMPP::Storage<double>* Storage = m_XAMPPInfo->GetVariableStorage<double>(Name);
        if (!IsInVector(Storage, m_EventWeights)) {
            m_EventWeights.push_back(Storage);
        }
    }
    const CP::SystematicSet* HistoBase::SystSet() const {
        return m_set;
    }
    bool HistoBase::RegisterCutFlow(const CutFlow& Flow) {
        if (IsInVector(&Flow, m_CutFlows)) {
            return false;
        }
        m_CutFlows.push_back(&Flow);
        return true;
    }
    CutFlowHistoVariable* HistoBase::CreateCutFlowHisto(const CutFlow& Flow) {
        if (!m_WriteCutFlow) {
            return NULL;
        }
        CutFlowHistoVariable* H = new CutFlowHistoVariable(this, &Flow);
        m_CutFlowHistos.push_back(H);
        return H;
    }

}
