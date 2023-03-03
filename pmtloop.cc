#include <TH3D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TCanvas.h>

#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/EffectiveVelocity.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/LightPathStraightScint.hh>
#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCTrack.hh>
#include <RAT/DS/MCTrackStep.hh>
#include <RAT/DB.hh>

int main( int argc, char **argv ) {

  if (argc != 3) {
    std::cout << "Syntax is $: imaging inputfile outputfile" << std::endl;
    exit(-1);
  }

  const std::string fname = argv[1];
  const std::string outfname = argv[2];

  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->SetRightMargin(0.12);
  c1->Print("testCanvas.pdf[");

  //// open file
  RAT::DU::DSReader dsReader( fname.c_str() );

  //// rat begin of runs etc
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator();
  const RAT::DU::EffectiveVelocity& effectiveVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity();
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  RAT::LP::LightPathStraightScint::BeginOfRun();
  RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator();

  // get event and fit vertex
  const RAT::DS::Entry& rDS = dsReader.GetEntry( 0 );
  const RAT::DS::MC& mc = rDS.GetMC();
  const RAT::DS::EV& rEV = rDS.GetEV( 0 );

  const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();

  //// Set up the #cube
  double min_xyz = -5500;
  double max_xyz = 5500;
  double init_cube_rad = 500;
  int    init_num_cubes = floor( ( max_xyz - min_xyz ) / (2*init_cube_rad) );
  std::cout << min_xyz << " " << max_xyz << " " << init_num_cubes << std::endl;
  //// Gonna make some histograms
  TH3D* h = new TH3D( "Overlap", "Overlap", init_num_cubes, min_xyz, max_xyz, init_num_cubes, min_xyz, max_xyz, init_num_cubes, min_xyz, max_xyz );
  h->GetXaxis()->SetTitle("X, mm  ");
  h->GetXaxis()->SetTitleOffset(1.5);
  h->GetYaxis()->SetTitle("Y, mm");
  h->GetYaxis()->SetTitleOffset(2.0);
  h->GetZaxis()->SetTitle("Z, mm");
  h->GetZaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(0);

  // now loop over hits
  for(size_t j = 0; j < 5; j++)//calibratedPMTs.GetCount(); j++)
    {  
      const RAT::DS::PMTCal& pmt = calibratedPMTs.GetPMT(j);
      const TVector3 pmtPos = pmtInfo.GetPosition( pmt.GetID() );
      int pmtid = pmt.GetID();

      TString h_name = Form("h_pmt_%d", pmtid);

      TH3D* h_best_t = (TH3D*)h->Clone(h_name);
      h_best_t->Reset("");

      // and loop over cubes
      for(int t = 200; t < 330; t++){
	      double cubet = t;

        //TString h_temp_name = "h_temp";
        //TH3D* h_temp = (TH3D*)h->Clone(h_temp_name);
        //h_temp->Reset("");

	      for(int x = 0; x < init_num_cubes; x++){
	        for(int y = 0; y < init_num_cubes; y++){
	          for(int z = 0; z < init_num_cubes; z++){
              
	            double cubex = (2*x * init_cube_rad) + min_xyz + init_cube_rad;
	            double cubey = (2*y * init_cube_rad) + min_xyz + init_cube_rad;
	            double cubez = (2*z * init_cube_rad) + min_xyz + init_cube_rad;
	            TVector3 cubePos(cubex, cubey, cubez);
              //std::cout << cubex << " " << cubey << " " << cubez << std::endl;
	            double distInAV = 0.0;
	            double distInWater = 0.0;
	            double distInTarget = 0.0;
	            RAT::LP::LightPathStraightScint::GetPath(pmtPos, cubePos, distInTarget, distInWater);

	            float transitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );

      	      double emisst1 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, cubet, false, 3.103125 * 1e-6, true, 0.0);

	            if(emisst1 > -0.5 && emisst1 < 0.5){
                int binnum = h_best_t->FindBin(cubex, cubey, cubez);
                h_best_t->SetBinContent(binnum, 1.0);
		            //h_best_t->Fill(cubex, cubey, cubez);
              }
	          }
	        }
	      }
        //std::cout << j << " " << t << " " << h_temp->Integral() << std::endl;
        //if( h_temp->Integral() > h_best_t->Integral()){
          //h_best_t = (TH3D*)h_temp->Clone(h_name);
        //}
      }
      h_best_t->Draw("box2z");
      c1->Print("testCanvas.pdf");
      h->Add(h_best_t);
    }

  h->Draw("box2z");
  c1->Print("testCanvas.pdf");
  c1->Print("testCanvas.pdf]");
}

