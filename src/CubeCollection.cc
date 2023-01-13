#include <Cube.hh>
#include <CubeCollection.hh>

void CubeCollection::SetPMTs( std::vector< RAT::DS::PMTCal > pmts_ ) {

  fPMTs = pmts_;

  for( size_t i_cube = 0; i_cube < this->GetNCubes(); i_cube++) {
    fCubeVec.at( i_cube )->SetPMTs( fPMTs );
  }

}


void CubeCollection::RemoveRepeatedPMTs( ) {
  
  this->SortByOverlap();

  std::vector< RAT::DS::PMTCal > hit_pmts = fCubeVec.at(0)->GetPMTs();

  for( size_t i_cube = 1; i_cube < this->GetNCubes(); i_cube++ ) {
    fCubeVec.at( i_cube )->RemovePMTs( hit_pmts );
    std::vector< RAT::DS::PMTCal > temp_vec = fCubeVec.at(i_cube)->GetPMTs();
    hit_pmts.insert( std::end(hit_pmts), std::begin(temp_vec), std::end(temp_vec) );
  }

}


void CubeCollection::SortByOverlap( ) {

  std::sort(fCubeVec.begin(), fCubeVec.end(), [](const Cube* c1, const Cube* c2) {
      return c1->GetLLH() > c2->GetLLH();
    });

}
