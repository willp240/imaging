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

  Cube4DCollection(const Cube4DCollection &collection ){

    fCubeVec = collection.GetCubeVec();
    fPMTs = collection.GetPMTs();
    fT = collection.GetT();
    fTRadius = collection.GetTRadius();
    
  }


  void AddCube( Cube4D* cube_ ) { fCubeVec.push_back( cube_ ); }
  std::vector<Cube4D*> GetCubeVec() const { return fCubeVec; }
  Cube4D* GetCube( int i_ ) const { return fCubeVec.at( i_ ); }
  int GetNCubes() { return fCubeVec.size(); }
  void SetPMTs( std::vector< std::pair< UInt_t, double > > pmts_ );
  std::vector< std::pair< UInt_t, double > > GetPMTs() const { return fPMTs; }
  void SetT( double t_ );
  double GetT() const { return fT; }
  void SetTRadius( double trad_ );
  double GetTRadius() const { return fTRadius; }
  void SortByOverlap();
  void RemoveRepeatedPMTs();
  void RemovePMTs( std::vector< std::pair< UInt_t, double > > pmts );

  private:
  ////// Cube Details
  std::vector<Cube4D*> fCubeVec;
  std::vector<std::pair< UInt_t, double > > fPMTs;
  double fT;
  double fTRadius;

};
#endif // __CUBE4DCOLLECTION__
