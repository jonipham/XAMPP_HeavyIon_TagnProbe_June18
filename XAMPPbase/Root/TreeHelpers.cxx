#include <XAMPPbase/TreeHelpers.h>
#include <XAMPPbase/EventStorage.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/EventInfo.h>
namespace XAMPP {
    TreeHelper::TreeHelper(TTree* tree) :
                m_tree(tree) {
    }
    std::string TreeHelper::EraseWhiteSpaces(const std::string &In) {
        std::string out = In;
        out.erase(std::remove_if(out.begin(), out.end(), isspace), out.end());
        return out;
    }
    //################################################################################################################################
    //                                              ITreeParticleFourMomentConstAccessor
    //################################################################################################################################
    bool ITreeParticleFourMomentConstAccessor::PushBack(const xAOD::IParticle* P, bool Resize, size_t Reserve) {
        if (Resize) Clear(Reserve);
        if (!P) return false;
        m_Vector.push_back((P->*m_Momentum)());
        return true;
    }
    void ITreeParticleFourMomentConstAccessor::Clear(size_t Reserve) {
        m_Vector.clear();
        if (Reserve > m_Vector.capacity()) m_Vector.reserve(Reserve);
    }
    bool ITreeParticleFourMomentConstAccessor::Init(const std::string &BranchName) {
        return AddBranch(BranchName, m_Vector);
    }
    std::string ITreeParticleFourMomentConstAccessor::name() const {
    return FourMomentum(m_Momentum);
    }
    ITreeParticleFourMomentConstAccessor::~ITreeParticleFourMomentConstAccessor() {
        m_Vector.clear();
    }
    ITreeParticleFourMomentConstAccessor::ITreeParticleFourMomentConstAccessor(TTree* tree, Momentum Mom) :
                TreeHelper(tree),
                m_Momentum(Mom),
                m_Vector() {

    }
    //################################################################################################################################
    //                                              TreeHelperMet
    //################################################################################################################################
    TreeHelperMet::TreeHelperMet(XAMPP::Storage<XAMPPmet>* met, TTree* tree) :
                TreeHelper(tree),
                m_store(met),
                m_met(),
                m_phi(),
                m_sumet() {
    }
    bool TreeHelperMet::Init() {
        if (!m_store) {
            Error("TreeHelperMet::initialize()", "The MET does not exist");
            return false;
        }
        if (!AddBranch(m_store->name() + "_met", m_met)) return false;
        if (!AddBranch(m_store->name() + "_phi", m_phi)) return false;
        if (!AddBranch(m_store->name() + "_sumet", m_sumet)) return false;
        return true;
    }
    bool TreeHelperMet::Fill() {
        if (m_store->GetValue()) {
            m_met = m_store->GetValue()->met();
            m_phi = m_store->GetValue()->phi();
            m_sumet = m_store->GetValue()->sumet();
        } else {
            m_met = -FLT_MAX;
            m_phi = -FLT_MAX;
            m_sumet = -FLT_MAX;
        }
        return true;
    }
    std::string TreeHelperMet::name() const {
        return m_store->name();
    }

    //################################################################################################################################
    //                                              TreeHelperIParticle
    //################################################################################################################################
    TreeHelperIParticle::TreeHelperIParticle(ParticleStorage* PStore, TTree* tree, bool UseMass) :
                TreeHelper(tree),
                m_Store(PStore),
                m_init(false),
                m_dumped(true),
                m_Branches(),
                m_Container(nullptr) {
        m_Branches.push_back(new ITreeParticleFourMomentConstAccessor(tree, &xAOD::IParticle::pt));
        m_Branches.push_back(new ITreeParticleFourMomentConstAccessor(tree, &xAOD::IParticle::eta));
        m_Branches.push_back(new ITreeParticleFourMomentConstAccessor(tree, &xAOD::IParticle::phi));
        if (UseMass) m_Branches.push_back(new ITreeParticleFourMomentConstAccessor(tree, &xAOD::IParticle::m));
        else m_Branches.push_back(new ITreeParticleFourMomentConstAccessor(tree, &xAOD::IParticle::e));
    }
    void TreeHelperIParticle::SetEventContainer(xAOD::IParticleContainer* Cont) {
        if (m_init) m_Container = Cont;
        m_dumped = false;
    }
    bool TreeHelperIParticle::Fill() {
        if (!Update(Container())) return false;
        m_dumped =true;
        return true;
    }
    xAOD::IParticleContainer* TreeHelperIParticle::Container() const {
        return m_Container;
    }
    bool TreeHelperIParticle::Init() {
        if (m_init) {
            Error("TreeHelperIParticle::Init()", "%s is already initialized", name().c_str());
            return false;
        }
        if (!m_tree) {
            Error("TreeHelperIParticle::Init()", "There is no tree to initialize");
            return false;
        }
        for (const auto & B : m_Branches) {
            if (!B->Init(m_Store->name() + "_" + B->name())) return false;
        }
        m_init = true;
        return true;
    }
    bool TreeHelperIParticle::Update(xAOD::IParticleContainer* Cont) {
        if (!m_init) {
            Error("TreeHelperIParticle::Update()", "%s is not initialized", name().c_str());
            return false;
        }
        if (m_dumped) {
            Error("TreeHelperIParticle::Update()", "No Container was given to %s", name().c_str());
            return false;
        }        
        if (!Cont->empty()) {
            bool Clear = true;
            size_t ContSize = Cont->size();
            for (const auto& P : *Cont) {
                if (!PushBack(P, Clear, ContSize)) return false;
                Clear = false;
            }
        } else {
            for (auto& B : m_Branches)
                B->Clear();
        }
        return true;
    }
    bool TreeHelperIParticle::PushBack(const xAOD::IParticle* P, bool Clear, size_t &Reserve) {
        for (auto& B : m_Branches) {
            if (!B->PushBack(P, Clear, Reserve)) {
                Error("TreeHelperIParticle::PushBack()", "Could not Save the particle:");
                PromptParticle(P);
                return false;
            }
        }
        return true;
    }
    TreeHelperIParticle::~TreeHelperIParticle() {
        for (auto B : m_Branches)
            delete B;
    }
    bool TreeHelperIParticle::IsDecoratorStored(const std::string &Name) {
        for (const auto& Dec : m_Branches)
            if (Name == Dec->name()) return true;
        return false;
    }
    std::string TreeHelperIParticle::name() const {
        return m_Store->name();
    }
}

