#ifndef XAMPPBASE_EventStorage_IXX 
#define XAMPPBASE_EventStorage_IXX

#include <XAMPPbase/EventStorage.h>

namespace XAMPP {

    template<class T> Storage<T>* StorageKeeper::RetrieveEventStorage(const std::string &Name, const IEventInfo* EvInfo) const {
        Storage<T>* S = m_CommonKeeper->RetrieveEventStorage<T>(Name);
        if (S) {
            return S;
        }
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(EvInfo);
        if (Keeper) {
            return Keeper->RetrieveEventStorage<T>(Name);
        }
        return NULL;
    }
    template<class T> std::vector<Storage<T>*> StorageKeeper::GetEventStorages(const IEventInfo* EvInfo) const {
        std::vector<Storage<T>*> S = m_CommonKeeper->GetEventStorages<T>();
        StorageKeeper::InfoKeeper* Keeper = FindKeeper(EvInfo);
        if (Keeper) {
            std::vector<Storage<T>*> S1 = Keeper->GetEventStorages<T>();
            for (auto& Element : S1) {
                S.push_back(Element);
            }
        }
        return S;
    }
    template<class T> Storage<T>* StorageKeeper::InfoKeeper::RetrieveEventStorage(const std::string &Name) const {
        if (!EventStorageExists(Name)) {
            return NULL;
        }
        return dynamic_cast<Storage<T>*>(m_Storages.at(Name));
    }

    template<class T> std::vector<Storage<T>*> StorageKeeper::InfoKeeper::GetEventStorages() const {
        std::vector<Storage<T>*> Stores;
        for (auto& Element : m_Storages) {
            Storage<T>* Store = dynamic_cast<Storage<T>*>(Element.second);
            if (Store) Stores.push_back(Store);
        }
        return Stores;
    }

    //################################################################################################
    //                              Storage
    //################################################################################################
    template<class T> Storage<T>::Storage(const std::string &Name, IEventInfo* Info, bool IsCommon) :
                IStorage(Name, Info, false, IsCommon),
                m_Variable(),
                m_Decorator(Name),
                m_ConstAccessor(Name) {
    }
    template<class T> StatusCode Storage<T>::Store(T Value) {
        if (!m_Registered || !m_Info || !m_Info->GetEventInfo()) {
            return StatusCode::FAILURE;
        }
        m_Decorator(*m_Info->GetEventInfo()) = Value;
        return StatusCode::SUCCESS;
    }
    template<class T> T& Storage<T>::GetValue() {
        if (isAvailable()) {
            if (m_Info->GetEventInfo()) m_Variable = m_ConstAccessor(*m_Info->GetEventInfo());
            else if (m_Info->GetOrigInfo()) m_Variable = m_ConstAccessor(*m_Info->GetOrigInfo());
        }
        return m_Variable;
    }
    template<class T> StatusCode Storage<T>::ConstStore(T Value) {
        if (!m_Registered || !m_Info->GetOrigInfo() || m_Info->GetEventInfo()) {
            return StatusCode::FAILURE;
        }
        m_Decorator(*m_Info->GetOrigInfo()) = Value;
        return StatusCode::SUCCESS;
    }
    template<class T> Storage<T>::~Storage() {
    }
    template<class T> bool Storage<T>::isAvailable() const {
        if (!m_Registered || !m_Info) {
            return false;
        }
        if (m_Info->GetEventInfo()) return m_ConstAccessor.isAvailable(*m_Info->GetEventInfo());
        if (m_Info->GetOrigInfo()) return m_ConstAccessor.isAvailable(*m_Info->GetOrigInfo());
        return false;
    }
    //################################################################################################
    //                              ParticleStorage
    //################################################################################################
    template<typename T> StatusCode ParticleStorage::SaveVariable(const std::string &Branch, bool SaveVariations) {
        return AddVariable(Branch, &TreeHelperIParticle::StoreDecorator<T>, SaveVariations);
    }

}
#endif
