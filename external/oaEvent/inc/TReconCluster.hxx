#ifndef TReconCluster_hxx_seen
#define TReconCluster_hxx_seen

#include <TMatrixT.h>
#include <TMatrixTSym.h>

#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TClusterState.hxx"
#include "EoaCore.hxx"

namespace ND {
    class TReconCluster;

    OA_EXCEPTION(EReconCluster,EoaCore);
    OA_EXCEPTION(EMomentsSize,EReconCluster);
}

/// Represent an extended energy deposition centered at a position (i.e. a
/// spheroidal blob of energy).  This type of energy deposit is described by
/// the amount of energy, and the central position of the deposit
/// (deposit,position,time) in an ND::TClusterState object.  The specific hits
/// associated with the cluster are available through the GetHits method.  The
/// moments of the distribution can be retrieved using the GetMoments()
/// method.
///
/// A TReconCluster object is a relatively heavy object.  Small collecions of
/// hits (e.g. pairs of hits in a single P0D plane, or very local TPC charge
/// depositions) should be saved using a TComboHit, or a TReconHit.  See
/// ND::TChargeCluster2D as an example.
///
/// The ND::TReconCluster class is intended to describe the geometry of the
/// energy deposition in a detector, and not make the association with a
/// particular particle identification.  Assignment of particle types to the
/// detector measurements is done in the ND::TReconPID class.
class ND::TReconCluster: public ND::TReconBase {
public:
    typedef TMatrixTSym<float> MomentMatrix;

    TReconCluster();

    /// copy constructor
    TReconCluster(const ND::TReconCluster& cluster);

    virtual ~TReconCluster();

    /// Get the energy deposited in the cluster.
    double GetEDeposit() const;

    /// Get the variance of the energy deposited in the cluster.
    double GetEDepositVariance() const;

    /// Get the cluster position.
    TLorentzVector GetPosition() const;

    /// Get the track starting position uncertainty.
    TLorentzVector GetPositionVariance() const;

    /// Get the number of (non-free) spacial dimensions
    int GetDimensions() const;

    /// Check if this cluster has X information.
    bool IsXCluster() const;

    /// Check if this cluster has Y information.
    bool IsYCluster() const;

    /// Check if this cluster has Z information.
    bool IsZCluster() const;

    /// Get the moments of the charge distribution.
    const MomentMatrix& GetMoments() const;

    /// Set the moments of the charge distribution.
    void SetMoments(double xx, double yy, double zz,
                    double xy, double xz, double yz);

    /// Set the moments of the charge distribution.  The input matrix is
    /// assumed to be symmetric and only the upper half of the elements are
    /// accessed.
    void SetMoments(const TMatrixT<double>& moments);

    /// Fill the ND::TReconCluster and ND::TClusterState objects from a
    /// ND::THitSelection.  The position is set to average hit position (the
    /// hit position uncertainty is used), and the energy deposit is the sum
    /// of the hit charges.
    void FillFromHits(const char* name, const ND::THitSelection& hits) {
        FillFromHits(name, hits.begin(), hits.end());
    }

    /// Fill the ND::TReconCluster and ND::TClusterState objects from hits
    /// between the begin and end iterators.  The position is set to average
    /// hit position (the hit position uncertainty is used), and the energy
    /// deposit is the sum of the hit charges.  The algorithm name is set to
    /// the first argument.
    ///
    /// \note Since FillFromHits is typically used with 3D hit objects
    /// (i.e. TReconHit), care must be taken in the reconstruction to make
    /// sure that the charge is properly shared among the hits.  For instance,
    /// if a particular hit contributes to several 3D hits (the usual case),
    /// its charge must be distributed over all of those hits.
    ///
    /// \note If FillFromHits is used with 2D hits (i.e. U, V, and X hits),
    /// the resulting covariances, and energy deposits will have been
    /// calculated using incorrect assumptions.
    template <typename T>
    void FillFromHits(const char* name, T begin,T end) {
        // Set the algorithm name.
        fAlgorithm = std::string(name);

        // Add a copy of the hits to the cluster.
        if (end == begin) return;
        THitSelection* hits = new THitSelection("clusterHits");
        std::copy(begin, end, std::back_inserter(*hits));
        AddHits(hits);

        // Update the cluster fields based on the hits.
        UpdateFromHits();
    }

    /// List the results of in the cluster.
    virtual void ls(Option_t* opt = "") const;

private:

    /// Fill the cluster state from the current hits.
    void UpdateFromHits();

    /// The moments for this cluster.
    MomentMatrix fMoments;

    ClassDef(TReconCluster,1);
};
#endif
