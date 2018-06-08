#include <XAMPPbase/Cuts.h>
#include <XAMPPbase/EventStorage.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <PATInterfaces/SystematicSet.h>
namespace XAMPP {
    StorageKeeper* StorageKeeper::m_Inst = nullptr;
    StorageKeeper* StorageKeeper::GetInstance() {
        if (!m_Inst) m_Inst = new StorageKeeper();
        return m_Inst;
    }
    StorageKeeper::StorageKeeper() :
                m_CommonKeeper(new StorageKeeper::InfoKeeper()),
                m_Keepers(),
                m_Cuts() {
    }
    StorageKeeper::~StorageKeeper() {
        delete m_CommonKeeper;
        for (auto& Keeper : m_Keepers) {
            delete Keeper;
        }
        for (auto& Cut : m_Cuts) {
            if (Cut != nullptr) delete Cut;
        }
        Info("StoreageKeeper()", "Destructor called");
        m_Inst = nullptr;
    }
    StorageKeeper::InfoKeeper* StorageKeeper::FindKeeper(const IEventInfo* Info) const {
        if (Info == nullptr) {
            return m_CommonKeeper;
        }
        for (const auto& InfoKeeper : m_Keepers) {
            if (InfoKeeper->GetInfo() == Info) {
                return InfoKeeper;
            }
        }
        return nullptr;
    }
    bool StorageKeeper::EventStorageExists(const std::string &Name, const IEventInfo* EvInfo) const {
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(EvInfo);
        if (Keeper && Keeper->EventStorageExists(Name)) {
            return true;
        }
        return Keeper != m_CommonKeeper && m_CommonKeeper->EventStorageExists(Name);
    }
    bool StorageKeeper::ParticleDefined(const std::string &Name, const IEventInfo* EvInfo) const {
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(EvInfo);
        if (Keeper && Keeper->ParticleDefined(Name)) {
            return true;
        }
        return Keeper != m_CommonKeeper && m_CommonKeeper->ParticleDefined(Name);
    }
    bool StorageKeeper::Register(IStorage* S, bool IsCommonVariable) {
        if (!S) {
            Error("StorageKeeper::Register()", "No element given");
            return false;
        }
        if (IsCommonVariable) {
            return m_CommonKeeper->Register(S);
        }
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(S->XAMPPInfo());
        if (!Keeper) {
            Info("StorageKeeper::Register()", ("Found new Info object " + S->XAMPPInfo()->name()).c_str());
            m_Keepers.push_back(new StorageKeeper::InfoKeeper(S->XAMPPInfo()));
            Keeper = FindKeeper(S->XAMPPInfo());
        }
        return Keeper->Register(S);
    }
    std::vector<ParticleStorage*> StorageKeeper::GetParticleStores(const IEventInfo* Info) const {
        std::vector<ParticleStorage*> S = m_CommonKeeper->GetParticleStorages();
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(Info);
        if (Keeper) {
            std::vector<ParticleStorage*> S1 = Keeper->GetParticleStorages();
            std::copy(S1.begin(), S1.end(), std::back_inserter(S));
        }
        return S;
    }
    ParticleStorage* StorageKeeper::RetrieveParticleStorage(const std::string &Name, const IEventInfo* Info) const {
        ParticleStorage* S = m_CommonKeeper->GetParticleStorage(Name);
        if (S) {
            return S;
        }
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(Info);
        if (!Keeper) {
            return nullptr;
        }
        return Keeper->GetParticleStorage(Name);
    }

