/*
 * Created by Matthew Dutson on 1/29/16.
 * Copyright © 2016 Matthew Dutson. All rights reserved.
 *
 * This file contains the implementation of "TTelescope.h". See the header file for method descriptions. A left-handed coordinate system is used, with the z-axis pointing along the axis of the telescope and the x-axis oriented parallel to the horizontal.
 */

#include "TTelescope.h"

void TTelescope::ViewPointPrivate(TShower shower, TRawData& data) {
    
    // Steps the shower along its path and runs the ray detection algorithm at each point
    for(Int_t i = 0; i < shower.GetIntensity(); i++) {
        TRay* planeDetection = RayDetection(shower);
        
        // If the detected ray hit the camera, skip this iteration
        if(planeDetection == nullptr) {
            delete planeDetection;
            continue;
        }
        
        TVector3 position =  planeDetection->GetPosition();
        
        // Change coordinates to the telescope frame and store data in the array
        RotateOut(position);
        TranslateOut(position);
        data.PushBack(position.X(), position.Y(), shower.GetTime());
        delete planeDetection;
    }
}

TRay* TTelescope::RayDetection(TShower shower) {

    // Find where the detected ray hits the mirror and the normal vector at that point
    TVector3* mirrorImpact = GetMirrorImpact();
    TVector3* mirrorNormal = GetMirrorNormal(*mirrorImpact);
    
    // Create the detected ray
    TRay* detectedRay = new TRay(shower.GetTime(), shower.GetPosition(), (*mirrorImpact) - shower.GetPosition());
    
    // Propagate the detected ray to the focal plane and check whether it collided with the mirror
    detectedRay->PropagateToPlane(fFocalPlane);
    TVector3 position = detectedRay->GetPosition();
    RotateOut(position);
    TranslateOut(position);
    
    if(fCamera.CheckCollision(position)) {
        delete mirrorImpact;
        delete mirrorNormal;
        delete detectedRay;
        return nullptr;
    }
    
    // If the detected ray didn't hit the camera, continue with the simulation
    detectedRay->PropagateToPoint(*mirrorImpact);
    
    // Reflects the ray from the mirror and propagates it to the pixel plane
    detectedRay->ReflectFromPlane(TPlane3(*mirrorNormal, *mirrorImpact));
    detectedRay->PropagateToPlane(fFocalPlane);
    delete mirrorImpact;
    delete mirrorNormal;
    return detectedRay;
}

TVector3* TTelescope::GetMirrorImpact() {
    Double_t xRandom = 0;
    Double_t yRandom = 0;
    
    // Select a random point in a circle
    if (fMirrorShape == 0) {
        bool iterate = true;
        while (iterate) {
            xRandom = (fRandom->Rndm() - 0.5) * fCrossDiameter;
            yRandom = (fRandom->Rndm() - 0.5) * fCrossDiameter;
            if (xRandom * xRandom + yRandom * yRandom <= fCrossDiameter * fCrossDiameter / 4) {
                iterate = false;
            }
        }
    }
    
    // Select a random point in a square
    else if (fMirrorShape == 1) {
        xRandom = (fRandom->Rndm() - 0.5) * fCrossDiameter;
        yRandom = (fRandom->Rndm() - 0.5) * fCrossDiameter;
    }
    
    // Find the z-component corresponding to xRandom and yRandom based on the equation of the mirror
    TVector3 relativePosition;
    if (fMirrorType == 0) {
        relativePosition = TVector3(xRandom, yRandom, -fRadius + (fRadius - TMath::Sqrt(fRadius * fRadius - xRandom * xRandom - yRandom * yRandom)));
    }
    else if (fMirrorType == 1) {
        relativePosition = TVector3(xRandom, yRandom, -fRadius + ((xRandom * xRandom / (2 * fRadius) + yRandom * yRandom / (2 * fRadius))));
    }
    
    // Rotate and translate the relative position into the correct frame
    RotateIn(relativePosition);
    return new TVector3(fCenterOfCurvature + relativePosition);
}

