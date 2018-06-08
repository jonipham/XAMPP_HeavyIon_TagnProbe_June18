#include <PATInterfaces/SystematicCode.h>
#include <PATInterfaces/SystematicSet.h>
#include <PATInterfaces/SystematicRegistry.h>
#include <PATInterfaces/SystematicVariation.h>

#include <SUSYTools/SUSYObjDef_xAOD.h>

#include <SUSYTools/SUSYCrossSection.h>
#include <XAMPPbase/SUSYSystematics.h>
#include <XAMPPbase/AnalysisUtils.h>

namespace XAMPP {
    SUSYSystematics::SUSYSystematics(std::string myname) :
                AsgTool(myname),
#ifdef XAOD_STANDALONE
                m_output_level(MSG::INFO),
#endif
                m_syst_all(0),
                m_syst_kin(0),
                m_syst_kin_ele(0),
                m_syst_kin_muo(0),
                m_syst_kin_jet(0),
                m_syst_kin_tau(0),
                m_syst_kin_pho(0),
                m_syst_kin_met(0),
                m_syst_kin_tru(0),
                m_syst_weight(0),
                m_syst_weight_elec(0),
                m_syst_weight_muon(0),
                m_syst_weight_tau(0),
                m_syst_weight_phot(0),
                m_syst_weight_jet(0),
                m_syst_weight_btag(0),
                m_syst_weight_met(0),
                m_syst_weight_evtweight(0),
                m_empty_syst(),
                m_act_syst(),
                m_doNoElecs(false),
                m_doNoMuons(false),
                m_doNoJets(false),           
                m_doNoTaus(false),
                m_doNoPhotons(false),
                m_doNoDiTaus(true),
                m_doNoBtag(false),
                m_doSyst(true),
                m_doWeights(true),
                m_Tools(),
                m_init(false),
                m_isData(false) {
        declareProperty("doNoElectrons", m_doNoElecs);
        declareProperty("doNoMuons", m_doNoMuons);
        declareProperty("doNoJets", m_doNoJets);
        declareProperty("doNoPhotons", m_doNoPhotons);
        declareProperty("doNoTaus", m_doNoTaus);
        declareProperty("doNoDiTaus", m_doNoDiTaus);
        declareProperty("doNoBtag", m_doNoBtag);
        declareProperty("doNoTruth",m_doNoTruth);
        declareProperty("doSyst", m_doSyst);
        declareProperty("doWeights", m_doWeights);
        declareProperty("isData", m_isData);
#ifdef XAOD_STANDALONE
        declareProperty("OutputLevel", m_output_level);
#endif
        m_empty_syst = new CP::SystematicSet();
        m_syst_all.push_back(m_empty_syst);
    }

