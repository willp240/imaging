////////////////////////////////////////////////////////////////////////
/// \class CubeCollection
////////////////////////////////////////////////////////////////////////
#ifndef __CUBECOLLECTION__
#define __CUBECOLLECTION__

#include <TVector3.h>
#include <iostream>

#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>

#include <Cube.hh>


class Cube;

class CubeCollection{

  public:
  CubeCollection( ) {};

  void AddCube( Cube* cube_ ) { fCubeVec.push_back( cube_ ); }
  Cube* GetCube( int i_ ) { return fCubeVec.at( i_ ); }
  int GetNCubes() { return fCubeVec.size(); }
  double GetMaxLLH();
  int GetMaxLLHCube();
  void SetPMTs( std::vector< RAT::DS::PMTCal > pmts_ );
  std::vector< RAT::DS::PMTCal > GetPMTs() { return fPMTs; }
  void SortByOverlap();
  void RemoveRepeatedPMTs();
  void RemovePMTs( std::vector< RAT::DS::PMTCal > pmts );

  private:
  ////// Cube Details
  std::vector<Cube*> fCubeVec;
  double fMaxLLH;
  int fMaxLLHCube;
  std::vector< RAT::DS::PMTCal > fPMTs;

};
#endif // __CUBECOLLECTION__
