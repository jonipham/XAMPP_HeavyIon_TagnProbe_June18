#include<XAMPPbase/CxAODMaker.h>

namespace XAMPP {

    template<typename Cont, typename AuxCont> void CxAODMaker::CreateContainer(Cont* &Container, AuxCont* & AuxContainer) {
        Container = new Cont();
        AuxContainer = new AuxCont();
        Container->setStore(AuxContainer);
    }

    template<typename Cont, typename AuxCont> StatusCode CxAODMaker::StoreContainer(std::string Key, Cont* Container, AuxCont* AuxContainer) {
#ifdef XAOD_STANDALONE
        if( !m_event->record(Container,Key).isSuccess() ) {
            ATH_MSG_ERROR("Could not store the container "<<Key);
            return StatusCode::FAILURE;
        }
        if (!m_event->record(AuxContainer, Key+"Aux.").isSuccess()) {
            ATH_MSG_ERROR("Could not store the  auxillary container "<<Key);
            return StatusCode::FAILURE;
        }
#endif
        return StatusCode::SUCCESS;
    }
    template<typename Cont> StatusCode CxAODMaker::CopyContainer(Cont* Original, Cont* Copy) {
        if (!Original || !Copy) {
            ATH_MSG_ERROR("Could not copy Container");
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
}
