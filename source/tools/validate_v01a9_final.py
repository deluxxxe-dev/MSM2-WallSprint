"""A/E are separate captures; namespace their session IDs rather than merging them."""
from pathlib import Path
import argparse,json
from review_surface_pair import review

def validate(a_log,e_log,baseline):
 am={'sessions':{'1':'A'},'control_sessions':[1],'source':'user-confirmed FaceA control; separate v0.1a9 capture'}
 em={'sessions':{'3':'E','4':'E','5':'E'},'diagonal_sessions':[3,4,5],'vertical_sessions':[4,5],'excluded':{'1':'too_short_failed_attach_0.030830s','2':'short_attach_only2samples_not_primary'},'diagonal_geometry_confirmed':True,'same_face_E_confirmed':True,'source':'user-confirmed45deg planar vertical FaceE; separate capture'}
 ar=review(a_log,am,baseline);er=review(e_log,em,baseline)
 aq=ar['sessions']['1'];evidence=baseline['cardinal_evidence_pass'] and ar['gate']['lifecycle_verified'] and aq['surface_pair_candidate_pass'] and aq['surface_plane_reference_candidate'] and aq['movement_direction_invariant'] and aq['min_signed_dot_to_crawl']>=.97 and er['gate']['lifecycle_verified'] and er['gate']['diagonal_surface_verified']
 result={'source':'user-confirmed v0.1a9 A and E; original pre-write telemetry','A':ar,'E':er,'gate':{'surface_axis_verified':bool(evidence),'surface_sign_verified':bool(evidence),'plane_reference_verified':bool(evidence),'diagonal_surface_verified':bool(evidence),'movement_direction_invariant':bool(evidence),'wallrun_subtype_invariant':bool(evidence),'physics_contact_verified':False,'v01b_gate':'READY_FOR_PLANAR_WALL_MVP' if evidence else 'BLOCKED','scope':'exact2.810.0.0/hash, planar vertical surfaces only, formal engine field names unknown'}}
 return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('a_log',type=Path);p.add_argument('e_log',type=Path);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=validate(a.a_log,a.e_log,json.loads(a.baseline.read_text()));a.output.write_text(json.dumps(r,indent=2)+'\n');print(r['gate'])
