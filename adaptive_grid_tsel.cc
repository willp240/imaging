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

#include <Cube4D.hh>
#include <Cube4DCollection.hh>

void AdapGrid( Cube4DCollection* col, Cube4DCollection* &final_cube_col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs, std::vector< std::pair < UInt_t, double > > pmts, double min_t, double max_t, double init_cube_rad_t, double res, int factor, double hit_cut );

double CalcOverlap( Cube4DCollection* &col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs );

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

  //// Get event and fit vertex
  const RAT::DS::Entry& r_DS = ds_reader.GetEntry( 0 );
  const RAT::DS::MC& mc = r_DS.GetMC();
  const RAT::DS::EV& r_Ev = r_DS.GetEV( 0 );

  const RAT::DS::FitVertex& r_vertex = r_Ev.GetFitResult("scintFitter").GetVertex(0);
  const TVector3 fit_pos = r_vertex.GetPosition();
  const double fit_time = r_vertex.GetTime();

  RAT::DS::CalPMTs calibrated_PMTs = r_Ev.GetCalPMTs();

  //// Set up the #cube
  int    init_num_t = 32;
  double init_cube_rad_t = 1.5;
  double min_t = fit_time - 2*init_cube_rad_t*(init_num_t/2);
  double max_t = fit_time + 2*init_cube_rad_t*(init_num_t/2);

  double min_xyz = -5500;
  double max_xyz = 5500;
  double init_cube_rad = 500;
  int    init_num_cubes = floor( ( max_xyz - min_xyz ) / 2*init_cube_rad );


  //// Adaptive grid parameters
  double res = 150;
  double factor = 10;
  int    num_mini_cubes = floor( ( max_xyz - min_xyz ) / 100 );
  int    num_t = floor( ( max_t - min_t ) / 0.3 );
  double hit_cut = 5000;

  std::cout << std::endl;

  //// Make vector of pmts to assign to each initial cube
	std::vector< std::pair < UInt_t, double > > pmts;
	for(size_t i_pmt = 0; i_pmt < calibrated_PMTs.GetCount(); i_pmt++) {
	  RAT::DS::PMTCal pmt = calibrated_PMTs.GetPMT( i_pmt );
    UInt_t PMT_ID = pmt.GetID();
    double PMT_t = pmt.GetTime();
    std::pair < UInt_t, double > pmt_pair( PMT_ID, PMT_t);
    pmts.push_back( pmt_pair );
	}

  std::cout << "xyz: " << min_xyz << " " << max_xyz << std::endl;
  std::cout << "t: " << min_t << " " << max_t << std::endl;

  Cube4DCollection* init_cube_col = new Cube4DCollection;

  //// Loop xyz positions to make initial cubes
  for(double x = min_xyz + init_cube_rad; x < max_xyz; x += 2*init_cube_rad) {
    for(double y = min_xyz + init_cube_rad; y < max_xyz; y += 2*init_cube_rad) {
      for(double z = min_xyz + init_cube_rad; z < max_xyz; z += 2*init_cube_rad) {

	      TVector3 cube_pos(x, y, z);
	      if( cube_pos.Mag() > 5500)
	        continue;

	      //// Make cube
	      Cube4D* cub = new Cube4D( x, y, z, init_cube_rad );

	      //// Get PMTs associated with the cube (at this point, all hit PMTs)
	      cub->SetPMTs( pmts );

	      // std::cout << "Adding initial cube at " << x << " " << y << " " << z << std::endl;

	      //// Add it to collection
	      init_cube_col->AddCube( cub );
      }
    }
  } //// End loop over xyz of cubes


  //// Adaptive Grid on Cube Collection
  Cube4DCollection* final_cube_col = new Cube4DCollection();
  AdapGrid( init_cube_col, final_cube_col, pmt_info, time_res_calc, calibrated_PMTs, pmts, min_t, max_t, init_cube_rad_t, res, factor, hit_cut );

  //// Gonna make some histograms
  TH3D* hists[num_t];
  for(int i=0; i<num_t; i++){
    TString hname = Form("h_%d",i);
    TString htitle = Form("h_%f", min_t + 2*init_cube_rad_t*i);
    hists[i] = new TH3D( hname, htitle, num_mini_cubes, min_xyz, max_xyz, num_mini_cubes, min_xyz, max_xyz, num_mini_cubes, min_xyz, max_xyz );
    hists[i]->GetXaxis()->SetTitle("X, mm  ");
    hists[i]->GetXaxis()->SetTitleOffset(1.5);
    hists[i]->GetYaxis()->SetTitle("Y, mm");
    hists[i]->GetYaxis()->SetTitleOffset(2.0);
    hists[i]->GetZaxis()->SetTitle("Z, mm");
    hists[i]->GetZaxis()->SetTitleOffset(1.3);
    hists[i]->SetLineWidth(0);
  }

  //// Now loop over cubes and fill histo
  for( int i_cube = 0; i_cube < final_cube_col->GetNCubes(); i_cube++ ){

    Cube4D* cub = final_cube_col->GetCube( i_cube );
    double cube_x = cub->GetX();
    double cube_y = cub->GetY();
    double cube_z = cub->GetZ();
    double cube_t = cub->GetT();
    double overlap = cub->GetLLH();
    //std::cout << "looping " << i_cube << " of " << final_cube_col->GetNCubes() << " " << cub->GetLLH() << std::endl;
    //std::cout << cube_x << " " << cube_y << " " << cube_z << " " << cube_t << " " << overlap << std::endl; 
    //// Fill histogram for this time slice if we have some density
    if( overlap > 0 ){
      int hist_num = floor((cube_t - min_t)/0.3);
      if(hist_num >= num_t){
        std::cout << "WARNING: calculated hist num " << hist_num << " setting to" << num_t-1 << std::endl;
        hist_num = num_t - 1;
      }
      int bin_number = hists[hist_num]->FindBin(cube_x, cube_y, cube_z);
      if(overlap > hists[hist_num]->GetBinContent(bin_number));{
        std::cout << "Filling " << cube_x << " " << cube_y << " " << cube_z << " " << overlap << " " << cube_t << " " << hist_num << std::endl;
        hists[hist_num]->SetBinContent( bin_number, overlap );
      }
    }
  }

  delete final_cube_col;
  delete init_cube_col;

  //// Write all histograms to file
  TFile *out_file = TFile::Open( out_fname.c_str(), "RECREATE");
  for(int i=0; i<num_t; i++){
    hists[i]->Write();
    std::cout << hists[i]->Integral() << std::endl;
  }

  out_file->Close();
}