    SUSYSystematics::~SUSYSystematics() {
        for (auto& set : m_syst_all) {
            delete set;
        }
        for (auto & Tool : m_Tools)
            delete Tool;
        ATH_MSG_DEBUG("Destructor called");
    }
    StatusCode SUSYSystematics::InsertKinematicSystematic(const CP::SystematicSet &set, XAMPP::SelectionObject T) {
        if (SystematicsFixed()) {
            ATH_MSG_ERROR("The tool is already initialized");
            return StatusCode::FAILURE;
        }
        if (!m_doSyst && !set.name().empty()) {
            ATH_MSG_DEBUG("Systematics disabled");
            return StatusCode::SUCCESS;
        }
        const CP::SystematicSet* Syst = CreateCopy(set);
        if (!Syst) return StatusCode::FAILURE;
        else if (T == XAMPP::SelectionObject::Electron) AppendSystematic(m_syst_kin_ele, Syst);
        else if (T == XAMPP::SelectionObject::Muon) AppendSystematic(m_syst_kin_muo, Syst);
        else if (T == XAMPP::SelectionObject::Photon) AppendSystematic(m_syst_kin_pho, Syst);
        else if (T == XAMPP::SelectionObject::Tau) AppendSystematic(m_syst_kin_tau, Syst);
        else if (T == XAMPP::SelectionObject::Jet) AppendSystematic(m_syst_kin_jet, Syst);
        else if (T == XAMPP::SelectionObject::TruthParticle) AppendSystematic(m_syst_kin_tru, Syst);
        else if (T == XAMPP::SelectionObject::MissingET) AppendSystematic(m_syst_kin_met, Syst);
        else {
            ATH_MSG_ERROR("Please define an object which is affected by the systematic " << set.name());
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    std::vector<const CP::SystematicSet*> SUSYSystematics::GetKinematicSystematics(XAMPP::SelectionObject T) const {
        if (T == XAMPP::SelectionObject::Electron) return m_syst_kin_ele;
        else if (T == XAMPP::SelectionObject::Muon) return m_syst_kin_muo;
        else if (T == XAMPP::SelectionObject::Photon) return m_syst_kin_pho;
        else if (T == XAMPP::SelectionObject::Tau) return m_syst_kin_tau;
        else if (T == XAMPP::SelectionObject::Jet) return m_syst_kin_jet;
        else if (T == XAMPP::SelectionObject::TruthParticle) return m_syst_kin_tru;
        else if (T == XAMPP::SelectionObject::MissingET) return m_syst_kin_met;
        return m_syst_kin;
    }
    std::vector<const CP::SystematicSet*> SUSYSystematics::GetWeightSystematics(XAMPP::SelectionObject T) const {
        if (T == XAMPP::SelectionObject::Electron) return m_syst_weight_elec;
        else if (T == XAMPP::SelectionObject::Muon) return m_syst_weight_muon;
        else if (T == XAMPP::SelectionObject::Photon) return m_syst_weight_phot;
        else if (T == XAMPP::SelectionObject::Tau) return m_syst_weight_tau;
        else if (T == XAMPP::SelectionObject::Jet) return m_syst_weight_jet;
        else if (T == XAMPP::SelectionObject::BTag) return m_syst_weight_btag;
        else if (T == XAMPP::SelectionObject::EventWeight) return m_syst_weight_evtweight;
        return m_syst_weight;
    }
    const CP::SystematicSet* SUSYSystematics::GetNominal() const {
        return m_empty_syst;
    }
    const CP::SystematicSet* SUSYSystematics::GetCurrent() const {
        return m_act_syst;
    }
    const CP::SystematicSet* SUSYSystematics::CreateCopy(const CP::SystematicSet &set) {
        for (const auto defined : m_syst_all) {
            if (defined->name() == set.name()) {
                ATH_MSG_DEBUG("The systematic " << set.name() << " is already known to the tool");
                return defined;
            }
        }
        const CP::SystematicSet* Syst = new CP::SystematicSet(set);
        m_syst_all.push_back(Syst);
        return Syst;
    }
    StatusCode SUSYSystematics::InsertWeightSystematic(const CP::SystematicSet &set, XAMPP::SelectionObject T) {
        if (SystematicsFixed()) {
            ATH_MSG_ERROR("The tool is already initialized");
            return StatusCode::FAILURE;
        }
        if ((!m_doSyst && !set.name().empty()) || !m_doWeights) {
            ATH_MSG_DEBUG("Systematics disabled");
            return StatusCode::SUCCESS;
        }
        const CP::SystematicSet* Syst = CreateCopy(set);
        if (!Syst) return StatusCode::FAILURE;
        else if (T == XAMPP::SelectionObject::Electron) AppendSystematic(m_syst_weight_elec, Syst);
        else if (T == XAMPP::SelectionObject::Muon) AppendSystematic(m_syst_weight_muon, Syst);
        else if (T == XAMPP::SelectionObject::Photon) AppendSystematic(m_syst_weight_phot, Syst);
        else if (T == XAMPP::SelectionObject::Tau) AppendSystematic(m_syst_weight_tau, Syst);
        else if (T == XAMPP::SelectionObject::Jet) AppendSystematic(m_syst_weight_jet, Syst);
        else if (T == XAMPP::SelectionObject::BTag) AppendSystematic(m_syst_weight_btag, Syst);
        // else if (T == XAMPP::SelectionObject::TruthParticle) AppendSystematic(m_syst_kin_tru,Syst);
        else if (T == XAMPP::SelectionObject::MissingET) AppendSystematic(m_syst_weight_met, Syst);
        else if (T == XAMPP::SelectionObject::EventWeight) AppendSystematic(m_syst_weight_evtweight, Syst);
        else {
            ATH_MSG_ERROR("Please define an object which is affected by the systematic " << set.name());
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    void SUSYSystematics::AppendSystematic(std::vector<const CP::SystematicSet*> &Systematics, const CP::SystematicSet* set) {
        if (!IsInVector(set, Systematics)) Systematics.push_back(set);
    }
    void SUSYSystematics::CopySystematics(std::vector<const CP::SystematicSet*> &From, std::vector<const CP::SystematicSet*> &To) {
        for (const auto& Copy : From) {
            AppendSystematic(To, Copy);
        }
    }
    StatusCode SUSYSystematics::FixSystematics() {
        if (SystematicsFixed()) return StatusCode::SUCCESS;
        ATH_MSG_INFO("Initialising...");
        if (m_doSyst && m_isData) {
            ATH_MSG_FATAL("Trying to run systematics on data! Exiting...");
            return StatusCode::FAILURE;
        }
        if (isData() || m_doNoTruth){
            ATH_MSG_INFO("Settint doNoTruth to true..");
            m_doNoTruth = true;
        }
        if (m_doNoTaus) ATH_MSG_INFO("Setting doNoTaus to true...");
        if (m_doNoPhotons) ATH_MSG_INFO("Setting doNoPhotons to true...");
        if (m_doNoBtag) ATH_MSG_INFO("Setting doNoBtag to true...");
        //No systematic or nothing has been appended yet
        if (!m_doSyst || m_syst_all.size() < 2) {
            m_act_syst = m_empty_syst;
            m_syst_kin = m_syst_all;
            m_syst_kin_ele = m_syst_all;
            m_syst_kin_muo = m_syst_all;
            m_syst_kin_jet = m_syst_all;
            m_syst_kin_tau = m_syst_all;
            m_syst_kin_pho = m_syst_all;
            m_syst_kin_tru = m_syst_all;
            m_syst_kin_met = m_syst_all;
        }
        CopySystematics(m_syst_kin_met, m_syst_kin);
        CopySystematics(m_syst_kin_tru, m_syst_kin);
        CopySystematics(m_syst_kin_ele, m_syst_kin);
        CopySystematics(m_syst_kin_muo, m_syst_kin);
        CopySystematics(m_syst_kin_tau, m_syst_kin);
        CopySystematics(m_syst_kin_jet, m_syst_kin);
        CopySystematics(m_syst_kin_pho, m_syst_kin);
        //Add to each weight syst the nominal set at the latest
        if (m_doWeights) {
            m_syst_weight_elec.push_back(m_empty_syst);
            m_syst_weight_muon.push_back(m_empty_syst);
            if (!m_doNoPhotons) m_syst_weight_phot.push_back(m_empty_syst);
            if (!m_doNoTaus) m_syst_weight_tau.push_back(m_empty_syst);
            m_syst_weight_jet.push_back(m_empty_syst);
            m_syst_weight_btag.push_back(m_empty_syst);
            m_syst_weight.push_back(m_empty_syst);
        }
        m_syst_weight_evtweight.push_back(m_empty_syst);
        PromptSystList(m_syst_kin, "kinematics");
        PromptSystList(m_syst_weight_elec, "Electron ScaleFactor");
        PromptSystList(m_syst_weight_muon, "Muon ScaleFactor");
        PromptSystList(m_syst_weight_tau, "Tau ScaleFactor");
        PromptSystList(m_syst_weight_phot, "Photon ScaleFactor");
        PromptSystList(m_syst_weight_jet, "Jet ScaleFactor");
        PromptSystList(m_syst_weight_btag, "BTag ScaleFactor");
        PromptSystList(m_syst_weight_met, "MET ScaleFactor");

#ifdef XAOD_STANDALONE
        this->msg().setLevel(XAMPP::setOutputLevel(m_output_level));
#endif
        m_init = true;
        return StatusCode::SUCCESS;
    }
    void SUSYSystematics::PromptSystList(std::vector<const CP::SystematicSet*> &List, const std::string &Type) const {
        if (List.empty()) return;
        ATH_MSG_INFO("Systematics affecting the " << Type << ": ");
        for (auto& isys : List) {
            ATH_MSG_INFO("- " << isys->name());
        }
    }
    StatusCode SUSYSystematics::resetSystematics() {
        if (!SystematicsFixed()) {
            ATH_MSG_ERROR("The systematic sets have not been fixed yet");
            return StatusCode::SUCCESS;
        }
        if (m_act_syst == m_empty_syst) return StatusCode::SUCCESS;
        for (const auto& Service : m_Tools)
            ATH_CHECK(Service->resetSystematic());
        m_act_syst = NULL;
        return StatusCode::SUCCESS;
    }

    StatusCode SUSYSystematics::setSystematic(const CP::SystematicSet* Set) {
        if (Set == m_act_syst) return StatusCode::SUCCESS;
        if (!Set) {
            ATH_MSG_ERROR("Null pointer given");
            return StatusCode::FAILURE;
        }
        if (!SystematicsFixed()) {
            ATH_MSG_ERROR("The systematic sets have not been fixed yet");
            return StatusCode::SUCCESS;
        }
        ATH_MSG_DEBUG("Apply systematic variation " << Set->name());
        for (const auto& Service : m_Tools) {
            if (!Service->setSystematic(Set).isSuccess()) {
                ATH_MSG_ERROR("Failed to apply systematic " << Set->name() << " to " << Service->name());
                return StatusCode::FAILURE;
            }
        }
        m_act_syst = Set;
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYSystematics::InsertSystematicToolService(XAMPP::ISystematicToolService* Service) {
        if (!Service) {
            ATH_MSG_ERROR("No ISystematicToolService was given.");
            return StatusCode::FAILURE;
        }

        for (const auto& Tool : m_Tools) {
            if (Tool == Service) {
                ATH_MSG_ERROR("The tool service " << Tool->name() << " has already  been inserted to the systematic tools. Please check your  setup carfully ");
                return StatusCode::FAILURE;
            }
        }
        if (SystematicsFixed()) {
            ATH_MSG_DEBUG("The systematic tool is already initialized. Will not call the initialize method.");
            ATH_MSG_DEBUG("Since the initialize method might extend the already fixed list of systematics.");
        }
        ATH_MSG_INFO("Add new systematic service " << Service->name());

        m_Tools.push_back(Service);
        return StatusCode::SUCCESS;
    }
    bool SUSYSystematics::ProcessObject(XAMPP::SelectionObject T) const {
        if (T == XAMPP::SelectionObject::Electron) return !m_doNoElecs;
        if (T == XAMPP::SelectionObject::Muon) return !m_doNoMuons;
        if (T == XAMPP::SelectionObject::Jet) return !m_doNoJets;
        if (T == XAMPP::SelectionObject::Photon) return !m_doNoPhotons;
        if (T == XAMPP::SelectionObject::Tau) return !m_doNoTaus;
        if (T == XAMPP::SelectionObject::DiTau) return !m_doNoDiTaus;
        if (T == XAMPP::SelectionObject::BTag) return !m_doNoBtag;
        if (T== XAMPP::SelectionObject::TruthParticle) return !m_doNoTruth;
        return true;
    }
    bool SUSYSystematics::AffectsOnlyMET(const CP::SystematicSet* Set) const {
        return (Set != GetNominal() && IsInVector(Set, m_syst_kin_met));
    }
    std::vector<const CP::SystematicSet*> SUSYSystematics::GetAllSystematics() const {
        return m_syst_all;
    }
    bool SUSYSystematics::isData() const {
        return m_isData;
    }
    bool SUSYSystematics::SystematicsFixed() const {
        return m_init;
    }

//########################################################################################################################
//                                                  SUSYToolsSystematicToolHandle
//########################################################################################################################
    SUSYToolsSystematicToolHandle::SUSYToolsSystematicToolHandle(ToolHandle<ST::ISUSYObjDef_xAODTool> &SusyTools) :
                m_SUSYTools_ptr(NULL),
                m_anaSUSYTools(""),
                m_SUSYTools(SusyTools),
                m_systematic("SystematicsTool"),
                m_init(false),
                m_objTypes() {

    }
    SUSYToolsSystematicToolHandle::SUSYToolsSystematicToolHandle(asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> &SusyTools) :
                m_SUSYTools_ptr(NULL),
                m_anaSUSYTools(SusyTools),
                m_SUSYTools(SusyTools.getHandle()),
                m_systematic("SystematicsTool"),
                m_init(false),
                m_objTypes() {

    }
    SUSYToolsSystematicToolHandle::SUSYToolsSystematicToolHandle(ST::ISUSYObjDef_xAODTool* SusyTools) :
                m_SUSYTools_ptr(SusyTools),
                m_anaSUSYTools(""),
                m_SUSYTools(SusyTools->name()),
                m_systematic("SystematicsTool"),
                m_init(false),
                m_objTypes() {

    }
    SUSYToolsSystematicToolHandle::~SUSYToolsSystematicToolHandle() {
        Info(name().c_str(), "Destructor called");
#ifdef XAOD_STANDALONE
        //It seems that PyROOT and smart pointers engage a marigage of deseaster
        //The basic problem is that the smart pointer and PyROOT itself are trying to delete the object each
        //The problem can be solved via leading the concept of smart pointers ad absurdum. Have fun with it

        asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool>* MyCrap = new asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool>("");
        MyCrap->swap(m_anaSUSYTools);
#endif

    }
    StatusCode SUSYToolsSystematicToolHandle::resetSystematic() {
        if (m_SUSYTools->resetSystematics() != CP::SystematicCode::Ok) {
            FatalMSG("Cannot reset systematics of SUSYTools!");
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYToolsSystematicToolHandle::setSystematic(const CP::SystematicSet* Set) {
        if (m_SUSYTools->applySystematicVariation(*Set) != CP::SystematicCode::Ok) {
            FatalMSG("Cannot configure SUSYTools for systematic variation " + Set->name());
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYToolsSystematicToolHandle::initialize() {
        if (m_init) {
            InfoMSG("Already initialized");
            return StatusCode::SUCCESS;
        }
        if (!m_SUSYTools.retrieve().isSuccess()) {
            FatalMSG("Failed to retrieve SUSYTools instance");
            return StatusCode::FAILURE;
        }
        if (!m_systematic.retrieve().isSuccess() || !m_systematic->InsertSystematicToolService(this).isSuccess()) {
            FatalMSG("Could not register myself with the ISystematicsTool");
            return StatusCode::FAILURE;
        }
        m_objTypes.push_back(xAOD::Type::ObjectType::Electron);
        m_objTypes.push_back(xAOD::Type::ObjectType::Muon);
        m_objTypes.push_back(xAOD::Type::ObjectType::Photon);
        m_objTypes.push_back(xAOD::Type::ObjectType::Tau);
        m_objTypes.push_back(xAOD::Type::ObjectType::Jet);
        m_objTypes.push_back(xAOD::Type::ObjectType::BTag);
        m_objTypes.push_back(xAOD::Type::ObjectType::TruthParticle);
        std::vector < ST::SystInfo > m_SystInfoList = m_SUSYTools->getSystInfoList();
        std::vector < xAOD::Type::ObjectType > KineObj { xAOD::Type::ObjectType::Electron, xAOD::Type::ObjectType::Muon, xAOD::Type::ObjectType::Photon, xAOD::Type::ObjectType::Tau, xAOD::Type::ObjectType::Jet, xAOD::Type::ObjectType::TruthParticle };
        for (auto& isys : m_SystInfoList) {
            if ((!m_systematic->ProcessObject(XAMPP::SelectionObject::Tau) && systematicAffectsOnly(isys, xAOD::Type::Tau)) || (!m_systematic->ProcessObject(XAMPP::SelectionObject::Photon) && systematicAffectsOnly(isys, xAOD::Type::Photon)) || (!m_systematic->ProcessObject(XAMPP::SelectionObject::BTag) && systematicAffectsOnly(isys, xAOD::Type::BTag))) {
                InfoMSG("Do not use systematic: " + isys.systset.name());
                continue;
            }
            if (isys.affectsKinematics || isys.systset.name().empty()) {
                for (auto Obj : KineObj) {
                    if (ST::testAffectsObject(Obj, isys.affectsType) || isys.systset.name().empty()) {
                        if (!m_systematic->InsertKinematicSystematic(isys.systset, (XAMPP::SelectionObject) Obj).isSuccess()) {
                            return StatusCode::FAILURE;
                        }
                    }
                }
                //Want to have the MET kinematic systematics first -> it cannot be one of the others
                if (systematicAffectsMET(isys)) {
                    if (!m_systematic->InsertKinematicSystematic(isys.systset, XAMPP::SelectionObject::MissingET).isSuccess()) {
                        return StatusCode::FAILURE;
                    }
                }
            }
            if (isys.affectsWeights && !isys.systset.name().empty() && !isys.affectsKinematics) {
                for (const auto& Obj : m_objTypes) {
                    if (ST::testAffectsObject(Obj, isys.affectsType) && !m_systematic->InsertWeightSystematic(isys.systset, (XAMPP::SelectionObject) Obj).isSuccess()) {
                        return StatusCode::FAILURE;
                    }
                }
                if (isys.affectsType == ST::SystObjType::EventWeight && !m_systematic->InsertWeightSystematic(isys.systset, XAMPP::SelectionObject::EventWeight).isSuccess()) {
                    return StatusCode::FAILURE;
                }
            }

        }
        m_init = true;
        return StatusCode::SUCCESS;
    }
    std::string SUSYToolsSystematicToolHandle::name() const {
        return "XAMPP.Srv" + m_SUSYTools.name();
    }
    void SUSYToolsSystematicToolHandle::FatalMSG(const std::string& MSG) {
        Fatal(name().c_str(), MSG.c_str());
    }
    void SUSYToolsSystematicToolHandle::InfoMSG(const std::string& MSG) {
        Info(name().c_str(), MSG.c_str());
    }
    bool SUSYToolsSystematicToolHandle::systematicAffectsOnly(ST::SystInfo set, xAOD::Type::ObjectType T) {
        if (!ST::testAffectsObject(T, set.affectsType)) return false;
        for (auto objType : m_objTypes) {
            if ((T != objType) && ST::testAffectsObject(objType, set.affectsType)) return false;
        }
        return true;
    }
    bool SUSYToolsSystematicToolHandle::systematicAffectsMET(ST::SystInfo& isys) {
        // the logic is that the systematic should not affect any type
        for (const auto& obj : m_objTypes) {
            if (ST::testAffectsObject(obj, isys.affectsType)) return false;
        }
        return true;
    }
}