TVector3* TTelescope::GetMirrorNormal(TVector3 mirrorImpact) {
    TVector3 mirrorNormal;
    
    // For circular mirrors, the normal vector points directly to the center of curvature
    if (fMirrorType == 0) {
        mirrorNormal = fCenterOfCurvature - mirrorImpact;
    }

    // For parabolic mirrors, the normal vector is found from the equation z = x^2 / (2R) + y^2 / (2R)
    else if (fMirrorType == 1) {
        mirrorImpact = mirrorImpact - fCenterOfCurvature;
        RotateOut(mirrorImpact);
        mirrorNormal = TVector3(-mirrorImpact.X(), -mirrorImpact.Y(), fRadius);
        RotateIn(mirrorNormal);
    }
    return new TVector3(mirrorNormal);
}

TCamera* TTelescope::GetCamera() {
    return &fCamera;
}

void TTelescope::RotateIn(TVector3& vector) {
    vector.RotateY(fInclination);
    vector.RotateX(fAzimuth);
}

void TTelescope::RotateOut(TVector3& vector) {
    vector.RotateY(-fInclination);
    vector.RotateX(-fAzimuth);
}

void TTelescope::TranslateIn(TVector3& vector) {
    vector += fCenterOfCurvature;
}

void TTelescope::TranslateOut(TVector3& vector) {
    vector -= fCenterOfCurvature;
}

TTelescope::TTelescope(Short_t mirrorShape, Short_t mirrorType, Double_t radius, Double_t focalLength, Double_t fNumber, TCamera camera): TTelescope(mirrorShape, mirrorType, radius, focalLength, fNumber, 0, 0, TVector3(0, 0, 0), TPlane3(TVector3(1, 0, 0), TVector3(0, 0, 0)), camera) {}

TTelescope::TTelescope(Short_t mirrorShape, Short_t mirrorType, Double_t radius, Double_t focalLength, Double_t fNumber, Double_t inclination, Double_t azimuth, TVector3 centerOfCurvature, TPlane3 groundPlane, TCamera camera) {
    // Set the mirror shape, checking for invalid input
    if (mirrorShape < 0 || mirrorShape > 1) {
        throw new std::invalid_argument("The mirror shape must lie in the range [0, 1]");
    }
    else {
        fMirrorShape = mirrorShape;
    }
    
    // Set the mirror type, checking for invalid input
    if (mirrorType < 0 || mirrorType > 1) {
        throw new std::invalid_argument("The mirror type must lie in the range [0, 1]");
    }
    else {
        fMirrorType = mirrorType;
    }
    
    // Initialize member variables
    fCamera = camera;
    fRadius = radius;
    fFocalLength = focalLength;
    fCrossDiameter = focalLength / fNumber;
    fCenterOfCurvature = centerOfCurvature;
    fGroundPlane = groundPlane;
    
    // Initialize angles
    fInclination = inclination;
    fAzimuth = azimuth; 
    
    // Initialize the axis of the mirror
    fMirrorAxis = TVector3(0, 0, 1);
    RotateIn(fMirrorAxis);
    
    // Initialize the focal plane
    TVector3 relativePosition(0, 0, -fRadius + focalLength);
    RotateIn(relativePosition);
    TVector3 focalPlaneCenter = fCenterOfCurvature + relativePosition;
    fFocalPlane = TPlane3(fMirrorAxis, focalPlaneCenter);
}

void TTelescope::ViewShower(TShower shower, Double_t timeDelay, TRawData& data) {
    
    // Clear the array before starting.
    data.Clear();
    
    // Creates arrays to store the output data
    Int_t numberOfSteps = (Int_t) (((shower.TimeToPlane(fGroundPlane)) / timeDelay) + 2);
    
    // Steps the shower along its path and runs the ray detection algorithm at each point
    for(Int_t i = 0; i < numberOfSteps; i++) {
        ViewPointPrivate(shower, data);
        shower.IncrementPosition(timeDelay);
    }
}

void TTelescope::ViewPoint(TShower shower, TRawData& data) {
    data.Clear();
    ViewPointPrivate(shower, data);
}

Double_t TTelescope::GetFocalLength() {
    return fFocalLength;
}

Double_t TTelescope::GetRadius() {
    return fRadius;
}

TVector3 TTelescope::GetAxis() {
    return fMirrorAxis;
}

TVector3 TTelescope::GetCenterOfCurvature() {
    return fCenterOfCurvature;
}