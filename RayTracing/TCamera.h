//
//  TCamera.hpp
//  RayTracing
//
//  Created by Matthew Dutson on 2/23/16.
//  Copyright © 2016 Matthew Dutson. All rights reserved.
//

#ifndef TCamera_h
#define TCamera_h

#include <stdio.h>
#include "TMath.h"
#include "TVector3.h"
#include "TRawData.h"
#include "TSegmentedData.h"
#include "TTelescope.h"

class TCamera {
    
private:
    
    Double_t fHeight;
    
    Int_t fNumberTubesY;
    
    Double_t fWidth;
    
    Int_t fNumberTubesX;
    
    Double_t fPMTResponseTime;
    
    bool fTransparent;
    
    Double_t GetX(Int_t bin);
    
    Double_t GetY(Int_t bin);
    
    Int_t GetBin(Int_t x, Int_t y);
    
    Double_t GetPixelElevationAngle(TTelescope telescope, Int_t pixel);
    
    Double_t GetPixelAzimuthalAngle(TTelescope telescope, Int_t pixel);
    
    TRay GetOutwardDirection(TTelescope telescope, Int_t pixel);
    
public:
    
    TCamera();
    
    TCamera(Double_t fHeight, Int_t numberTubesY, Double_t fWidth, Int_t numberTubesX, Double_t PMTResponseTime, bool transparent);
    
    TSegmentedData ParseData(TRawData data);
    
    void WriteDataToFile(TString filename, TSegmentedData);
    
    bool CheckCollision(TVector3 position);
    
    /*
     * Approximates the incoming direction and impact parameter based on the data collected by the camera. The output array contains, in order, the impact parameter, the angle of the shower-detector plane, and the shower's angle in the shower-detector plane.
     */
    std::vector<Double_t>* ReconstructShower(TSegmentedData data);
    
    /*
     * Estimates the shower plane based on the input data.
     */
    TPlane3 EstimateShowerPlane(TSegmentedData data);
    
};

#endif /* TCamera_h */
