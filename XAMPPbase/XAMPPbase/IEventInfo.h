#ifndef XAMPPbase_IEventInfo_H
#define XAMPPbase_IEventInfo_H

#include <xAODTracking/Vertex.h>
#include <xAODEventInfo/EventInfo.h>

#include <AsgTools/IAsgTool.h>

namespace CP {
    class SystematicSet;
}

namespace XAMPP {
    class ParticleStorage;
    class IEventInfo: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (XAMPP::IEventInfo)
        public:
            enum OutputElement {
                Tree, Histo
            };

            virtual StatusCode initialize()=0;
            virtual StatusCode LoadInfo()=0;
            virtual StatusCode SetSystematic(const CP::SystematicSet* set)=0;

            virtual const xAOD::EventInfo* GetOrigInfo() const=0;
            virtual xAOD::EventInfo* GetEventInfo() const = 0;
            virtual const xAOD::Vertex* GetPrimaryVertex() const = 0;

            virtual const CP::SystematicSet* GetSystematic() const=0;

            virtual bool isMC() const=0;
            virtual unsigned long long eventNumber() const=0;
            virtual int mcChannelNumber() const=0;
            virtual unsigned int runNumber() const =0;
            virtual StatusCode CopyInfoFromNominal(const CP::SystematicSet* To)=0;
            virtual double GetGenWeight(unsigned int idx = 0) const=0;
            virtual bool PassCleaning() const = 0;

            //
            //Functions to create and retieve particle storages
            virtual StatusCode BookParticleStorage(const std::string &Name, bool StoreMass = false, bool SaveVariations = true)=0;
            virtual ParticleStorage* GetParticleStorage(const std::string &Name) const=0;
            virtual std::vector<ParticleStorage*> GetParticleStorages(OutputElement e = OutputElement::Tree, bool NominalOnly = false) const=0;

            //Method which disables the adding of storages
            virtual void Lock()=0;
            virtual ~IEventInfo() {
            }

    };
}
#endif
