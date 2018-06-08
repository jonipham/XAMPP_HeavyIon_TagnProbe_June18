#ifndef XAMPPbase_EventStorage_H
#define XAMPPbase_EventStorage_H

#include <AsgTools/IAsgTool.h>
#include <xAODEventInfo/EventInfo.h>

#include <XAMPPbase/IEventInfo.h>
#include <XAMPPbase/TreeHelpers.h>
#include <XAMPPbase/IEventInfo.h>

#include <map>
#include <string>
#include <vector>
#include <iostream>

namespace CP {
    class SystematicSet;
}
namespace XAMPP {
    class IStorage;
    class ParticleStorage;
    class IEventInfo;
    template<class T> class Storage;
    class Cut;
    class StorageKeeper {
        public:
            static StorageKeeper* GetInstance();
            bool Register(IStorage* S, bool IsCommon = false);

            bool EventStorageExists(const std::string &Name, const IEventInfo* EvInfo = 0) const;
            bool ParticleDefined(const std::string &Name, const IEventInfo* EvInfo = 0) const;

            template<class T> Storage<T>* RetrieveEventStorage(const std::string &Name, const IEventInfo* EvInfo = 0) const;

            template<class T> std::vector<Storage<T>*> GetEventStorages(const IEventInfo* EvInfo = 0) const;

            std::vector<ParticleStorage*> GetParticleStores(const IEventInfo* Info = 0) const;
            ParticleStorage* RetrieveParticleStorage(const std::string &Name, const IEventInfo* Info = 0) const;

            ~StorageKeeper();
            //Let's attach the cuts to the keeper just for deletion
            void AttachCut(Cut* C);
            void DettachCut(Cut* C);

        protected:
            static StorageKeeper* m_Inst;

            StorageKeeper();
            StorageKeeper(const StorageKeeper &) = delete;
            void operator =(const StorageKeeper &) = delete;

            class InfoKeeper {
                public:
                    InfoKeeper(const IEventInfo* = 0);
                    const IEventInfo* GetInfo() const;
                    template<class T> Storage<T>* RetrieveEventStorage(const std::string& Name) const;
                    template<class T> std::vector<Storage<T>*> GetEventStorages() const;
                    bool EventStorageExists(const std::string &Name) const;

                    ParticleStorage* GetParticleStorage(const std::string& Name) const;
                    std::vector<ParticleStorage*> GetParticleStorages() const;
                    bool ParticleDefined(const std::string &Name) const;

                    bool Register(IStorage* Storage);
                    ~InfoKeeper();

                private:
                    const IEventInfo* m_RefInfo;
                    std::map<std::string, IStorage*> m_Storages;
                    std::map<std::string, IStorage*> m_ParticleStores;

            };
            InfoKeeper* FindKeeper(const IEventInfo* Info = NULL) const;

            InfoKeeper* m_CommonKeeper;
            std::vector<InfoKeeper*> m_Keepers;
            std::vector<Cut*> m_Cuts;

    };
    class IStorage {
        public:
            void SetSaveTrees(bool B);
            void SetSaveHistos(bool B);
            void SetSaveVariations(bool B);

            bool SaveTrees() const;
            bool SaveVariations() const;
            bool IsParticleVariable() const;

            bool SaveHistos() const;
            bool SaveVariable() const;
            virtual ~IStorage() {
            }
            std::string name() const;
            IEventInfo* XAMPPInfo() const;
        protected:
            IStorage(const std::string &Name, IEventInfo* Info, bool IsParticleVariable, bool IsCommonVariable = false);
            std::string m_Name;
            IEventInfo* m_Info;
            bool m_IsParticleVariable;
            bool m_Registered;
            bool m_SaveHisto;
            bool m_SaveTree;
            bool m_SaveAlways;

    };
    template<class T> class Storage: public IStorage {
        public:
            Storage(const std::string &Name, IEventInfo* Info, bool IsCommon = false);

            StatusCode Store(T Value);
            StatusCode ConstStore(T Value);
            StatusCode ConstStore(const xAOD::EventInfo*, T Value) {
                return ConstStore(Value);
            }

            T& GetValue();
            bool isAvailable() const;
            virtual ~Storage();
        private:
            T m_Variable;
            SG::AuxElement::Decorator<T> m_Decorator;
            SG::AuxElement::ConstAccessor<T> m_ConstAccessor;
    };

    class ParticleStorage: public IStorage {
        public:
            ParticleStorage(const std::string &Name, XAMPP::IEventInfo* info);
            StatusCode Fill(xAOD::IParticleContainer* Particles);
            xAOD::IParticleContainer* Container();
            void DisableTree();

            //Saves the mass in the four momentum instead of the energy
            ITreeBranchVariable* CreateParticleTree(const CP::SystematicSet* Set, TTree* Tree);
            void SaveMassInP4(bool B);

            //Here you can add extra bits of information
            StatusCode SaveInteger(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveFloat(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveDouble(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveChar(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveIntegerVector(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveFloatVector(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveDoubleVector(const std::string &Branch, bool SaveVariations = true);
            StatusCode SaveCharVector(const std::string &Branch, bool SaveVariations = true);
            template<typename T> StatusCode SaveVariable(const std::string &Branch, bool SaveVariations = true);

            virtual ~ParticleStorage();

        protected:
            //Load the TreeHelperIParticle for the current systematic
            void LoadParticleTree(const CP::SystematicSet* Set);

            StatusCode AddVariable(const std::string &Branch, TreeHelperIParticle::StoreVariable, bool SaveVariations);
        private:
            std::string m_Name;
            bool m_UseMass;
            std::map<const CP::SystematicSet*, TreeHelperIParticle*> m_ParticleTrees;
            struct AdditionalBranches {
                    AdditionalBranches(bool S, TreeHelperIParticle::StoreVariable D) {
                        SaveSyst = S;
                        DataType = D;
                    }
                    TreeHelperIParticle::StoreVariable DataType;
                    bool SaveSyst;
            };
            std::map<std::string, AdditionalBranches> m_Vars;
            TreeHelperIParticle* m_CurrentTree;
            const CP::SystematicSet* m_CurrentSyst;
            bool m_Cleared;
            bool m_FillTrees;
    };
}
#include<XAMPPbase/EventStorage.ixx>
#endif
