#ifndef XAMPPbase_SUSYPhotonSelector_H
#define XAMPPbase_SUSYPhotonSelector_H

#include <XAMPPbase/IPhotonSelector.h>
#include <XAMPPbase/SUSYParticleSelector.h>

namespace XAMPP {
    struct PhoWeightDecorators {
            const CP::SystematicSet* Systematic;
            SG::AuxElement::Decorator<double>* PhoSF;
            SG::AuxElement::Decorator<double>* PhoSF_Reco;
            SG::AuxElement::Decorator<double>* PhoSF_Isol;

            XAMPP::Storage<double>* TotalSF;
            XAMPP::Storage<double>* TotalSF_Reco;
            XAMPP::Storage<double>* TotalSF_Isol;
    };

    class SUSYPhotonSelector: public SUSYParticleSelector, virtual public IPhotonSelector {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYPhotonSelector, XAMPP::IPhotonSelector)

            SUSYPhotonSelector(std::string myname);
            virtual ~SUSYPhotonSelector();

            virtual StatusCode initialize();

            virtual StatusCode LoadContainers();
            virtual StatusCode InitialFill(const CP::SystematicSet &systset);
            virtual StatusCode FillPhotons(const CP::SystematicSet &systset);

            virtual PhoLink GetLink(const xAOD::Photon &ph) const;
            virtual PhoLink GetOrigLink(const xAOD::Photon &ph) const;

            virtual xAOD::PhotonContainer* GetPhotons() const {
                return m_Photons;
            }
            virtual xAOD::PhotonContainer* GetPrePhotons() const {
                return m_PrePhotons;
            }
            virtual const xAOD::PhotonContainer* GetPhotonContainer() const {
                return m_xAODPhotons;
            }
            virtual xAOD::PhotonContainer* GetSignalPhotons() const {
                return m_SignalPhotons;
            }
            virtual xAOD::PhotonContainer* GetBaselinePhotons() const {
                return m_BaselinePhotons;
            }
            virtual xAOD::PhotonContainer* GetCustomPhotons(std::string kind);

            virtual StatusCode SaveScaleFactor();

        protected:

            virtual StatusCode CallSUSYTools();
            virtual bool PassSignal(const xAOD::IParticle &P) const;

            const xAOD::PhotonContainer* m_xAODPhotons;
            xAOD::PhotonContainer* m_Photons;
            xAOD::ShallowAuxContainer* m_PhotonsAux;

            xAOD::PhotonContainer * m_PrePhotons; // before OR
            xAOD::PhotonContainer * m_BaselinePhotons; // after OR
            xAOD::PhotonContainer * m_SignalPhotons;

            bool m_SeparateSF;
            std::vector<PhoWeightDecorators> m_SF;
            bool m_doRecoSF;
            bool m_doIsoSF;

    };
}
#endif
