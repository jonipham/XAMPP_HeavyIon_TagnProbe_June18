#include <XAMPPbase/TreeBase.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/TreeHelpers.h>

#include <XAMPPbase/AnalysisConfig.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/ISystematics.h>

#include <iostream>
#include <PATInterfaces/SystematicSet.h>

#ifndef XAOD_STANDALONE
#include <GaudiKernel/ITHistSvc.h>
#endif

namespace XAMPP {

    TreeBase::TreeBase(const CP::SystematicSet* set) :
                m_set(set),
                m_XAMPPInfo(0),
                m_systematics(0),
                m_config(0),
                m_tree(0),
                m_init(false),
                m_isWritten(false),
#ifdef XAOD_STANDALONE
                m_outFile(0),
#else
                m_histSvc("THistSvc", RemoveAllExpInStr("TreeBase" + set->name(), "_")),
#endif
                m_Branches(),
                m_mcChannelNumber(-1) {
    }

    TreeBase::~TreeBase() {
        for (auto B : m_Branches)
            delete B;
#ifndef XAOD_STANDALONE
        if (m_tree) {
            delete m_tree;
        }
#endif
    }
    void TreeBase::SetAnalysisConfig(const ToolHandle<XAMPP::IAnalysisConfig>&config) {
        if (!m_init) m_config = config.operator->();
    }

    void TreeBase::SetEventInfoHandler(const XAMPP::EventInfo* Info) {
        if (!m_init) m_XAMPPInfo = Info;
    }
    void TreeBase::SetEventInfoHandler(const ToolHandle<XAMPP::IEventInfo>& EvInfo) {
        SetEventInfoHandler(dynamic_cast<const XAMPP::EventInfo*>(EvInfo.operator->()));
    }
    void TreeBase::SetSystematicsTool(const ToolHandle<XAMPP::ISystematics>& Syst) {
        if (!m_init) m_systematics = Syst.operator->();
    }

    StatusCode TreeBase::InitializeTree() {
        if (!m_set) {
            Error("TreeBase::InitializeTree()", "Invalid systematic");
            return StatusCode::FAILURE;
        }
        if (m_init) {
            Error("TreeBase::InitializeTree()", "The TreeBase is already intialized for systematic %s", m_set->name().c_str());
            return StatusCode::FAILURE;
        }
        if (!m_XAMPPInfo) {
            Error("TreeBase::InitializeTree()", "No XAMPPInfo object could be found");
            return StatusCode::FAILURE;
        }
        if (!m_systematics) {
            Error("TreeBase::InitializeTree()", "Could not retrieve the systematics tool");
            return StatusCode::FAILURE;
        }
        if (m_set != m_systematics->GetNominal()) Info("TreeBase::InitiailzeTree()", "The current tree writes kinematic systematics %s. Variations of the event weights will not be stored", m_set->name().c_str());
        if (!m_config) {
            Error("TreeBase::InitiailzeTree()", "Unable to get the associated AnalysisConfig class");
            return StatusCode::FAILURE;
        }
        std::string m_systN = m_set->name();
        if (m_set == m_systematics->GetNominal()) {
            m_systN = "Nominal";
        }
#ifdef XAOD_STANDALONE
        if (!m_outFile || !m_outFile->IsOpen()) {
            Error("TreeBase::InitializeTree()","Cannot get output file");
            return StatusCode::FAILURE;
        }
        m_outFile->cd();
#endif
        m_tree = new TTree((m_config->TreeName() + "_" + m_systN).c_str(), "SmallTree for fast analysis");
        m_tree->SetAutoFlush(0);
        m_tree->SetAutoSave(0);
#ifndef XAOD_STANDALONE
        if (!m_histSvc->regTree(Form("/XAMPP/%s_%s", m_config->TreeName().c_str(), m_systN.c_str()), m_tree).isSuccess()) {
            return StatusCode::FAILURE;
        }
#endif
        if (!m_systematics->isData()) m_tree->Branch("mcChannelNumber", &m_mcChannelNumber);

        CreateBranches();
        std::vector<XAMPP::Storage<XAMPPmet>*> MetStorageVector = m_XAMPPInfo->GetStorages<XAMPPmet>(XAMPP::EventInfo::OutputElement::Tree, m_set != m_systematics->GetNominal());
        for (auto& entry : MetStorageVector) {
            m_Branches.push_back(new TreeHelperMet(entry, m_tree));
        }
        std::vector<ParticleStorage*> Particles = m_XAMPPInfo->GetParticleStorages(XAMPP::EventInfo::OutputElement::Tree, m_set != m_systematics->GetNominal());
        for (const auto& P : Particles) {
            XAMPP::ITreeBranchVariable* B = P->CreateParticleTree(m_set, m_tree);
            if (!B) return StatusCode::FAILURE;
            m_Branches.push_back(B);
        }

        for (const auto & B : m_Branches) {
            //Info("TreeBase::FillTree()", "Try to initialize %s", B->name().c_str());
            if (!B->Init()) {
                Error("TreeBase::InitiailzeTree()", "Could not setup branch %s", B->name().c_str());
                return StatusCode::FAILURE;
            }
        }
        m_isWritten = false;
        return StatusCode::SUCCESS;
    }
    void TreeBase::CreateBranches() {
        CreateEventBranches<double>();
        CreateEventBranches<float>();
        CreateEventBranches<int>();
        CreateEventBranches<char>();
        CreateEventBranches<bool>();
        CreateEventBranches<unsigned int>();
        CreateEventBranches<unsigned long long>();
    }
    StatusCode TreeBase::FillTree() {
        if (!m_systematics->isData()) m_mcChannelNumber = m_XAMPPInfo->mcChannelNumber();
        for (auto B : m_Branches) {
            //Info("TreeBase::FillTree()", "Try to fill %s", B->name().c_str());
            if (!B->Fill()) {
                Error("TreeBase::FillTree()", "The branch %s was not updated since the last call of ComputeEventVariables().", B->name().c_str());
                return StatusCode::FAILURE;
            }
            //Info("TreeBase::FillTree()", "Successfully filled %s", B->name().c_str());

        }
        if (m_tree->Fill() < 0) {
            Error("TreeBase::FillTree()", "Failed to fill the tree");
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    StatusCode TreeBase::FinalizeTree() {
        if (m_isWritten) {
            return StatusCode::SUCCESS;
        }
        m_isWritten = true;
#ifdef XAOD_STANDALONE
        if (!m_outFile) {
            Error("TreeBase::FinalizeTree()","Something wrong: cannot find outFile to fill Out Tree");
            return StatusCode::FAILURE;
        }
        m_outFile->cd();
        m_tree->Write("", TObject::kOverwrite);
#endif
        Info("TreeBase::FinalizeTree()", "Written successfully %s (%s), containing %llu entries", m_tree->GetName(), m_config->TreeName().c_str(), m_tree->GetEntries());
        m_tree = NULL;
        return StatusCode::SUCCESS;
    }

#ifdef XAOD_STANDALONE
    void TreeBase::SetOutFile(TFile* file) {
        if (!m_init) m_outFile = file;
    }
#else
    void TreeBase::SetHistService(ServiceHandle<ITHistSvc> & Handle) {
        if (!m_init) m_histSvc = Handle;
    }
#endif
}
