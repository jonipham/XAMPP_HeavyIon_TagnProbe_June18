#ifndef XAMPPbase_TreeHelpers_H
#define XAMPPbase_TreeHelpers_H

#include <XAMPPbase/Defs.h>
#include <xAODBase/IParticleContainer.h>

#include <AsgTools/IAsgTool.h>
#include <TMath.h>
#include <iostream>
#include <TTree.h>

namespace CP {
    class SystematicSet;
}
namespace XAMPP {
    template<class T> class Storage;
    class ParticleStorage;
    //Interface class to store particle variables in Vector Branches 
    class ITreeParticleAccesor {
        public:
            virtual bool PushBack(const xAOD::IParticle* P, bool Resize, size_t Reserve)=0;
            virtual void Clear(size_t Reserve = 0)=0;
            virtual bool Init(const std::string &BranchName)=0;
            virtual std::string name() const = 0;
            virtual ~ITreeParticleAccesor() {
            }
    };
    //Interface class to store Variables in general
    class ITreeBranchVariable {
        public:
            virtual bool Init()=0;
            virtual std::string name() const = 0;
            virtual bool Fill()=0;
            virtual ~ITreeBranchVariable() {
            }
    };

    class TreeHelper {
        public:
            virtual ~TreeHelper() {
            }
        protected:
            TreeHelper(TTree* tree);
            template<typename T> bool AddBranch(const std::string &Name, T &Element);
            std::string EraseWhiteSpaces(const std::string &In);
            TTree* m_tree;
    };

    class TreeHelperMet: public TreeHelper, virtual public ITreeBranchVariable {
        public:
            TreeHelperMet(XAMPP::Storage<XAMPPmet>* met, TTree* tree);
            virtual bool Init();
            virtual std::string name() const;
            virtual bool Fill();
        protected:
            XAMPP::Storage<XAMPPmet>* m_store;
            float m_met;
            float m_phi;
            float m_sumet;
    };
    template<class T> class EventBranchVariable: public TreeHelper, virtual public ITreeBranchVariable {
        public:
            EventBranchVariable(XAMPP::Storage<T>* Store, TTree* tree);
            virtual bool Init();
            virtual std::string name() const;
            virtual bool Fill();
            virtual ~EventBranchVariable() {
            }
        protected:
            XAMPP::Storage<T>* m_Store;
    };

    class TreeHelperIParticle: public TreeHelper, virtual public ITreeBranchVariable {
        public:
            TreeHelperIParticle(ParticleStorage* PStore, TTree* tree, bool UseMass = false);

            void SetEventContainer(xAOD::IParticleContainer* Cont);
            virtual ~TreeHelperIParticle();

            virtual bool Fill();
            virtual bool Init();
            xAOD::IParticleContainer* Container() const;
            virtual std::string name() const;

            typedef bool (TreeHelperIParticle::*StoreVariable)(std::string Name);
            template<typename T> bool StoreDecorator(std::string BranchName);
        private:
            bool Update(xAOD::IParticleContainer* Cont);
            bool IsDecoratorStored(const std::string &Name);
            bool PushBack(const xAOD::IParticle* P, bool Clear, size_t &Reserve);
            ParticleStorage* m_Store;
            bool m_init;
            bool m_dumped;
            std::vector<ITreeParticleAccesor*> m_Branches;
            xAOD::IParticleContainer* m_Container;

    };
    //Class to store a general particle variable which is retrievable via a SG::AuxElement ConstAccessor
    template<class T> class TreeParticleAccesor: public TreeHelper, public ITreeParticleAccesor {
        public:
            TreeParticleAccesor(const std::string &Name, TTree* tree);
            ~TreeParticleAccesor();
            virtual bool PushBack(const xAOD::IParticle* P, bool Resize, size_t Reserve);
            virtual void Clear(size_t Reserve = 0);
            virtual bool Init(const std::string &BranchName);
            virtual std::string name() const;
        protected:
            std::string m_name;
            std::vector<T> m_Vector;
            SG::AuxElement::ConstAccessor<T>* m_Acc;
    };

    //Classes to store the momentum Components of the particle
    class ITreeParticleFourMomentConstAccessor: public TreeHelper, public ITreeParticleAccesor {
        public:
            virtual bool PushBack(const xAOD::IParticle* P, bool Resize, size_t Reserve);
            virtual void Clear(size_t Reserve = 0);
            virtual bool Init(const std::string &BranchName);
            virtual std::string name() const;
            virtual ~ITreeParticleFourMomentConstAccessor();
            ITreeParticleFourMomentConstAccessor(TTree* tree, Momentum Mom);
        protected:
            Momentum m_Momentum;
            std::vector<float> m_Vector;
    };

}
#include <XAMPPbase/TreeHelpers.ixx>
#endif
