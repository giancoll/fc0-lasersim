#include <TLorentzVector.h>
#include "TReconState.hxx"
namespace ND
{
	namespace TReconStateUtilities
	{
	 TLorentzVector GetPosition(ND::THandle<ND::TReconState> s);
	 TLorentzVector GetPositionVariance(ND::THandle<ND::TReconState> s);
	 ND::TCorrValues ProjectPosition(ND::THandle<ND::TReconState> s);
	 int GetPositionIndex(ND::THandle<ND::TReconState> s);

	}
}