#include <Cube4D.hh>
#include <Cube4DCollection.hh>

Cube4DCollection::Cube4DCollection( const Cube4DCollection &col ){

  //    fCubeVec = collection.GetCubeVec();
  for(int i_cube = 0; i_cube < col.GetCubeVec().size(); i_cube++){
    Cube4D* cub = new Cube4D(*col.GetCube(i_cube));
    fCubeVec.push_back(cub);
  }
  fPMTs = col.GetPMTs();
  fT = col.GetT();
  fTRadius = col.GetTRadius();

}

void Cube4DCollection::SetPMTs( std::vector<std::pair< UInt_t, double > > pmts_ ) {

  fPMTs = pmts_;

  for( size_t i_cube = 0; i_cube < this->GetNCubes(); i_cube++) {
    fCubeVec.at( i_cube )->SetPMTs( fPMTs );
  }

}


void Cube4DCollection::SetT( double t_ ) {

  fT = t_;

  for( size_t i_cube = 0; i_cube < this->GetNCubes(); i_cube++) {
    fCubeVec.at( i_cube )->SetT( fT );
  }

}


void Cube4DCollection::SetTRadius( double trad_ ) {

  fTRadius = trad_;

  for( size_t i_cube = 0; i_cube < this->GetNCubes(); i_cube++) {
    fCubeVec.at( i_cube )->SetTRadius( fTRadius );
  }

}


void Cube4DCollection::RemoveRepeatedPMTs( ) {

  this->SortByOverlap();

  std::vector<std::pair< UInt_t, double > > hit_pmts = fCubeVec.at(0)->GetPMTs();

  for( size_t i_cube = 1; i_cube < this->GetNCubes(); i_cube++ ) {
    double cubeX = fCubeVec.at( i_cube )->GetX();
    double cubeY = fCubeVec.at( i_cube )->GetY();
    double cubeZ = fCubeVec.at( i_cube )->GetZ();
    double cubellh = fCubeVec.at( i_cube )->GetLLH();

    fCubeVec.at( i_cube )->RemovePMTs( hit_pmts );

    if ( cubellh != fCubeVec.at( i_cube )->GetLLH())
      this->SortByOverlap();

    if( abs(fCubeVec.at( i_cube )->GetX() - cubeX) < 0.1 && abs(fCubeVec.at( i_cube )->GetY() - cubeY) < 0.1 && abs(fCubeVec.at( i_cube )->GetZ() - cubeZ) < 0.1){
      std::vector<std::pair< UInt_t, double > > temp_vec = fCubeVec.at(i_cube)->GetPMTs();
      hit_pmts.insert( std::end(hit_pmts), std::begin(temp_vec), std::end(temp_vec) );
    }
    else
      i_cube = i_cube - 1;

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
