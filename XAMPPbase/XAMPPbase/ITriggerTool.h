// ITriggerTool.h

#ifndef XAMPPbase_ITriggerTool_H
#define XAMPPbase_ITriggerTool_H

#include <AsgTools/IAsgTool.h>
#include <xAODBase/ObjectType.h>

#include <XAMPPbase/IElectronSelector.h>
#include <XAMPPbase/IMuonSelector.h>
#include <XAMPPbase/IPhotonSelector.h>
#include <XAMPPbase/ITauSelector.h>

namespace XAMPP {
    typedef std::vector<EleLink> MatchedEl;
    typedef std::vector<MuoLink> MatchedMuo;
    typedef std::vector<PhoLink> MatchedPho;
    typedef std::vector<TauLink> MatchedTau;

    class ParticleStorage;
    class ITriggerTool: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (ITriggerTool)

        public:
            virtual bool CheckTrigger()=0;
            virtual bool CheckTriggerMatching()=0;
            virtual bool CheckTrigger(const std::string &trigger_name) =0;
            virtual bool IsMatchedObject(const xAOD::Electron* el, const std::string& Trig)=0;
            virtual bool IsMatchedObject(const xAOD::Muon* mu, const std::string & Trig)=0;
            virtual bool IsMatchedObject(const xAOD::Photon* ph, const std::string & Trig)=0;
            virtual StatusCode SaveObjectMatching(ParticleStorage* Storage, xAOD::Type::ObjectType Type)=0;
            virtual ~ITriggerTool(){}
    };
}
#endif