    StorageKeeper::InfoKeeper::InfoKeeper(const IEventInfo* Info) :
                m_RefInfo(Info),
                m_Storages(),
                m_ParticleStores() {
    }
    const IEventInfo* StorageKeeper::InfoKeeper::GetInfo() const {
        return m_RefInfo;
    }
    bool StorageKeeper::InfoKeeper::EventStorageExists(const std::string &Name) const {
        return m_Storages.find(Name) != m_Storages.end();
    }
    bool StorageKeeper::InfoKeeper::ParticleDefined(const std::string &Name) const {
        return m_ParticleStores.find(Name) != m_ParticleStores.end();
    }
    bool StorageKeeper::InfoKeeper::Register(IStorage* Storage) {
        if (!Storage->IsParticleVariable()) {
            if (EventStorageExists(Storage->name())) {
                Error("InfoKeeper::Register()", "The storage %s already exists.", Storage->name().c_str());
                return false;
            }
            m_Storages.insert(std::pair<std::string, IStorage*>(Storage->name(), Storage));
            return true;
        }
        if (ParticleDefined(Storage->name())) {
            Error("InfoKeeper::Register()", "The particle %s is already defined", Storage->name().c_str());
            return false;
        }

        m_ParticleStores.insert(std::pair<std::string, IStorage*>(Storage->name(), Storage));
        return true;
    }
    ParticleStorage* StorageKeeper::InfoKeeper::GetParticleStorage(const std::string& Name) const {
        if (!ParticleDefined(Name)) {
            return nullptr;
        }
        return dynamic_cast<ParticleStorage*>(m_ParticleStores.at(Name));
    }
    std::vector<ParticleStorage*> StorageKeeper::InfoKeeper::GetParticleStorages() const {
        std::vector<ParticleStorage*> S;
        for (auto& StorePairs : m_ParticleStores) {
            S.push_back(dynamic_cast<ParticleStorage*>(StorePairs.second));
        }
        return S;
    }
    StorageKeeper::InfoKeeper::~InfoKeeper() {
        for (auto Store : m_Storages) {
            delete Store.second;
        }
        for (auto Store : m_ParticleStores) {
            delete Store.second;
        }
    }
    void StorageKeeper::AttachCut(Cut* C) {
        if (!IsInVector(C, m_Cuts)) m_Cuts.push_back(C);
    }
    void StorageKeeper::DettachCut(Cut* C) {
        for (auto& defined : m_Cuts) {
            if (C == defined) defined = nullptr;
        }
    }

    //################################################################################################################################
    //                                              IStorage
    //################################################################################################################################
    IStorage::IStorage(const std::string &Name, IEventInfo* Info, bool IsParticleVariable, bool IsCommonVariable) :
                m_Name(Name),
                m_Info(Info),
                m_IsParticleVariable(IsParticleVariable),
                m_Registered(false),
                m_SaveHisto(false),
                m_SaveTree(false),
                m_SaveAlways(true) {
        m_Registered = StorageKeeper::GetInstance()->Register(this, IsCommonVariable);
    }
    IEventInfo* IStorage::XAMPPInfo() const {
        return m_Info;
    }
    std::string IStorage::name() const {
        return m_Name;
    }
    void IStorage::SetSaveTrees(bool B) {
        m_SaveTree = B;
    }
    void IStorage::SetSaveHistos(bool B) {
        m_SaveHisto = B;
    }
    void IStorage::SetSaveVariations(bool B) {
        m_SaveAlways = B;
    }
    bool IStorage::SaveTrees() const {
        return m_SaveTree;
    }
    bool IStorage::SaveHistos() const {
        return m_SaveHisto;
    }
    bool IStorage::SaveVariations() const {
        return m_SaveAlways;
    }
    bool IStorage::IsParticleVariable() const {
        return m_IsParticleVariable;
    }

