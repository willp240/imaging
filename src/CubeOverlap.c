#include <Cube.hh>
#include <CubeCollection.hh>

CubeCollection* Cube::Divide( int factor ){

  CubeCollection* col = new CubeCollection();

  double new_radius = fRadius/factor;
  double x_min = fX - fRadius;
  double y_min = fY - fRadius;
  double z_min = fZ - fRadius;

  for(int i_x = 1; i_x <= 2*factor; i_x += 1) {
    for(int i_y = 1; i_y <= 2*factor; i_y += 1) {
      for(int i_z = 1; i_z <= 2*factor; i_z += 1) {
	
	double x = x_min + i_x*new_radius;
	double y = y_min + i_y*new_radius;
	double z = z_min + i_z*new_radius;

	if( sqrt(x*x+y*y+z*z) > 6000 )
	  continue;

	Cube* new_cube = new Cube( x, y, z, new_radius );
	col->AddCube( new_cube );
      }
    }
  }

  return col;
}


void Cube::RemovePMTs( std::vector< RAT::DS::PMTCal > pmts_ ){

  std::vector<double> pmt_ids;
  for( int i_pmt = 0; i_pmt < pmts_.size(); i_pmt++ ){
    pmt_ids.push_back( pmts_.at( i_pmt ).GetID() );
  }

  for( int i_pmt = 0; i_pmt < fPMTs.size(); i_pmt++ ){
    double id = fPMTs.at(i_pmt).GetID();
    if( std::find(pmt_ids.begin(), pmt_ids.end(), id) != pmt_ids.end() ){
      fPMTs.erase( fPMTs.begin() + i_pmt );
      i_pmt--;
    }	
  }

}
