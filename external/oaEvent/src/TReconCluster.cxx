#include <limits>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "TReconCluster.hxx"
#include "TReconNode.hxx"
#include "HEPUnits.hxx"

ClassImp(ND::TReconCluster);

ND::TReconCluster::TReconCluster()
    : fMoments(3) {
    fState = new TClusterState;
    fNodes = new TReconNodeContainerImpl<ND::TClusterState>;
}

ND::TReconCluster::TReconCluster(const ND::TReconCluster& cluster)
    : ND::TReconBase(cluster), fMoments(3) {
    fNodes = new TReconNodeContainerImpl<ND::TClusterState>;

    // Copy the nodes
    // Create new nodes with TClusterState's
    ND::TReconNodeContainer::const_iterator in;
    for (in=cluster.GetNodes().begin(); in!=cluster.GetNodes().end();in++){
        ND::THandle<ND::TReconNode> node(new ND::TReconNode);
        ND::THandle<ND::TReconBase> object = (*in)->GetObject();
        node->SetObject(object);
        ND::THandle<ND::TClusterState> tstate = (*in)->GetState();
        if (tstate){
            ND::THandle<ND::TReconState> pstate(new ND::TClusterState(*tstate));
            node->SetState(pstate);
        }
        node->SetQuality((*in)->GetQuality());

        fNodes->push_back(node);
    }

    if (cluster.GetState()){
        ND::THandle<ND::TClusterState> state = cluster.GetState();
        fState = new TClusterState(*state);
    }
    else {
        fState = new TClusterState;
    }
}

ND::TReconCluster::~TReconCluster() {}

double ND::TReconCluster::GetEDeposit() const {
    const TClusterState* state
        = dynamic_cast<const TClusterState*>(fState);
    if (!state) throw EMissingField();
    return state->GetEDeposit();
}

double ND::TReconCluster::GetEDepositVariance() const {
    const TClusterState* state
        = dynamic_cast<const TClusterState*>(fState);
    if (!state) throw EMissingField();
    return state->GetEDepositVariance();
}

TLorentzVector ND::TReconCluster::GetPosition() const {
    // This is the preferred way to access a state field.
    THandle<ND::TClusterState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPosition();
}

TLorentzVector ND::TReconCluster::GetPositionVariance() const {
    // This is the preferred way to access a state field.
    THandle<ND::TClusterState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPositionVariance();
}

bool ND::TReconCluster::IsXCluster() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.X())) return false;
    return true;
}

bool ND::TReconCluster::IsYCluster() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Y())) return false;
    return true;
}

bool ND::TReconCluster::IsZCluster() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Z())) return false;
    return true;
}

int ND::TReconCluster::GetDimensions() const{
    TLorentzVector var = GetPositionVariance();
    int dim = 0;
    if (IsXCluster()) ++dim;
    if (IsYCluster()) ++dim;
    if (IsZCluster()) ++dim;
    return dim;
}

const ND::TReconCluster::MomentMatrix& ND::TReconCluster::GetMoments() const {
    return fMoments;
}

void ND::TReconCluster::SetMoments(double xx, double yy, double zz,
                                   double xy, double xz, double yz) {
    fMoments(0,0) = xx;
    fMoments(1,1) = yy;
    fMoments(2,2) = zz;
    fMoments(0,1) = xy;
    fMoments(1,0) = xy;
    fMoments(0,2) = xz;
    fMoments(2,0) = xz;
    fMoments(1,2) = yz;
    fMoments(2,1) = yz;
}

void ND::TReconCluster::SetMoments(const TMatrixT<double>& moments) {
    if (moments.GetNrows() != fMoments.GetNrows()) throw EMomentsSize();
    if (moments.GetNcols() != fMoments.GetNcols()) throw EMomentsSize();
    for (int row=0; row<3; ++row) {
        for (int col=0; col<3; ++col) {
            fMoments(row,col) = moments(row,col);
        }
    }
}

