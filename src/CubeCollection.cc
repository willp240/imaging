#include <Cube.hh>
#include <CubeCollection.hh>

void CubeCollection::SetPMTs( std::vector< RAT::DS::PMTCal > pmts_ ) {

  fPMTs = pmts_;

  for( size_t i_cube = 0; i_cube < this->GetNCubes(); i_cube++) {
    fCubeVec.at( i_cube )->SetPMTs( fPMTs );
  }

}
