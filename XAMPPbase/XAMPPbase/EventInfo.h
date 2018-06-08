#ifndef XAMPPbase_EventInfo_H
#define XAMPPbase_EventInfo_H

#include <XAMPPbase/EventStorage.h>

#include <XAMPPbase/IEventInfo.h>
#include <XAMPPbase/ISystematics.h>

#include <xAODTracking/Vertex.h>
#include <xAODEventInfo/EventInfo.h>

#include <AsgTools/IAsgTool.h>
#include <AsgTools/AsgTool.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/AnaToolHandle.h>

#include <memory>

namespace CP {
    class SystematicSet;
    class IPileupReweightingTool;
}
class IGoodRunsListSelectionTool;

namespace XAMPP {
    class ISystematics;
    class EventInfo: public asg::AsgTool, virtual public IEventInfo {
        public:

            EventInfo(std::string myname);
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(EventInfo, XAMPP::IEventInfo)

            virtual StatusCode initialize();
            virtual StatusCode LoadInfo();
            virtual StatusCode SetSystematic(const CP::SystematicSet* set);

            virtual const xAOD::EventInfo* GetOrigInfo() const;
            virtual xAOD::EventInfo* GetEventInfo() const;
            virtual const xAOD::Vertex* GetPrimaryVertex() const;
            virtual bool isMC() const;
            virtual unsigned long long eventNumber() const;
            virtual int mcChannelNumber() const;
            virtual unsigned int runNumber() const;
            virtual const CP::SystematicSet* GetSystematic() const;
            virtual bool PassCleaning() const;

            //Functions to create and retrieve event variables saved to the output
            template<typename T> bool DoesVariableExist(const std::string &Name) const;
            template<typename T> StatusCode NewEventVariable(const std::string &Name, bool saveToTree = true, bool SaveVariations = true);
            template<typename T> StatusCode NewCommonEventVariable(const std::string &Name, bool saveToTree = true, bool SaveVariations = true);

            template<typename T> void RemoveVariableFromOutput(const std::string &Name);
            template<typename T> Storage<T>* GetVariableStorage(const std::string &Name) const;
            template<typename T> std::vector<Storage<T>*> GetStorages(OutputElement e = OutputElement::Tree, bool NoimnalOnly = false) const;

            //Functions to create and retieve particle storages
            virtual StatusCode BookParticleStorage(const std::string &Name, bool StoreMass = false, bool SaveVariations = true);
            virtual ParticleStorage* GetParticleStorage(const std::string &Name) const;
            virtual std::vector<ParticleStorage*> GetParticleStorages(OutputElement e = OutputElement::Tree, bool NominalOnly = false) const;

            virtual double GetGenWeight(unsigned int idx = 0) const;
            virtual StatusCode CopyInfoFromNominal(const CP::SystematicSet* To);

            virtual void Lock();

            virtual ~EventInfo();
        protected:
            StatusCode FindPrimaryFertex();
            StatusCode GetInfoFromStore(const CP::SystematicSet* set);
            StatusCode ApplyPrW();
#ifdef XAOD_STANDALONE
            int m_output_level;
#endif
            const xAOD::EventInfo* m_ConstEvtInfo;
            xAOD::EventInfo* m_EvtInfo;
            const xAOD::Vertex* m_primaryVtx;

            const CP::SystematicSet* m_ActSys;
            ToolHandle<XAMPP::ISystematics> m_systematics;
            asg::AnaToolHandle<CP::IPileupReweightingTool> m_prwTool;
            asg::AnaToolHandle<IGoodRunsListSelectionTool> m_GrlTool;
            bool m_ApplyPRW;
            bool m_ApplyGRL;
            bool m_Init;
            bool m_Locked;
            bool m_Filter;
            bool m_RecoFlags;
            struct PileUpDecorators {
                    XAMPP::Storage<double>* muWeight;
                    XAMPP::Storage<float>* AverageCross;
                    XAMPP::Storage<unsigned int>* RandomRunNumber;
                    XAMPP::Storage<unsigned int>* RandomLumiBlock;
            };
            std::map<const CP::SystematicSet*, PileUpDecorators> m_DecPup;  // PileupWeight           
            XAMPP::Storage<int>* m_nNVtx;
            XAMPP::Storage<int>* m_nPU_Vtx;
            XAMPP::Storage<int>* m_PUvertices;
            //XAMPP::Storage<int>* m_nPVtx;
            XAMPP::Storage<char>* m_PassGRL;
            XAMPP::Storage<char>* m_PassLArTile;
            XAMPP::Storage<char>* m_HasVtx;



            //This shared pointer deletes the storage keeper if it goes out of scope
            std::shared_ptr<StorageKeeper> m_Keeper;


    };
}
#include <XAMPPbase/EventInfo.ixx>
#endif
