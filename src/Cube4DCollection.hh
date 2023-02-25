////////////////////////////////////////////////////////////////////////
/// \class Cube4DCollection
////////////////////////////////////////////////////////////////////////
#ifndef __CUBE4DCOLLECTION__
#define __CUBE4DCOLLECTION__

#include <TVector3.h>
#include <iostream>

#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>

#include <Cube4D.hh>


class Cube4D;

class Cube4DCollection{

  public:
  Cube4DCollection( ) {};
   ~Cube4DCollection();

  void AddCube( Cube4D* cube_ ) { fCubeVec.push_back( cube_ ); }
  Cube4D* GetCube( int i_ ) { return fCubeVec.at( i_ ); }
  int GetNCubes() { return fCubeVec.size(); }
  double GetMaxLLH();
  int GetMaxLLHCube();
  void SetPMTs( std::vector< std::pair< UInt_t, double > > pmts_ );
  std::vector< std::pair< UInt_t, double > > GetPMTs() { return fPMTs; }
  void SetT( double t_ );
  double GetT() { return fT; }
  void SetTRadius( double trad_ );
  double GetTRadius() { return fTRadius; }
  void SortByOverlap();
  void RemoveRepeatedPMTs();
  void RemovePMTs( std::vector< std::pair< UInt_t, double > > pmts );

  private:
  ////// Cube Details
  std::vector<Cube4D*> fCubeVec;
  double fMaxLLH;
  int fMaxLLHCube;
  std::vector<std::pair< UInt_t, double > > fPMTs;
  double fT;
  double fTRadius;

};
#endif // __CUBE4DCOLLECTION__
