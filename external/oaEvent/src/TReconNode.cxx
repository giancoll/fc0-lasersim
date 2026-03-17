
#include "TReconNode.hxx"

ClassImp(ND::TReconNode);

ND::TReconNode::TReconNode()
    : fQuality(0.) { }

ND::TReconNode::~TReconNode() {}

void ND::TReconNode::ls(Option_t *opt) const {
    TROOT::IndentLevel();
    std::cout << ClassName() << "(" << this << ")::" << std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Quality: " << GetQuality() << std::endl;
    if (fState) {
        fState->ls(opt);
    }
    else {
        TROOT::IndentLevel();
        std::cout << "State Information is missing" << std::endl;
    }
    if (fObject) {
        fObject->ls(opt);
    }
    else {
        TROOT::IndentLevel();
        std::cout << "Object Information is missing" << std::endl;
    }
    TROOT::DecreaseDirLevel();
}

ClassImp(ND::TReconNodeContainer);

ND::TReconNodeContainer::TReconNodeContainer() {}

ND::TReconNodeContainer::~TReconNodeContainer() {}

void ND::TReconNodeContainer::ls(Option_t *opt) const {
    TROOT::IndentLevel();
    std::cout << ClassName() << "(" << this << ")::" << std::endl;
    TROOT::IncreaseDirLevel();
    for (ND::TReconNodeContainer::const_iterator n = begin();
         n != end();
         ++n) {
        (*n)->ls(opt);
    }
    TROOT::DecreaseDirLevel();
}

