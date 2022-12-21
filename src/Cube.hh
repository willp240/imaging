////////////////////////////////////////////////////////////////////////
/// \class Cube
////////////////////////////////////////////////////////////////////////

#ifndef __CUBE__
#define __CUBE__

#include <TVector3.h>
#include <iostream>

#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>

#include <CubeCollection.hh>

class CubeCollection;

class Cube{

  public:
  Cube( double x_, double y_, double z_, double radius_ )
    : fX(x_), fY(y_), fZ(z_), fRadius(radius_) {}

  Cube(){};
  
  void SetX( double x_ ) { fX = x_; }
  double GetX() { return fX; }
  void SetY( double y_ ) { fY = y_; }
  double GetY() { return fY; }
  void SetZ( double z_ ) { fZ = z_; }
  double GetZ() { return fZ; }
  void SetT( double t_ ) { fT = t_; }
  double GetT() { return fT; }
  void SetRadius( double radius_ ) { fRadius = radius_; }
  double GetRadius() { return fRadius; }
  void SetLLH( double llh_ ) { fLLH = llh_; }
  double GetLLH() { return fLLH; }
  void SetPMTs( std::vector< RAT::DS::PMTCal > pmts_ ) { fPMTs = pmts_; }
  std::vector< RAT::DS::PMTCal > GetPMTs( ) { return fPMTs; }

  CubeCollection* Divide( int factor );

  private:
  ////// Cube Details
  double fX;
  double fY;
  double fZ;
  double fT;
  double fRadius;
  double fLLH = 0;
  std::vector< RAT::DS::PMTCal > fPMTs;

};

#endif // __CUBE__
