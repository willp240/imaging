#include <vector>
#include <iostream>

#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include "TStopwatch.h"
#include <TCanvas.h>
#include <TRandom3.h>

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

  if (argc != 4) {
    std::cout << "Syntax is $: mcmc_fixedT inputfile outputfile t" << std::endl;
    exit(-1);
  }

  const std::string fname = argv[1];
  const std::string outfname = argv[2];
  const std::string tstring = argv[3];
  const double t = stod(tstring);
  const int numsteps = 100000;
  const bool debugMode = false;

  TFile *outfile = TFile::Open( outfname.c_str(), "RECREATE");

  TRandom3 rand = TRandom3();
  rand.SetSeed(0);
 
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

  // set up initial parameters and sigmas
  double currentl = 0, steptime, currentx = 12000*rand.Uniform()-6000, currenty = 12000*rand.Uniform()-6000, currentz = 12000*rand.Uniform()-6000, accrate = 0;
  int stepnum;
  bool accepted;
  double xsig = 500, ysig = 500, zsig = 500;

  while( sqrt(currentx*currentx + currenty*currenty + currentz*currentz) > 6000 ) {
    currentx = 12000*rand.Uniform()-6000;
    currenty = 12000*rand.Uniform()-6000;
    currentz = 12000*rand.Uniform()-6000;
  }

  // make output tree
  TTree *chain = new TTree("posteriors", "Posterior_Distributions");
  chain->Branch("LogL", &currentl, "LogL/D");
  chain->Branch("Accepted", &accepted, "Accepted/O");
  chain->Branch("Step", &stepnum, "Step/I");
  chain->Branch("StepTime", &steptime, "StepTime/D");
  chain->Branch("X", &currentx, "x/D");
  chain->Branch("Y", &currenty, "y/D");
  chain->Branch("Z", &currentz, "z/D");

  std::cout << "Fit Pos: " << fit_pos.X() << " " << fit_pos.Y() << " " << fit_pos.Z() << " " <<fit_pos.Mag() << std::endl;
  std::cout << "Fit Time: " << fit_time << std::endl;
  std::cout << "Fixed Time: " << t << std::endl;
  std::cout << "Start point: " << currentx << " " << currenty << " " << currentz << " " << t << " " << sqrt(currentx*currentx + currenty*currenty + currentz*currentz) << std::endl;

  TStopwatch stepClock;
  stepClock.Start();

  std::cout << std::endl;
  std::cout << "Starting  " << numsteps << " steps" << std::endl << std::endl;

  // start stepping
  for(size_t istep = 0; istep < numsteps; istep++){

    // propose a new step
    double propl = 0;
    double propt = t;
    double propx = currentx + rand.Gaus(0, xsig);
    double propy = currenty + rand.Gaus(0, ysig);
    double propz = currentz + rand.Gaus(0, zsig);
    TVector3 propPos(propx, propy, propz);

    while( propPos.Mag() > 6000 ) {
      propl = 0;
      propx = currentx + rand.Gaus(0, xsig);
      propy = currenty + rand.Gaus(0, ysig);
      propz = currentz + rand.Gaus(0, zsig);
      propPos.SetXYZ(propx, propy, propz);
    }

    // now loop over hits
    for(size_t j = 0; j < calibratedPMTs.GetCount(); j++)
      {  
	const RAT::DS::PMTCal& pmt = calibratedPMTs.GetPMT(j);
	const TVector3 pmtPos = pmtInfo.GetPosition( pmt.GetID() );
	
	double photondist = (propPos - pmtPos).Mag();
	
	double distInAV = 0.0;
	double distInWater = 0.0;
	double distInTarget = 0.0;
	RAT::LP::LightPathStraightScint::GetPath(pmtPos, propPos, distInTarget, distInWater);
	
	float transitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
	
	// calc time residuals
	double emisst1 = timeResCalc.CalcTimeResidual(pmt.GetID(), pmt.GetTime(), propPos, propt, false, 3.103125 * 1e-6, true, 0.0);
	
	// if time residual close to 0, add to llh
	if(emisst1 > -0.5 && emisst1 < 0.5){
	  propl += 1;
	}
	
      } // end hit loop
    
    // decide if we accept the step
    if(propl >= currentl)
      accepted = true;
    else if ( propl/currentl < rand.Uniform() )
      accepted = true;
    else
      accepted = false;
    
    if(debugMode || istep % 1000 == 0){
      std::cout << "Step " << istep << " proposed: " << std::endl;
      std::cout << "x = " << propx << std::endl;
      std::cout << "y = " << propy << std::endl;
      std::cout << "z = " << propz << std::endl;
      std::cout << "t = " << propt << std::endl;
      std::cout << "LLH " << propl << ", accepted? " << accepted << std::endl;
      std::cout << "Acceptance Rate " << accrate << std::endl << std::endl;
    }
    
    // update current parameter values
    if(accepted){
      currentx = propx;
      currenty = propy;
      currentz = propz;
      currentl = propl;
      accrate += 1/(double)numsteps;
    }

    stepnum = istep;
    steptime = stepClock.RealTime();
    stepClock.Start();
    chain->Fill();
  } // end step loop

  chain->Write();
  outfile->Close();
  
  std::cout << "Finished MCMC, final acceptance rate = " << accrate << std::endl; 

}

