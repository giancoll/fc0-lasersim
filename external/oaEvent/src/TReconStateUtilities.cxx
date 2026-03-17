#include "TReconStateUtilities.hxx"
#include "TClusterState.hxx"
#include "TShowerState.hxx"
#include "TTrackState.hxx"
#include "TPIDState.hxx"
#include "TClusterState.hxx"
#include "TVertexState.hxx"

namespace ND
{
	namespace TReconStateUtilities
	{
		
		TLorentzVector GetPosition(ND::THandle<ND::TReconState> s)
		{
			TReconState* p=GetPointer(s);
			TClusterState* CS = dynamic_cast<TClusterState*> (p);
			if(CS) return CS->GetPosition();

			TShowerState* SS = dynamic_cast<TShowerState*> (p);
			if(SS) return SS->GetPosition();

			TTrackState* TS = dynamic_cast<TTrackState*> (p);
			if(TS) return TS->GetPosition();

			TPIDState* PS = dynamic_cast<TPIDState*> (p);
			if(PS) return PS->GetPosition();

			TVertexState* VS = dynamic_cast<TVertexState*> (p);
			if(VS) return VS->GetPosition();

			ND280Warn(" Control reached end of TReconStateUtilities::GetPosition, this should be impossible.");
			return TLorentzVector();
		}
		TLorentzVector GetPositionVariance(ND::THandle<ND::TReconState> s)
		{
			TReconState* p=GetPointer(s);
			TClusterState* CS = dynamic_cast<TClusterState*> (p);
			if(CS) return CS->GetPositionVariance();

			TShowerState* SS = dynamic_cast<TShowerState*> (p);
			if(SS) return SS->GetPositionVariance();

			TTrackState* TS = dynamic_cast<TTrackState*> (p);
			if(TS) return TS->GetPositionVariance();

			TPIDState* PS = dynamic_cast<TPIDState*> (p);
			if(PS) return PS->GetPositionVariance();

			TVertexState* VS = dynamic_cast<TVertexState*> (p);
			if(VS) return VS->GetPositionVariance();

			ND280Warn(" Control reached end of TReconStateUtilities::GetPositionVariance, this should be impossible.");
			return TLorentzVector();
		}
		ND::TCorrValues ProjectPosition(ND::THandle<ND::TReconState> s)
		{
			TReconState* p=GetPointer(s);

			TClusterState* CS = dynamic_cast<TClusterState*> (p);
			if(CS) return ND::TClusterState::ProjectPosition(s);

			TShowerState* SS = dynamic_cast<TShowerState*> (p);
			if(SS) return ND::TShowerState::ProjectPosition(s);

			TTrackState* TS = dynamic_cast<TTrackState*> (p);
			if(TS) return ND::TTrackState::ProjectPosition(s);

			TPIDState* PS = dynamic_cast<TPIDState*> (p);
			if(PS) return ND::TPIDState::ProjectPosition(s);

			TVertexState* VS = dynamic_cast<TVertexState*> (p);
			if(VS) return ND::TVertexState::ProjectPosition(s);


			ND280Warn(" Control reached end of TReconStateUtilities::ProjectPosition, this should be impossible.");
			return TCorrValues();
		}
		int GetPositionIndex(ND::THandle<ND::TReconState> s)
		{
			TReconState* p=GetPointer(s);

			TClusterState* CS = dynamic_cast<TClusterState*> (p);
			if(CS) return CS->GetPositionIndex();

			TShowerState* SS = dynamic_cast<TShowerState*> (p);
			if(SS) return SS->GetPositionIndex();

			TTrackState* TS = dynamic_cast<TTrackState*> (p);
			if(TS) return TS->GetPositionIndex();


			TPIDState* PS = dynamic_cast<TPIDState*> (p);
			if(PS) return PS->GetPositionIndex();


			TVertexState* VS = dynamic_cast<TVertexState*> (p);
			if(VS) return VS->GetPositionIndex();


			ND280Warn(" Control reached end of TReconStateUtilities::GetPositionIndex, this should be impossible.");
			return -1;
		}

	}
}