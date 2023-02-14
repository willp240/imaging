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

  const double c = 299.792; // speed of light in mm / ns

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
  std::cout << "True Initial Pos: " << mc.GetMCParticle(0).GetPosition().X() << " " << mc.GetMCParticle(0).GetPosition().Y() << " " << mc.GetMCParticle(0).GetPosition().Z() << std::endl;
  std::cout << "True Initial Mom: " << mc.GetMCParticle(0).GetMomentum().X() << " " << mc.GetMCParticle(0).GetMomentum().Y() << " " << mc.GetMCParticle(0).GetMomentum().Z() << std::endl;
  const RAT::DS::FitVertex& rVertex = rEV.GetFitResult("scintFitter").GetVertex(0);
  const TVector3 fit_pos = rVertex.GetPosition();
  const double fit_time = rVertex.GetTime();

  const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();

  // calc a "true" position of entry
  const TVector3 true_pos(0,0,4730);
  const double true_time = 3770 / 299.792;

  //// Set up the #cube
  double cubesizex = 1000;
  double cubesizey = 1000;
  double cubesizez = 50;
  double cubesizet = 1;

  int    num_cubes = 3;
  int    num_t0    = 3;

  double minx = fit_pos.X() - (num_cubes/(double)2) * cubesizex;
  double maxx = fit_pos.X() + (num_cubes/(double)2) * cubesizex;
  double miny = fit_pos.Y() - (num_cubes/(double)2) * cubesizey;
  double maxy = fit_pos.Y() + (num_cubes/(double)2) * cubesizey;
  double minz = fit_pos.Z() - (num_cubes) * cubesizez;
  double maxz = fit_pos.Z();
  double mint = fit_time - (num_t0/(double)2) * cubesizet;
  double maxt = fit_time + (num_t0/(double)2) * cubesizet;

  //  TH3D* h3 = new TH3D( "h3", "h3", 10*num_cubes, minx, maxx, 10*num_cubes, miny, maxy, 10*num_cubes, minz, maxz );
  TH3D* hists[30];
  for(int i=0; i<num_t0; i++){
    TString hname = Form("h_%f", mint+num_t0*i);
    hists[i] = new TH3D( hname, hname, 10*num_cubes, minx, maxx, 10*num_cubes, miny, maxy, 10*num_cubes, minz, maxz );
    hists[i]->GetXaxis()->SetTitle("X, mm  ");
    hists[i]->GetXaxis()->SetTitleOffset(1.5);
    hists[i]->GetYaxis()->SetTitle("Y, mm");
    hists[i]->GetYaxis()->SetTitleOffset(2.0);
    hists[i]->GetZaxis()->SetTitle("Z, mm");
    hists[i]->GetZaxis()->SetTitleOffset(1.3);
    hists[i]->SetLineWidth(0);
  }

  std::cout<< "'True' Pos: " << true_pos.X() << " " << true_pos.Y() << " " << true_pos.Z() << " " << true_pos.Mag() << std::endl;
  std::cout << "Fit Pos: " << fit_pos.X() << " " << fit_pos.Y() << " " << fit_pos.Z() << " " <<fit_pos.Mag() << std::endl;
  std::cout << "'True' Time: " << true_time << ", Fit Time: " << fit_time << std::endl;

  std::cout << "Histogram Bounds: " << minx << " " << maxx << ", " << miny << " " << maxy << ", " << minz << " " << maxz << std::endl;
  int count = 0;

  // now loop over hits
  for(size_t j = 0; j < calibratedPMTs.GetCount(); j++)
    {  
      const RAT::DS::PMTCal& pmt = calibratedPMTs.GetPMT(j);
      const TVector3 pmtPos = pmtInfo.GetPosition( pmt.GetID() );

      // and loop over cubes
      for(int t = 0; t < num_t0; t++){
	double cubet = (t * cubesizet) + mint;

	for(int x = 0; x < num_cubes; x++){
	  for(int y = 0; y < num_cubes; y++){
	    for(int z = 0; z < num_cubes; z++){

	      double cubex = (x * cubesizex) + minx + cubesizex/2;
	      double cubey = (y * cubesizey) + miny + cubesizey/2;
	      double cubez = (z * cubesizez) + minz + cubesizez/2;
	      TVector3 cubePos(cubex, cubey, cubez);

	      double muondist = (cubePos - fit_pos).Mag();
	      double photondist = (cubePos - pmtPos).Mag();

	      double muont = muondist / c;

	      double distInAV = 0.0;
	      double distInWater = 0.0;
	      double distInTarget = 0.0;
	      RAT::LP::LightPathStraightScint::GetPath(pmtPos, cubePos, distInTarget, distInWater);

	      float transitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );

	      //double emisst1 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, true_time + muont + 275, false, 3.103125 * 1e-6, true, 0.0);//275.2
	      //double emisst2 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, true_time - muont + 275, false, 3.103125 * 1e-6, true, 0.0);//275.2

	      //double emisst1 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, fit_time + 12.5 + muont, false, 3.103125 * 1e-6, true, 0.0);
              //double emisst2 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, fit_time + 12.5 - muont, false, 3.103125 * 1e-6, true, 0.0);

	      double emisst1 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, cubet + muont, false, 3.103125 * 1e-6, true, 0.0);
              double emisst2 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), cubePos, cubet - muont, false, 3.103125 * 1e-6, true, 0.0);

	      if(emisst1 > -0.5 && emisst1 < 0.5){
		hists[t]->Fill(cubex, cubey, cubez);
	      }
	      else if(emisst2 > -0.5 && emisst2 < 0.5){
		hists[t]->Fill(cubex, cubey, cubez);
	      }

	    }
	  }
	}
      }

    }

  TFile *outfile = TFile::Open( outfname.c_str(), "RECREATE");
  for(int i=0; i<num_t0; i++){
    hists[i]->Write();
    std::cout << hists[i]->Integral() << std::endl;
  }
  outfile->Close();
  //  TCanvas* c1 = new TCanvas("c1","c1");
  //c1->SetRightMargin(0.13);
  //h3->Draw("box2Z");
}

