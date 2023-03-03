#include <TH3D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TCanvas.h>
#include <memory>

#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DB.hh>

#include <Cube.hh>
#include <CubeCollection.hh>

int main( int argc, char **argv ) {

  if (argc != 3) {
    std::cout << "Syntax is $: adaptive_grid inputfile outputfile" << std::endl;
    exit(-1);
  }

  const std::string fname = argv[1];
  const std::string out_fname = argv[2];

  //// Open file
  RAT::DU::DSReader ds_reader( fname.c_str() );

  //// RAT begin of runs etc
  RAT::DU::PMTInfo pmt_info = RAT::DU::Utility::Get()->GetPMTInfo();
  RAT::DU::TimeResidualCalculator time_res_calc = RAT::DU::Utility::Get()->GetTimeResidualCalculator();

  // Get event and fit vertex
  const RAT::DS::Entry& r_DS = ds_reader.GetEntry( 0 );
  const RAT::DS::MC& mc = r_DS.GetMC();
  const RAT::DS::EV& r_Ev = r_DS.GetEV( 0 );

  const RAT::DS::FitVertex& r_vertex = r_Ev.GetFitResult("scintFitter").GetVertex(0);
  const TVector3 fit_pos = r_vertex.GetPosition();
  const double fit_time = r_vertex.GetTime();

  RAT::DS::CalPMTs calibrated_PMTs = r_Ev.GetCalPMTs();

  //// Set up the #cube
  int    num_t = 80;
  double init_cube_size_t = 1;
  double min_t = fit_time - (num_t/2);

  double min_xyz = -5500;
  double max_xyz = 5500;
  double init_cube_rad = 250;


  CubeCollection* init_cube_col = new CubeCollection();

  std::vector<Cube> cube_vec;
  // Loop xyz positions to make initial cubes
  for(double x = min_xyz + init_cube_rad; x < max_xyz; x += 2*init_cube_rad) {
    for(double y = min_xyz + init_cube_rad; y < max_xyz; y += 2*init_cube_rad) {
      for(double z = min_xyz + init_cube_rad; z < max_xyz; z += 2*init_cube_rad) {

	TVector3 cube_pos(x, y, z);
	if( cube_pos.Mag() > 6000)
	  continue;

	// Make cube
	Cube cub( x, y, z, init_cube_rad );
	// Get PMTs associated with the cube (at this point, all hit PMTs)
	std::vector< RAT::DS::PMTCal > pmts;
	for(size_t i_pmt = 0; i_pmt < calibrated_PMTs.GetCount(); i_pmt++) {
	  RAT::DS::PMTCal pmt = calibrated_PMTs.GetPMT( i_pmt );
	  pmts.push_back( pmt );
	}
	//	cub.SetPMTs( pmts );
	
	std::cout << "Adding initial cube at " << x << " " << y << " " << z << std::endl;

	cube_vec.push_back( cub );

	// Add it to collection
	//init_cube_col->AddCube( cub );
	//	delete cub;
      }
    }
  }

  std::cout << std::endl;
}
