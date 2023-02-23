#include <Cube4D.hh>
#include <Cube4DCollection.hh>

void Cube4DCollection::SetPMTs( std::vector<std::pair< UInt_t, double > > pmts_ ) {

  fPMTs = pmts_;

  for( size_t i_cube = 0; i_cube < this->GetNCubes(); i_cube++) {
    fCubeVec.at( i_cube )->SetPMTs( fPMTs );
  }

}


void Cube4DCollection::RemoveRepeatedPMTs( ) {
  
  this->SortByOverlap();

  std::vector<std::pair< UInt_t, double > > hit_pmts = fCubeVec.at(0)->GetPMTs();

  for( size_t i_cube = 1; i_cube < this->GetNCubes(); i_cube++ ) {
    fCubeVec.at( i_cube )->RemovePMTs( hit_pmts );
    std::vector<std::pair< UInt_t, double > > temp_vec = fCubeVec.at(i_cube)->GetPMTs();
    hit_pmts.insert( std::end(hit_pmts), std::begin(temp_vec), std::end(temp_vec) );
  }

}


void Cube4DCollection::SortByOverlap( ) {

  std::sort(fCubeVec.begin(), fCubeVec.end(), [](const Cube4D* c1, const Cube4D* c2) {
      return c1->GetLLH() > c2->GetLLH();
    });

}


void Cube4DCollection::RemovePMTs( std::vector< std::pair< UInt_t, double > > pmts ) {

  for( size_t i_cube = 1; i_cube < this->GetNCubes(); i_cube++ ) {
    fCubeVec.at( i_cube )->RemovePMTs( pmts );
  }

}


Cube4DCollection::~Cube4DCollection() {
  for( size_t i_cube = 0; i_cube < fCubeVec.size(); i_cube++ ) {
    delete fCubeVec.at(i_cube);
  }
  fCubeVec.clear();
}