    bool IStorage::SaveVariable() const {
        return m_SaveTree || m_SaveHisto;
    }
    //################################################################################################################################
    //                                              ParticleStorage
    //################################################################################################################################
    ParticleStorage::ParticleStorage(const std::string &Name, XAMPP::IEventInfo* info) :
                IStorage(Name, info, true),
                m_UseMass(false),
                m_ParticleTrees(),
                m_CurrentTree(nullptr),
                m_CurrentSyst(0),
                m_Cleared(false),
                m_FillTrees(true) {
    }
    StatusCode ParticleStorage::Fill(xAOD::IParticleContainer* Particles) {
        if (!m_FillTrees) return StatusCode::SUCCESS;
        LoadParticleTree(m_Info->GetSystematic());
        if (!Particles) {
            Error("ParticleStorage::Fill()", "No container was given to %s", name().c_str());
            return StatusCode::FAILURE;
        }
        if (!m_CurrentSyst) {
            Error("ParticleStorage::Fill()", "%s has not loaded any systematic", name().c_str());
            return StatusCode::FAILURE;
        }
        if (!m_CurrentTree) {
            Error("ParticleStorage::Fill()", "No %s branches were booked for systematic %s", name().c_str(), m_CurrentSyst->name().c_str());
            return StatusCode::FAILURE;
        }
        m_CurrentTree->SetEventContainer(Particles);
        if (!m_Cleared) {
            m_Vars.clear();
            m_Cleared = true;
        }
        return StatusCode::SUCCESS;
    }
    void ParticleStorage::DisableTree() {
        m_FillTrees = false;
    }
    ParticleStorage::~ParticleStorage() {
    }
    void ParticleStorage::SaveMassInP4(bool B) {
        m_UseMass = B;
    }
    ITreeBranchVariable* ParticleStorage::CreateParticleTree(const CP::SystematicSet* Set, TTree* Tree) {
        if (!Set) {
            Error("ParticleStoarge::CreateParticleTree()", "No systematic set was given to %s", name().c_str());
            return nullptr;
        } else if (!Tree) {
            Error("ParticleStoarge::CreateParticleTree()", "No tree was given for systematic %s to %s", Set->name().c_str(), name().c_str());
            return nullptr;
        } else if (m_ParticleTrees.find(Set) != m_ParticleTrees.end()) {
            Error("ParticleStorage::CreateParticleTree()", "There has already been a Tree defined for systematic %s in %s", Set->name().c_str(), name().c_str());
            return nullptr;
        }
        TreeHelperIParticle* P = new TreeHelperIParticle(this, Tree, m_UseMass);
        m_ParticleTrees.insert(std::pair<const CP::SystematicSet*, TreeHelperIParticle*>(Set, P));
        //Some mechanism to store all the branches;
        for (const auto& Branch : m_Vars) {
            //Skip variables which should only be saved to the nominal tree
            if (!Branch.second.SaveSyst && !Set->name().empty()) continue;
            if (!(P->*Branch.second.DataType)(Branch.first)) {
                delete P;
                return nullptr;
            }
        }
        return P;
    }
    void ParticleStorage::LoadParticleTree(const CP::SystematicSet* Set) {
        if (m_CurrentSyst == Set && m_CurrentTree) return;
        std::map<const CP::SystematicSet*, TreeHelperIParticle*>::const_iterator Itr = m_ParticleTrees.find(Set);
        if (!Set || Itr == m_ParticleTrees.end()) m_CurrentTree = nullptr;
        else m_CurrentTree = Itr->second;
        m_CurrentSyst = Set;
    }
    xAOD::IParticleContainer* ParticleStorage::Container() {
        LoadParticleTree(m_Info->GetSystematic());
        if (!m_CurrentTree) return nullptr;
        return m_CurrentTree->Container();
    }

    StatusCode ParticleStorage::AddVariable(const std::string &Branch, TreeHelperIParticle::StoreVariable V, bool SaveVariations) {
        if (!m_ParticleTrees.empty()) {
            Error("ParticleStorage::AddVariable()", "Storage %s is already locked", name().c_str());
            return StatusCode::FAILURE;
        }
        if (m_Vars.find(Branch) != m_Vars.end()) {
            Error("ParticleStorage::AddVariable()", "Variable %s has already been added for %s", Branch.c_str(), name().c_str());
            return StatusCode::FAILURE;
        }
        m_Vars.insert(std::pair<std::string, AdditionalBranches>(Branch, AdditionalBranches(SaveVariations, V)));
        return StatusCode::SUCCESS;
    }
    StatusCode ParticleStorage::SaveInteger(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<int>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveFloat(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<float>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveDouble(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<double>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveChar(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<char>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveIntegerVector(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<std::vector<int>>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveFloatVector(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<std::vector<float>>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveDoubleVector(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<std::vector<double>>, SaveVariations);
    }
    StatusCode ParticleStorage::SaveCharVector(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<std::vector<char>>, SaveVariations);
    }

}
