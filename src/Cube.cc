#include <Cube.hh>
#include <CubeCollection.hh>

CubeCollection* Cube::Divide( int factor ){

  CubeCollection* col = new CubeCollection();

  double new_radius = fRadius/factor;
  double x_min = fX - fRadius;
  double y_min = fY - fRadius;
  double z_min = fZ - fRadius;

  for(int i_x = 1; i_x <= 2*factor; i_x += 2) {
    for(int i_y = 1; i_y <= 2*factor; i_y += 2) {
      for(int i_z = 1; i_z <= 2*factor; i_z += 2 ) {
	
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