void ND::TReconCluster::UpdateFromHits() {
    // Make sure there is a hit container.
    ND::THandle<ND::THitSelection> hits = GetHits();
    if (!hits) return;

    // Make sure the hit container isn't empty.
    ND::THitSelection::const_iterator beg = hits->begin();
    ND::THitSelection::const_iterator end = hits->end();
    if (end-beg < 1) return;

    // Assume this succeeds!
    fStatus = ND::TReconBase::kSuccess;
    fQuality = 1.0;
    fNDOF = std::max(1,int(end-beg-1));
    ND::THandle<ND::TClusterState> state = GetState();

    int dim = state->GetDimensions();
    TVectorT<double> vals(dim);
    TVectorT<double> sigs(dim);
    TVectorT<double> spreads(dim);

    // Save the index into the state for each of the values.
    int eDep = state->GetEDepositIndex();
    int posX = state->GetXIndex();
    int posY = state->GetYIndex();
    int posZ = state->GetZIndex();
    int posT = state->GetTIndex();

    // Find the energy deposit and the average position.
    TVectorT<double> stateValues(dim);
    TVectorT<double> stateNorms(dim);
    for (ND::THitSelection::const_iterator h = beg;
         h != end;
         ++h) {
        vals(eDep) = (*h)->GetCharge();
        sigs(eDep) = 1.0;

        vals(posX) = (*h)->GetPosition().X();
        sigs(posX) = (*h)->GetUncertainty().X();
        if (!std::isfinite(sigs(posX))
            || sigs(posX) < std::numeric_limits<float>::epsilon()) {
            sigs(posX) = 2.0*(*h)->GetSpread().X()/std::sqrt(12.0);
        }

        vals(posY) = (*h)->GetPosition().Y();
        sigs(posY) = (*h)->GetUncertainty().Y();
        if (!std::isfinite(sigs(posY))
            || sigs(posY) < std::numeric_limits<float>::epsilon()) {
            sigs(posY) = 2.0*(*h)->GetSpread().Y()/std::sqrt(12.0);
        }

        vals(posZ) = (*h)->GetPosition().Z();
        sigs(posZ) = (*h)->GetUncertainty().Z();
        if (!std::isfinite(sigs(posZ))
            || sigs(posZ) < std::numeric_limits<float>::epsilon()) {
            sigs(posZ) = 2.0*(*h)->GetSpread().Z()/std::sqrt(12.0);
        }

        vals(posT) = (*h)->GetTime();
        sigs(posT) = (*h)->GetTimeUncertainty();
        if (!std::isfinite(sigs(posT))
            || sigs(posT) < std::numeric_limits<float>::epsilon()) {
            sigs(posT) = 1.0*unit::ns;
        }

        for (int i=0; i<dim; ++i) {
            if (i == posX && !(*h)->IsXHit()) continue;
            if (i == posY && !(*h)->IsYHit()) continue;
            if (i == posZ && !(*h)->IsZHit()) continue;
            double vv = vals(i)/(sigs(i)*sigs(i));
            double nn = 1.0/(sigs(i)*sigs(i));
            if (!std::isfinite(vv)) continue;
            if (!std::isfinite(nn)) continue;
            stateValues(i) += vv;
            stateNorms(i) += nn;
        }
    }

    // Find the average values.  Notice that the EDeposit is the sum, not the
    // average (done by forcing the stateNorm to 1.0).
    stateNorms(eDep) = 1.0;
    for (int i=0; i<dim; ++i) {
        if (stateNorms(i) > 0) stateValues(i) /= stateNorms(i);
        else stateValues(i) = 0.0;
    }

    // Estimate the covariance of cluster state values.
    TMatrixTSym<double> stateCov(dim);
    // This counts the weight for each bin of the covariance.
    TMatrixTSym<double> weights(dim);
    // This counts the number of degrees of freedom contributing to each bin.
    TMatrixTSym<double> dof(dim);
    for (ND::THitSelection::const_iterator h = beg;
         h != end;
         ++h) {
        // This is will be ignored...  The value here is chosen to make sure
        // that it has a non-zero covariance.
        vals(eDep) = std::sqrt((*h)->GetCharge());
        sigs(eDep) = 1.0;
        spreads(eDep) = 0.0;

        vals(posX) = (*h)->GetPosition().X() - stateValues(posX);
        sigs(posX) = (*h)->GetUncertainty().X();
        spreads(posX) = (*h)->GetSpread().X();
        if (!std::isfinite(sigs(posX))
            || sigs(posX) < std::numeric_limits<float>::epsilon()) {
            sigs(posX) = 2.0*spreads(posX)/std::sqrt(12.0);
        }

        vals(posY) = (*h)->GetPosition().Y() - stateValues(posY);
        sigs(posY) = (*h)->GetUncertainty().Y();
        spreads(posY) = (*h)->GetSpread().Y();
        if (!std::isfinite(sigs(posY))
            || sigs(posY) < std::numeric_limits<float>::epsilon()) {
            sigs(posY) = 2.0*spreads(posY)/std::sqrt(12.0);
        }

        vals(posZ) = (*h)->GetPosition().Z() - stateValues(posZ);
        sigs(posZ) = (*h)->GetUncertainty().Z();
        spreads(posZ) = (*h)->GetSpread().Z();
        if (!std::isfinite(sigs(posZ))
            || sigs(posZ) < std::numeric_limits<float>::epsilon()) {
            sigs(posZ) = 2.0*spreads(posZ)/std::sqrt(12.0);
        }

        vals(posT) = (*h)->GetTime() - stateValues(posT);
        sigs(posT) = (*h)->GetTimeUncertainty();
        spreads(posT) = 1.0;
        if (!std::isfinite(sigs(posT))
            || sigs(posT) < std::numeric_limits<float>::epsilon()) {
            sigs(posT) = 2.0*spreads(posT)/std::sqrt(12.0);
        }

        for (int row = 0; row<dim; ++row) {
            if (row == posX && !(*h)->IsXHit()) continue;
            if (row == posY && !(*h)->IsYHit()) continue;
            if (row == posZ && !(*h)->IsZHit()) continue;
            for (int col = row; col<dim; ++col) {
                if (col == posX && !(*h)->IsXHit()) continue;
                if (col == posY && !(*h)->IsYHit()) continue;
                if (col == posZ && !(*h)->IsZHit()) continue;
                double weight = 1.0/(sigs(row)*sigs(col));
                if (!std::isfinite(weight)) weight = 0.0;
                stateCov(row,col) += weight*vals(row)*vals(col);
                weights(row,col) += weight;
                double degrees
                    = 4.0*spreads(row)*spreads(col)*weight/12.0;
                if (row == posT && col == posT) degrees = 1.0;
                else if (!std::isfinite(degrees)) degrees = 0.0;
                dof(row,col) += degrees;
            }
        }
    }

    // Turn the "stateCov" variable into the RMS.
    for (int row = 0; row<dim; ++row) {
        for (int col = row; col<dim; ++col) {
            if (weights(row,col)>0) stateCov(row,col) /= weights(row,col);
            else stateCov(row,col) = 0.0;
            stateCov(col,row) = stateCov(row,col);
        }
    }

    // Turn the RMS into a covariance of the mean (This is almost a repeat of
    // turning the value into an RMS.
    for (int row = 0; row<dim; ++row) {
        for (int col = row; col<dim; ++col) {
            if (dof(row,col)>0.9) stateCov(row,col) /= std::sqrt(dof(row,col));
            else if (row==col) stateCov(row,col) = ND::TCorrValues::kFreeValue;
            else stateCov(row,col) = 0.0;
            stateCov(col,row) = stateCov(row,col);
        }
    }

    // Add the correction for finite size of the hits (i.e. the bars).
    TVectorT<double> hitWeights(dim);
    for (ND::THitSelection::const_iterator h = beg;
         h != end;
         ++h) {
        sigs(eDep) = std::sqrt((*h)->GetCharge());
        sigs(posX) = (*h)->GetUncertainty().X();
        if (!std::isfinite(sigs(posX))
            || sigs(posX) < std::numeric_limits<float>::epsilon()) {
            sigs(posX) = 2.0*(*h)->GetSpread().X()/std::sqrt(12.0);
        }
        sigs(posY) = (*h)->GetUncertainty().Y();
        if (!std::isfinite(sigs(posY))
            || sigs(posY) < std::numeric_limits<float>::epsilon()) {
            sigs(posY) = 2.0*(*h)->GetSpread().Y()/std::sqrt(12.0);
        }
        sigs(posZ) = (*h)->GetUncertainty().Z();
        if (!std::isfinite(sigs(posZ))
            || sigs(posZ) < std::numeric_limits<float>::epsilon()) {
            sigs(posZ) = 2.0*(*h)->GetSpread().Z()/std::sqrt(12.0);
        }
        sigs(posT) = (*h)->GetTimeUncertainty();
        if (!std::isfinite(sigs(posT))
            || sigs(posT) < std::numeric_limits<float>::epsilon()) {
            sigs(posT) = 1.0*unit::ns;
        }

        for (int idx = 0; idx<dim; ++idx) {
            if (idx == posX && !(*h)->IsXHit()) continue;
            if (idx == posY && !(*h)->IsYHit()) continue;
            if (idx == posZ && !(*h)->IsZHit()) continue;
            double weight = 1.0/(sigs(idx)*sigs(idx));
            if (!std::isfinite(weight)) weight = 0.0;
            hitWeights(idx) += weight;
        }
    }

    for (int idx = 0; idx<dim; ++idx) {
        if (hitWeights(idx)<std::numeric_limits<float>::epsilon()) continue;
        stateCov(idx,idx) += 1.0/hitWeights(idx);
    }

    // Fix the variance of the deposited energy.  This assumes it's Poisson
    // distributed.
    stateCov(eDep,eDep) = stateValues(eDep);

    // Set the state value and covariance.
    for (int row=0; row<dim; ++row) {
        state->SetValue(row,stateValues(row));
        for (int col=0; col<dim; ++col) {
            state->SetCovarianceValue(row,col,stateCov(row,col));
            state->SetCovarianceValue(col,row,stateCov(row,col));
        }
    }

    // Find the moments of the cluster.  This could be done as part of the
    // covariance calculation, but is seperated so it's easier to see what is
    // going on.
    TMatrixTSym<double> moments(3);
    TMatrixTSym<double> chargeSum(3);
    TVector3 center(state->GetValue(posX),
                    state->GetValue(posY),
                    state->GetValue(posZ));

    // Calculate the charge weighted "sum of squared differences".
    for (ND::THitSelection::const_iterator h = beg;
         h != end;
         ++h) {
        TVector3 diff = (*h)->GetPosition() - center;
        for (int row = 0; row<3; ++row) {
            if (row == 0 && !(*h)->IsXHit()) continue;
            if (row == 1 && !(*h)->IsYHit()) continue;
            if (row == 2 && !(*h)->IsZHit()) continue;
            for (int col = row; col<3; ++col) {
                if (col == 0 && !(*h)->IsXHit()) continue;
                if (col == 1 && !(*h)->IsYHit()) continue;
                if (col == 2 && !(*h)->IsZHit()) continue;
                moments(row,col) += diff(row)*diff(col)*(*h)->GetCharge();
                // Correct for the finite size of the hit.
                if (row==col) {
                    double r = 2.0*(*h)->GetSpread()[row]/std::sqrt(12.0);
                    moments(row,col) += r*r*(*h)->GetCharge();
                }
                chargeSum(row,col) += (*h)->GetCharge();
            }
        }
    }

    // Turn this into the average square difference (i.e. the moments)
    for (int row = 0; row<3; ++row) {
        for (int col = row; col<3; ++col) {
            // No divide by zero please!
            if (chargeSum(row,col) > 1E-6) {
                moments(row,col) = moments(row,col)/chargeSum(row,col);
            }
            else if (row == col) {
                // There were no measurements on this axis so spread the
                // charge over the entire range of the detector
                // (i.e. +-"10 m + epsilon")
                moments(row,col) = 1E+9;
            }
            else {
                // There were no measurements so the aren't any off-axis
                // correlations.
                moments(row,col) = 0;
            }
            // Keep it symmetric.
            moments(col,row) = moments(row,col);
        }
    }

    SetMoments(moments);
}

void ND::TReconCluster::ls(Option_t *opt) const {
    ND::TReconBase::ls_base(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();

    if (fState) {
        TROOT::IncreaseDirLevel();
        fState->ls(opt);
        TROOT::DecreaseDirLevel();
    }

    std::ios::fmtflags save = std::cout.flags();
    for (int i = 0; i<3; ++i) {
        TROOT::IndentLevel();
        if (i == 0) std::cout << "Moments: ";
        else        std::cout << "         ";
        for (int j = 0; j<3; ++j) {
            std::cout << std::setw(12) << fMoments(i,j);
        }
        std::cout << std::setw(0) << std::endl;
    }
    std::cout.flags(save);

    if (fNodes) {
        TROOT::IncreaseDirLevel();
        fNodes->ls(opt);
        TROOT::DecreaseDirLevel();
    }

    TROOT::IncreaseDirLevel();
    for (const_iterator v = begin(); v != end(); ++v) {
        (*v)->ls(opt);
    };
    TROOT::DecreaseDirLevel();

    TROOT::DecreaseDirLevel();
}
