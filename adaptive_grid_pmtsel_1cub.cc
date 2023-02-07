#include <TH3D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TCanvas.h>

#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DB.hh>

#include <Cube.hh>
#include <CubeCollection.hh>

void FindBestMiniCubes( CubeCollection* col, CubeCollection* &final_cube_col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs, int num_t, double init_cube_size_t, double min_t, double t_res, double res, int factor );

double CalcOverlap( CubeCollection* &col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs, int num_t, double init_cube_size_t, double min_t, double t_res );

std::vector< RAT::DS::PMTCal > RemovePMTs(std::vector< RAT::DS::PMTCal > pmt_vec, std::vector< RAT::DS::PMTCal > pmts_to_remove);

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
  double init_cube_rad = 500;
  int    init_num_cubes = floor( ( max_xyz - min_xyz ) / init_cube_rad );


  //// Adaptive grid parameters
  double t_res = 3.5;
  double res = 100;
  double factor = 10;
  int    num_mini_cubes = floor( ( max_xyz - min_xyz ) / res );

  //// Gonna make some histograms
  TH3D* h = new TH3D( "Overlap", "Overlap", num_mini_cubes, min_xyz, max_xyz, num_mini_cubes, min_xyz, max_xyz, num_mini_cubes, min_xyz, max_xyz );
  h->GetXaxis()->SetTitle("X, mm  ");
  h->GetXaxis()->SetTitleOffset(1.5);
  h->GetYaxis()->SetTitle("Y, mm");
  h->GetYaxis()->SetTitleOffset(2.0);
  h->GetZaxis()->SetTitle("Z, mm");
  h->GetZaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(0);
  TH3D* h_t = new TH3D( "EmissionT", "EmissionT", num_mini_cubes, min_xyz, max_xyz, num_mini_cubes, min_xyz, max_xyz, num_mini_cubes, min_xyz, max_xyz );
  h_t->GetXaxis()->SetTitle("X, mm  ");
  h_t->GetXaxis()->SetTitleOffset(1.5);
  h_t->GetYaxis()->SetTitle("Y, mm");
  h_t->GetYaxis()->SetTitleOffset(2.0);
  h_t->GetZaxis()->SetTitle("Z, mm");
  h_t->GetZaxis()->SetTitleOffset(1.3);
  h_t->SetLineWidth(0);

  CubeCollection* init_cube_col = new CubeCollection();

  // Loop xyz positions to make initial cubes
  for(double x = min_xyz + init_cube_rad; x < max_xyz; x += 2*init_cube_rad) {
    for(double y = min_xyz + init_cube_rad; y < max_xyz; y += 2*init_cube_rad) {
      for(double z = min_xyz + init_cube_rad; z < max_xyz; z += 2*init_cube_rad) {

        TVector3 cube_pos(x, y, z);
	      if( cube_pos.Mag() > 5500)
	        continue;

	      // Make cube
	      Cube* cub = new Cube( x, y, z, init_cube_rad );
	      // Get PMTs associated with the cube (at this point, all hit PMTs)
	      std::vector< RAT::DS::PMTCal > pmts;
	      for(size_t i_pmt = 0; i_pmt < calibrated_PMTs.GetCount(); i_pmt++) {
      	  RAT::DS::PMTCal pmt = calibrated_PMTs.GetPMT( i_pmt );
	        pmts.push_back( pmt );
	      }
	      cub->SetPMTs( pmts );

	      std::cout << "Adding initial cube at " << x << " " << y << " " << z << std::endl;

	      // Add it to collection
	      init_cube_col->AddCube( cub );
      }
    }
  }

  std::cout << std::endl;

  // Adaptive Grid on Cube Collection
  CubeCollection* final_cube_col = new CubeCollection();
  FindBestMiniCubes( init_cube_col, final_cube_col, pmt_info, time_res_calc, calibrated_PMTs, num_t, init_cube_size_t, min_t, t_res, res, factor );

  /// Now loop over cubes and fill histo
  for( int i_cube = 0; i_cube < final_cube_col->GetNCubes(); i_cube++ ){

    Cube* cub = final_cube_col->GetCube( i_cube );
    double cube_x = cub->GetX();
    double cube_y = cub->GetY();
    double cube_z = cub->GetZ();
    double cube_t = cub->GetT();
    double overlap = cub->GetLLH();

    // TO DO: make sure this bin isn't already filled. Maybe we don't need to

    if( overlap > 0 ){
      std::cout << "Filling " << cube_x << " " << cube_y << " " << cube_z << " " << overlap << " " << cube_t << std::endl;
      h->Fill( cube_x, cube_y, cube_z, overlap );
      h_t->Fill( cube_x, cube_y, cube_z, cube_t );
    }
  }

  TFile *out_file = TFile::Open( out_fname.c_str(), "RECREATE");

  h->Write();
  h_t->Write();

  out_file->Close();

}
 
