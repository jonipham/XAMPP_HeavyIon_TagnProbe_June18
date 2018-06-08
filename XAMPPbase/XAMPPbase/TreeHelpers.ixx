#include<XAMPPbase/TreeHelpers.h>

namespace XAMPP {

    //################################################################################################
    //                              TreeHelper
    //################################################################################################
    template<typename T> bool TreeHelper::AddBranch(const std::string &Name, T &Element) {
        std::string bName = EraseWhiteSpaces(Name);
        if (m_tree->FindBranch(bName.c_str())) {
            Error("TreeHelper::AddBranch()", "The branch %s already exists in TTree %s", Name.c_str(), m_tree->GetName());
            return false;
        }
        if (m_tree->Branch(bName.c_str(), &Element) == nullptr) {
            Error("TreeHelper::AddBranch()", "Could not create the branch %s in TTree %s", Name.c_str(), m_tree->GetName());
            return false;
        }
        return true;
    }
    template<typename T> bool TreeHelperIParticle::StoreDecorator(std::string BranchName) {
        if (IsDecoratorStored(BranchName)) {
            Error("TreeHelperIParticle::StoreDecorator()", "The variable %s is already stored", BranchName.c_str());
            return false;
        }
        m_Branches.push_back(new TreeParticleAccesor<T>(BranchName, m_tree));
        return true;
    }
    //################################################################################################
    //                              EventBranchVariable
    //################################################################################################
    template<class T> EventBranchVariable<T>::EventBranchVariable(XAMPP::Storage<T>* Store, TTree* tree) :
                TreeHelper(tree),
                m_Store(Store) {

    }
    template<class T> bool EventBranchVariable<T>::Init() {
        return AddBranch(m_Store->name(), m_Store->GetValue());
    }
    template<class T> std::string EventBranchVariable<T>::name() const {
        return m_Store->name();
    }
    template<class T> bool EventBranchVariable<T>::Fill() {
        if (!m_Store->isAvailable()) {
            Error("EventBranchVariable::Fill()", "Nothing has been stored in %s", name().c_str());
            return false;
        }
        m_Store->GetValue();
        return true;
    }
    //################################################################################################
    //                              TreeParticleAccesor
    //################################################################################################
    template<class T> TreeParticleAccesor<T>::TreeParticleAccesor(const std::string &Name, TTree* tree) :
                TreeHelper(tree),
                m_name(Name),
                m_Vector(),
                m_Acc(0) {
        m_Acc = new SG::AuxElement::ConstAccessor<T>(Name);

    }
    template<class T> TreeParticleAccesor<T>::~TreeParticleAccesor() {
        delete m_Acc;
        m_Vector.clear();
    }
    template<class T> bool TreeParticleAccesor<T>::PushBack(const xAOD::IParticle* P, bool Resize, size_t Reserve) {
        if (Resize) Clear(Reserve);
        if (!P) {
            Error("TreeParticleConstAccessor::PushBack()", "No particle was given to write branch %s", name().c_str());
            return false;
        }
        if (!m_Acc->isAvailable(*P)) {
            Error("TreeParticleConstAccessor::PushBack()", "The particle does not contain %s", name().c_str());
            return false;
        }
        m_Vector.push_back(m_Acc->operator()(*P));
        return true;
    }
    template<class T> bool TreeParticleAccesor<T>::Init(const std::string &BranchName) {
        if (!AddBranch(BranchName, m_Vector)) {
            Error("TreeParticleConstAccessor::Init()", "Could not setup branches to save %s", name().c_str());
            return false;
        }
        return true;
    }
    template<class T> std::string TreeParticleAccesor<T>::name() const {
        return m_name;
    }
    template<class T> void TreeParticleAccesor<T>::Clear(size_t Reserve) {
        m_Vector.clear();
        if (Reserve > m_Vector.capacity()) m_Vector.reserve(Reserve);
    }

}

