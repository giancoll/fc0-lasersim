#ifndef TReconNode_hxx_seen
#define TReconNode_hxx_seen

#include "TObject.h"

#include "EoaCore.hxx"
#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TReconState.hxx"
#include "TND280Log.hxx"

namespace ND {
    class TReconNode;
    class TReconNodeContainer;

    /// An exception base class for TReconNode.
    OA_EXCEPTION(EReconNode, EReconObject);

    /// An exception thrown when a TReconNode object being added to a
    /// TReconNodeContainer has a state with the wrong type.  The type of the
    /// state must match the state type of the containing TReconBase object.
    OA_EXCEPTION(EWrongStateType, EReconNode);

    /// An exception thrown when a TReconNode object being added to a
    /// TReconNodeContainer without an associated reconstruction object.
    OA_EXCEPTION(EObjectEmpty, EReconNode);
}

/// Class to contain the association between a TReconState and the object
/// assocated with the state.  This class is used to encode information
/// required to refit an object (e.g. refit a track as part of a global fit),
/// as well as to encode the path of extended objects.  This class can be
/// thought of as representing one point in a fit.
class ND::TReconNode: public TObject {
public:
    TReconNode();
    virtual ~TReconNode();

    /// Get the state associated with this node.
    ND::THandle<ND::TReconState> GetState() const {return fState;}

    /// Set the state associated with this node.
    void SetState(ND::THandle<ND::TReconState>& state) {fState = state;}

    /// Get the reconstruction object associated with this node.
    ND::THandle<ND::TReconBase> GetObject() const {return fObject;}
    
    /// Set the reconstruction object associates with this node.
    void SetObject(ND::THandle<ND::TReconBase>& object) {fObject = object;}

    /// Get the goodness associated with the connection between the state and
    /// the object.
    double GetQuality() const {return fQuality;}

    /// Set the goodness associated with the connection between this state
    /// and object.
    void SetQuality(double quality) {fQuality = quality;}
    
    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;

private:

    /// The state associated with the object.
    ND::THandle<ND::TReconState> fState;

    /// The object that is associated with the state.
    ND::THandle<ND::TReconBase> fObject;
    
    /// A log likelihood for the association of the object with the state.
    float fQuality;

    ClassDef(TReconNode,2);
};

/// A base class for containers of TReconNode objects.
class ND::TReconNodeContainer 
    : public TObject, public std::vector< THandle<TReconNode> > {
public:
    TReconNodeContainer();
    virtual ~TReconNodeContainer();

    /// Add a node to the container.  The std::vector::push_back method is
    /// overloaded so that it can check that the correct type of TReconState
    /// object is being added to the container.  This is overloaded by the
    /// TReconNodeContainerImpl template.
    virtual void push_back(const ND::THandle<ND::TReconNode>& node) {
        //ND::THandle<ND::TReconBase> obj = node->GetObject();
        //if (!obj) {
        //    ND280Severe("Node added to a TReconNodeContainer"
        //                " without an associated recon object");
        //    throw EObjectEmpty();
        //}
        //std::vector< ND::THandle<ND::TReconNode> >::push_back(node);

        ND280Error("Never directly call TReconNodeContainer::push_back()");
        throw std::exception();
    }

    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;


    ClassDef(TReconNodeContainer,1);
};

namespace ND {
    /// Provide a class specific container for TReconNode objects.  This
    /// checks that all of the nodes contain the right class of state before
    /// they are added to the container.
    template <class T>
    class TReconNodeContainerImpl : public TReconNodeContainer {
    public:
        TReconNodeContainerImpl() {}
        virtual ~TReconNodeContainerImpl() {}

        /// Add a node to the container.  The std::vector::push_back method is
        /// overloaded so that it can make sure that the node contains the
        /// correct type of TReconState object.
        virtual void push_back(const ND::THandle<ND::TReconNode>& node) {
            
            // Check that node has a state
            if (node->GetState()){
                ND::THandle<T> n(node->GetState());
                // Check that the state handle is valid (correct state type)
                if (!n) {
                    ND280Severe("Wrong type of state being added to a "
                                "TReconNodeContainer");
                    throw EWrongStateType();
                }
            }

            ND::THandle<ND::TReconBase> obj = node->GetObject();
            if (!obj) {
                ND280Severe("Node added to a TReconNodeContainer"
                            " without an associated recon object");
                throw EObjectEmpty();
            }
            std::vector< ND::THandle<ND::TReconNode> >::push_back(node);
        }

        ClassDefT(TReconNodeContainerImpl,1);
    };
    ClassDefT2(TReconNodeContainerImpl,T);
}
#endif
