#ifndef XAMPPbase_Defs_H
#define XAMPPbase_Defs_H

#include <xAODMissingET/MissingET.h>
namespace XAMPP {
    typedef xAOD::MissingET* XAMPPmet;
    typedef double (xAOD::IParticle::*Momentum)() const;

    typedef SG::AuxElement::ConstAccessor<char> CharAccessor;
    typedef SG::AuxElement::Decorator<char> CharDecorator;
    typedef std::unique_ptr<CharAccessor> SelectionAccessor;
    typedef std::unique_ptr<CharDecorator> SelectionDecorator;

    typedef SG::AuxElement::ConstAccessor<float> FloatAccessor;
    typedef SG::AuxElement::Decorator<float> FloatDecorator;

    typedef SG::AuxElement::ConstAccessor<double> DoubleAccessor;
    typedef SG::AuxElement::Decorator<double> DoubleDecorator;

    typedef SG::AuxElement::ConstAccessor<int> IntAccessor;
    typedef SG::AuxElement::Decorator<int> IntDecorator;


    typedef SG::AuxElement::ConstAccessor<unsigned int> UIntAccessor;
    typedef SG::AuxElement::Decorator<unsigned int> UIntDecorator;

    enum RunType {
        Local = 0, Grid, Batch
    };
    enum SelectionObject {
        Other = 0, Jet = 2, Electron = 6, Photon = 7, Muon = 8, Tau = 9, BTag = 102, TruthParticle = 201, MissingET = 301, EventWeight = 302, RecoParticle = 960, DiTau = 980
    };
    enum ZVeto {
        Pass = 1, Fail2Lep = 2, Fail3Lep = 4, Fail4Lep = 8, Fail2LepPhot = 16
    };
    enum Stop_RecoTypes {
        None = 0, TopWCandidate1, // distinguish top and W by PdgId (6 vs. 24)
        TopWCandidate2, // distinguish top and W by PdgId (6 vs. 24)
        DRB4TopWCandidate1, // distinguish top and W by PdgId (6 vs. 24)
        DRB4TopWCandidate2, // distinguish top and W by PdgId (6 vs. 24)
        MinMassTopWCandidate1, // distinguish top and W by PdgId (6 vs. 24)
        MinMassTopWCandidate2, // distinguish top and W by PdgId (6 vs. 24)
        Chi2TopWCandidate1, // distinguish top and W by PdgId (6 vs. 24)
        Chi2TopWCandidate2 // distinguish top and W by PdgId (6 vs. 24)
    };
    const float Z_MASS = 91200.;
    const float W_MASS = 80387.;
    const float W_WIDTH = 2085;
    const float Top_MASS = 173210.;
    const float MeVToGeV = 1. / 1000.;
    enum LLE_Decay {
        Unknown = 0, ElElmu, ElMuel, //121
        ElMumu,
        MuMuel, //122
        ElTamu,
        MuTael, //123
        ElElta,
        ElTael, //131
        ElMuta, //MuTael //132
        ElTata,
        TaTael, //133
        //ElMuta,ElTamu, //231
        MuMuta,
        MuTamu, //232
        MuTata,
        TaTamu //233
    };
    enum WDecayModes {
        Unspecified = 0, Hadronic = 1, ElecNeut = 2, MuonNeut = 3, HadTauNeut = 4, LepTauNeut = 5,
    };
    enum JetAlgorithm {
        AntiKt2 = 0, AntiKt4, AntiKt10
    };

}
#endif