// 
void FindBestMiniCubes( CubeCollection* col, CubeCollection* &final_cube_col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs, int num_t, double init_cube_size_t, double min_t, double t_res, double res, int factor ) {

std::vector< RAT::DS::PMTCal > pmt_vec = col->GetCube(0)->GetPMTs();

  while( pmt_vec.size() > 0){

    double best_global_overlap = CalcOverlap( col, pmt_info, time_res_calc, calibrated_PMTs, num_t, init_cube_size_t, min_t, t_res );

    col->SortByOverlap();

    Cube* cub = col->GetCube( 0 );
    CubeCollection* new_col = cub->Divide( factor );
    //TO DO: set cub collection pmts here?
    new_col->SetPMTs(pmt_vec);

    //TO DO: also, account for case where maybe a hit doesn't look good from any cube (maybe a crazy scatter)

    t_res = t_res / factor;
    best_global_overlap = CalcOverlap( new_col, pmt_info, time_res_calc, calibrated_PMTs, num_t, init_cube_size_t, min_t, t_res );
    new_col->SortByOverlap();
    t_res = t_res * factor;

    Cube* mini_cub = new_col->GetCube( 0 );
    double cube_x = mini_cub->GetX();
    double cube_y = mini_cub->GetY();
    double cube_z = mini_cub->GetZ();

    std::cout << "Adding final cubes " << cube_x << " " << cube_y << " " << cube_z << std::endl;
    final_cube_col->AddCube( mini_cub );

    // Get PMTs of cube
    std::vector< RAT::DS::PMTCal > pmt_vec_cub = mini_cub->GetPMTs();
    // Remove these from list
    std::vector< RAT::DS::PMTCal > updated_pmt_vec = RemovePMTs(pmt_vec, pmt_vec_cub);
  }

}


double CalcOverlap( CubeCollection* &col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs, int num_t, double init_cube_size_t, double min_t, double t_res ) {

  // Loop Cubes
  for( int i_cube = 0; i_cube < col->GetNCubes(); i_cube++ ){

    Cube* cub = col->GetCube( i_cube );
    double cube_x = cub->GetX();
    double cube_y = cub->GetY();
    double cube_z = cub->GetZ();
    TVector3 cube_pos( cube_x, cube_y, cube_z );
    
    double best_t_overlap = -999;
    double best_t = -999;
    std::vector< RAT::DS::PMTCal > best_pmt_list;
    
    // Loop over emission times and count overlap to find best fit
    for(int t = 0; t < num_t; t++){
      double cube_t = (t * init_cube_size_t) + min_t;
      double overlap = 0;
      std::vector< RAT::DS::PMTCal > pmt_list;
      
      // Now loop over hits
      std::vector< RAT::DS::PMTCal > pmts = cub->GetPMTs();
      for( size_t i_pmt = 0; i_pmt < pmts.size(); i_pmt++ ) {
	
      	// Loop PMTs & get LLH
	      const RAT::DS::PMTCal pmt = pmts.at( i_pmt );
	      double emission_t = time_res_calc.CalcTimeResidual( pmt.GetID(), pmt.GetTime(), cube_pos, cube_t, false, 3.103125 * 1e-6, true, 0.0 );
	
	      if(emission_t > -t_res && emission_t < t_res){
	        // If we have time residual close to 0, add one to overlap, and save PMT so it's remains associated with the cube
	        overlap++;
	        pmt_list.push_back( pmt );
	      }
	
      } // End loop over hits
      
      // Is it the best time for the cube so far?
      if(overlap > best_t_overlap) {
	      best_t_overlap = overlap;
	      best_pmt_list = pmt_list;
	      best_t = cube_t;
      }
      
    } // End loop over time offsets
    
    std::cout << "Cube at " << cube_x << " " << cube_y << " " << cube_z << " best overlap " << best_t_overlap << " at " << best_t << std::endl;
    
    // Store overlap PMT IDs
    cub->SetLLH( best_t_overlap );
    cub->SetPMTs( best_pmt_list );
    cub->SetT( best_t );
    
  } // End loop over cubes

}


std::vector< RAT::DS::PMTCal > RemovePMTs(std::vector< RAT::DS::PMTCal > pmt_vec, std::vector< RAT::DS::PMTCal > pmts_to_remove) {

  std::vector<double> pmt_ids;
  for( int i_pmt = 0; i_pmt < pmts_to_remove.size(); i_pmt++ ){
    pmt_ids.push_back( pmts_to_remove.at( i_pmt ).GetID() );
  }

  for( int i_pmt = 0; i_pmt < pmt_vec.size(); i_pmt++ ){
    double id = pmts_to_remove.at(i_pmt).GetID();
    if( std::find(pmt_ids.begin(), pmt_ids.end(), id) != pmt_ids.end() ){
      pmt_vec.erase( pmt_vec.begin() + i_pmt );
      i_pmt--;
    }	
  }

  std::vector< RAT::DS::PMTCal > updated_vec = pmt_vec;
  return updated_vec;
}