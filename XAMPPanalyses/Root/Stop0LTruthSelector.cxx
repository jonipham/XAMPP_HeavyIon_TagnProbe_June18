#include <XAMPPanalyses/Stop0LTruthSelector.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/EventInfo.h>

namespace XAMPP {
    Stop0LTruthSelector::Stop0LTruthSelector(std::string myname) :
                    SUSYTruthSelector(myname),
                    m_MEPhotonCache(false) {
        declareProperty("CheckMEPhoton", m_CheckMEPhoton = true);
        declareProperty("MEphotonPtCut", m_MEphotonPtCut = 80000.);
    }

    StatusCode Stop0LTruthSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(SUSYTruthSelector::initialize());
        if (m_CheckMEPhoton) ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("MEPhoton"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("isW30"));
        return StatusCode::SUCCESS;
    }
    void Stop0LTruthSelector::InitDecorators(const xAOD::Jet* J, bool Pass) {
        SUSYTruthSelector::InitDecorators(J, Pass);
        IsBJet(J);
    }
    void Stop0LTruthSelector::InitDecorators(const xAOD::TruthParticle* T, bool Pass) {
        SUSYTruthSelector::InitDecorators(T, Pass);
    }
    StatusCode Stop0LTruthSelector::FillTruth(const CP::SystematicSet &systset) {
        ATH_CHECK(SUSYTruthSelector::FillTruth(systset));
        static XAMPP::Storage<char>* dec_isW30 = m_XAMPPInfo->GetVariableStorage<char>("isW30");
        bool onlyRealWs = true;
        static SG::AuxElement::Decorator<char> dec_RealWMass("isRealW");
        for (const auto& particle : *m_Particles) {
            dec_RealWMass(*particle) = false;
            if (isTrueW(particle)) {
                dec_RealWMass(*particle) = true;
                if (fabs(particle->m() - W_MASS) > 30000.) onlyRealWs = false;
            }
        }
        ATH_CHECK(dec_isW30->Store(onlyRealWs));
        if (m_CheckMEPhoton) {
            static XAMPP::Storage<char>* dec_MEPhoton = m_XAMPPInfo->GetVariableStorage<char>("MEPhoton");
            if (SystematicAffects(systset)) {
                const xAOD::TruthParticle* LeadPh = dynamic_cast<const xAOD::TruthParticle*>(XAMPP::FindLeadingParticle(GetTruthBaselinePhotons()));
                m_MEPhotonCache = false;
                if (!LeadPh || !LeadPh->isPhoton() || LeadPh->pt() < m_MEphotonPtCut) m_MEPhotonCache = false;
                else {
                    const xAOD::TruthParticle* Ph = XAMPP::GetFirstChainLink(LeadPh);
                    for (size_t p = 0; p < Ph->nParents(); ++p) {
                        if (Ph->parent(p)->absPdgId() < 100) m_MEPhotonCache = true;
                    }
                }
            }
            ATH_CHECK(dec_MEPhoton->Store(m_MEPhotonCache));
        }
        return StatusCode::SUCCESS;
    }

}