//// Function to recursively perform the adaptive grid
void AdapGrid( Cube4DCollection* init_cube_col, Cube4DCollection* &final_cube_col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs, std::vector< std::pair < UInt_t, double > > pmts, double min_t, double max_t, double init_cube_rad_t, double res, int factor, double hit_cut ) {

  for(double t = min_t + init_cube_rad_t; t < max_t; t += 2*init_cube_rad_t){ 
    std::cout << "Adap grid for " << t << std::endl;

    Cube4DCollection* col = new Cube4DCollection(*init_cube_col);
    col->SetT(t);
    col->SetTRadius(init_cube_rad_t);
    col->SetPMTs(pmts);

    double best_global_overlap = CalcOverlap( col, pmt_info, time_res_calc, calibrated_PMTs );

    col->RemoveRepeatedPMTs();

    best_global_overlap = CalcOverlap( col, pmt_info, time_res_calc, calibrated_PMTs );

    //// Loop Cubes
    for( int i_cube = 0; i_cube < col->GetNCubes(); i_cube++ ){
      // std::cout << "final loop " << i_cube << " out of " << col->GetNCubes() << std::endl;
      Cube4D* cub = new Cube4D(*col->GetCube( i_cube ));
      double cube_x = cub->GetX();
      double cube_y = cub->GetY();
      double cube_z = cub->GetZ();
      double cube_r = cub->GetRadius();

      //std::cout << cube_x << " " << cube_y << " " << cube_z << " " << cub->GetPMTs().size() << std::endl;

      //// If we're above the resolution, we might want to divide the cube into subcubes
      if( cube_r > res && best_global_overlap > hit_cut ){
      
        //// If llh > 50% best
        if( cub->GetLLH() > 0 ){ // 0.5*best_global_overlap ) {
	        Cube4DCollection* new_col = cub->Divide( factor );

	        //// Each new cube has same associated PMTs as the parent bigger cube
	        new_col->SetPMTs( cub->GetPMTs() );
	
          double new_rad_t = 2*init_cube_rad_t / factor;
          double new_min_t = t - init_cube_rad_t;
          double new_max_t = t + init_cube_rad_t;

	        std::cout << "Dividing cube " << cube_x << " " << cube_y << " " << cube_z << std::endl;
	        //std::cout << "\t starts " << cube_x - cube_r << " " << cube_y - cube_r << " " << cube_z - cube_r << std::endl;
	        //std::cout << "\t end " << cube_x + cube_r <<" " << cube_y + cube_r << " " << cube_z + cube_r<< std::endl;	

	        //// Rerun adaptive grid on new collection
	        AdapGrid( new_col, final_cube_col, pmt_info, time_res_calc, calibrated_PMTs, pmts, new_min_t, new_max_t, new_rad_t, res, factor, hit_cut/factor );
        }
      // std::cout << "ending journey " << cube_x << " " << cube_y << " " << cube_z << std::endl;
      }
      else { 
        //std::cout << "Adding final cubes " << cube_x << " " << cube_y << " " << cube_z << " " << cub->GetLLH() << std::endl;
        final_cube_col->AddCube( cub );
      }
   } //// End 2nd loop over cubes
    delete col;
  } //// End loop over t
}


