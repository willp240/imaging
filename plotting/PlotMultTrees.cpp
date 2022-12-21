#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"
#include "TTree.h"

#include <string>
#include <iostream>
#include <sstream>

void PlotMultTrees( ){

  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->SetRightMargin(0.12);
  TCanvas* c2 = new TCanvas("c2", "c2", 1600,800);
  c2->SetRightMargin(0.12);

  TH3D* h_t = new TH3D( "h_T", "Emission Time", 60, -6000, 6000, 60, -6000, 6000, 60, -6000, 6000 );
  h_t->GetXaxis()->SetTitle("X, mm  ");
  h_t->GetXaxis()->SetTitleOffset(1.5);
  h_t->GetYaxis()->SetTitle("Y, mm");
  h_t->GetYaxis()->SetTitleOffset(2.0);
  h_t->GetZaxis()->SetTitle("Z, mm");
  h_t->GetZaxis()->SetTitleOffset(1.3);
  h_t->SetLineWidth(0);
  TH3D* h_llh = new TH3D( "h_LLH", "Step Density", 60, -6000, 6000, 60, -6000, 6000, 60, -6000, 6000 );
  h_llh->GetXaxis()->SetTitle("X, mm  ");
  h_llh->GetXaxis()->SetTitleOffset(1.5);
  h_llh->GetYaxis()->SetTitle("Y, mm");
  h_llh->GetYaxis()->SetTitleOffset(2.0);
  h_llh->GetZaxis()->SetTitle("Z, mm");
  h_llh->GetZaxis()->SetTitleOffset(1.3);
  h_llh->SetLineWidth(0);

  for(int j=0; j<h_t->GetXaxis()->GetNbins(); j++){
    for(int k=0; k<h_t->GetYaxis()->GetNbins(); k++){
      for(int l=0; l<h_t->GetZaxis()->GetNbins(); l++){
        h_t->SetBinContent(j,k,l,0);
        h_llh->SetBinContent(j,k,l,0);
      }
    }
  }

  for(int i=236; i<316; i++){

    TString fname = Form("/data/snoplus/parkerw/imaging/dec6_fixedTmcmc/dec6_fixedTmcmc_%d.root", i);

    TFile *f = TFile::Open(fname);

    /// Load in the tree
    TTree* chain = (TTree*)f->Get("posteriors")->Clone("tree");


    double llh = 0;
    std::stringstream ss;
    ss << "Step > 20000";
    std::string cut = ss.str();
    std::cout << cut << std::endl;

    chain->Draw("X", cut.c_str() );
    TList *primitives = c2->GetListOfPrimitives();
    if(!primitives->FindObject("htemp"))
      continue;
    TH1 *hx = (TH1*)gPad->GetPrimitive("htemp");
    double x = hx->GetMean();
    double xllh = hx->GetMaximum();
    if(xllh>llh)
      llh = xllh;

    chain->Draw("Y", cut.c_str() );
    primitives = c2->GetListOfPrimitives();
    if(!primitives->FindObject("htemp"))
      continue;
    TH1 *hy = (TH1*)gPad->GetPrimitive("htemp");
    double y = hy->GetMean();
    double yllh = hy->GetMaximum();
    if(yllh>llh)
      llh = yllh;

    chain->Draw("Z", cut.c_str() );
    primitives = c2->GetListOfPrimitives();
    if(!primitives->FindObject("htemp"))
      continue;
    TH1 *hz = (TH1*)gPad->GetPrimitive("htemp");
    double z = hz->GetMean();
    double zllh = hz->GetMaximum();
    if(zllh>llh)
      llh = zllh;

    int bin = h_t->FindBin(x,y,z);
    if(llh > h_llh->GetBinContent(bin)) {
      if(llh>3300)
	h_t->SetBinContent(bin, i+0.5);
      h_llh->SetBinContent(bin, llh);
    }
  }

  TColor::InvertPalette();
  c1->cd();
  //h_t->GetXaxis()->SetRangeUser(-1000,1000);
  //h_t->GetYaxis()->SetRangeUser(-1000,1000);
  //h_t->GetZaxis()->SetRangeUser(-1000,1000);
  h_t->Draw("box2Z");
  c2->cd();
  //h_llh->GetXaxis()->SetRangeUser(-1000,1000);
  //h_llh->GetYaxis()->SetRangeUser(-1000,1000);
  //h_llh->GetZaxis()->SetRangeUser(-1000,1000);
  h_llh->Draw("box2Z");

  TFile *outfile = TFile::Open( "t_llPlots.root", "RECREATE");
  h_t->Write();
  h_llh->Write();
  
}
