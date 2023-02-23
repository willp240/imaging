#include <Cube4D.hh>
#include <Cube4DCollection.hh>

Cube4DCollection* Cube4D::Divide( int factor ){

  Cube4DCollection* col = new Cube4DCollection();

  double new_radius = fRadius/factor;
  double x_min = fX - fRadius;
  double y_min = fY - fRadius;
  double z_min = fZ - fRadius;
  double t_min = fT - fTRadius;
  double new_radius_t = fTRadius/factor;

  for(int i_t = 1; i_t <= 2*factor; i_t += 2) {
    for(int i_x = 1; i_x <= 2*factor; i_x += 2) {
      for(int i_y = 1; i_y <= 2*factor; i_y += 2) {
        for(int i_z = 1; i_z <= 2*factor; i_z += 2 ) {

  	      double x = x_min + i_x*new_radius;
	        double y = y_min + i_y*new_radius;
	        double z = z_min + i_z*new_radius;
          double t = t_min + i_t*new_radius_t;

	        if( sqrt(x*x+y*y+z*z) > 5500 )
	          continue;

        	Cube4D* new_cube = new Cube4D( x, y, z, new_radius, t, new_radius_t );
	        col->AddCube( new_cube );
       }
     }
    }
  }

  return col;
}


void Cube4D::RemovePMTs( std::vector< std::pair< UInt_t, double > > pmts_ ){

  std::vector<double> pmt_ids;
  for( int i_pmt = 0; i_pmt < pmts_.size(); i_pmt++ ){
    pmt_ids.push_back( pmts_.at( i_pmt ).first );
  }

  for( int i_pmt = 0; i_pmt < fPMTs.size(); i_pmt++ ){
    double id = fPMTs.at(i_pmt).first;
    if( std::find(pmt_ids.begin(), pmt_ids.end(), id) != pmt_ids.end() ){
      fPMTs.erase( fPMTs.begin() + i_pmt );
      i_pmt--;
    }	
  }

}