//// Function to calculate overlap for cubes in a cube collection
double CalcOverlap( Cube4DCollection* &col, RAT::DU::PMTInfo pmt_info, RAT::DU::TimeResidualCalculator time_res_calc, RAT::DS::CalPMTs calibrated_PMTs ) {

  double best_global_overlap = 0;

  //// Loop Cubes
  for( int i_cube = 0; i_cube < col->GetNCubes(); i_cube++ ){
    Cube4D* cub = col->GetCube( i_cube );
    double cube_x = cub->GetX();
    double cube_y = cub->GetY();
    double cube_z = cub->GetZ();
    TVector3 cube_pos( cube_x, cube_y, cube_z );
    double cube_t = cub->GetT();
    double cube_rad_t = cub->GetTRadius();

    double overlap = 0;
    std::vector<std::pair< UInt_t, double > > pmt_list;

    //// Now loop over hits
    std::vector< std::pair< UInt_t, double > > pmts = cub->GetPMTs();
    for( size_t i_pmt = 0; i_pmt < pmts.size(); i_pmt++ ) {
	    //// Loop PMTs & get LLH
	    const std::pair< UInt_t, double > pmt_pair = pmts.at(i_pmt);
	    double emission_t = time_res_calc.CalcTimeResidual( pmt_pair.first, pmt_pair.second, cube_pos, cube_t, false, 3.103125 * 1e-6, true, 0.0 );
	
	    if(emission_t > -cube_rad_t && emission_t < cube_rad_t){
	      //// If we have time residual close to 0, add one to overlap, and save PMT so it's remains associated with the cube
	      overlap++;
	      pmt_list.push_back( pmt_pair );
	    }
    }  //// End loop over hits
      
   // std::cout << "Cube at " << cube_x << " " << cube_y << " " << cube_z << " overlap " << overlap << " at " << cube_t << std::endl;
    
    //// Store overlap PMT IDs
    cub->SetLLH( overlap );
    cub->SetPMTs( pmt_list );
    
    if( overlap > best_global_overlap )
      best_global_overlap = overlap;

  } //// End loop over cubes
  return best_global_overlap;

}
