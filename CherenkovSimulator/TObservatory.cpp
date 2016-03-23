//
//  TObservatory.cpp
//  CherenkovSimulator
//
//  Created by Matthew Dutson on 3/14/16.
//  Copyright © 2016 Matthew Dutson. All rights reserved.
//

#include "TObservatory.h"

TObservatory::TObservatory(TMirror mirror, TCamera camera, TCoordinates coordinates, TSurroundings surroundings) {
    fMirror = mirror;
    fCamera = camera;
    fCoordinates = coordinates;
    fSurroundings = surroundings;
}

TRawData TObservatory::ViewPoint(TShower shower) {
    TRawData outputData = TRawData();
    ViewPointPrivate(shower, outputData);
    return outputData;
}

TRawData TObservatory::ViewShower(TShower shower, Double_t timeDelay) {
    TRawData outputData = TRawData();
    Int_t numberOfSteps = (Int_t) (((shower.TimeToPlane(fSurroundings.GroundPlane())) / timeDelay) + 2);
    for(Int_t i = 0; i < numberOfSteps; i++) {
        ViewPointPrivate(shower, outputData);
        shower.IncrementPosition(timeDelay);
    }
    return outputData;
}

void TObservatory::ViewPointPrivate(TShower shower, TRawData &rawData) {
    for (Int_t i = 0; i < shower.GetIntensity(); i++) {
        TVector3 mirrorImpact = fMirror.GetMirrorImpact();
        TVector3 mirrorNormal = fMirror.GetMirrorNormal(mirrorImpact);
        TVector3 transformedPosition = shower.GetPosition();
        fCoordinates.PositionToObservatoryFrame(transformedPosition);
        TPlane3 focalPlane = TPlane3(TVector3(0, 0, 1), TVector3(0, 0, -fMirror.Radius() + fCamera.FocalLength()));
        TRay detectedRay = TRay(shower.GetTime(), transformedPosition, mirrorImpact - transformedPosition);
        detectedRay.PropagateToPlane(focalPlane);
        if (fCamera.CheckCollision(detectedRay.GetPosition())) {
            continue;
        }
        detectedRay.PropagateToPoint(mirrorImpact);
        detectedRay.ReflectFromPlane(TPlane3(mirrorNormal, mirrorImpact));
        detectedRay.PropagateToPlane(focalPlane);
        rawData.PushBack(detectedRay.GetPosition().X(), detectedRay.GetPosition().Y(), detectedRay.GetTime());
    }
}

TPlane3 TObservatory::ApproximateShowerPlane(TSegmentedData data) {
    TVector3 bestNormal = TVector3(0, 0, 1);
    Double_t angleStep = 1;
    Double_t bestSquare = 1e300;
    for (Double_t theta = 0; theta <= TMath::Pi(); theta += angleStep) {
        for (Double_t phi = -TMath::Pi(); phi <= TMath::Pi(); phi += angleStep) {
            TVector3 normal = TVector3(0, 0, 1);
            normal.RotateX(theta);
            normal.RotateZ(phi);
            Double_t squareSum = 0;
            for (Int_t i = 0; i < data.GetNBins(); i++) {
                squareSum += normal.Dot(fCamera.GetViewDirection(i)) * data.GetSegment(i)->size();
            }
            if (squareSum < bestSquare) {
                bestSquare = squareSum;
                bestNormal = normal;
            }
        }
    }
    return TPlane3(bestNormal, TVector3(0, 0, 0));
}

TRay TObservatory::ReconstructShower(TSegmentedData data) {
    Int_t nBins = data.GetNBins();
    TPlane3 showerPlane = ApproximateShowerPlane(data);
    TVector3 groundIntersection =  showerPlane.IntersectWithXYPlane();
    Double_t averages[nBins], angles[nBins];
    for (Int_t i = 0; i < nBins; i++) {
        averages[i] = TUtility::SumArray(*data.GetSegment(i)) / data.GetSegment(i)->size();
        angles[i] = fCamera.GetViewDirection(i).Angle(groundIntersection);
    }
    Double_t bestImpactParam = 0;
    Double_t impactRange[] = {0, 100000};
    Double_t impactStepSize = 100;
    Double_t bestShowerAngle = 0;
    Double_t angleStep = 1;
    Double_t bestSquare = 1e300;
    for (Double_t impactParam = impactRange[0]; impactParam < impactRange[1]; impactParam += impactStepSize) {
        for (Double_t showerAngle = 0; showerAngle < TMath::Pi(); showerAngle += angleStep) {
            Double_t squareSum = 0;
            Double_t impactAngle = TMath::Pi() / 2 - showerAngle;
            Int_t nearestIndex = 0;
            for (Int_t i = 0; i < nBins; i++) {
                if (TMath::Abs(angles[i] - impactAngle) < TMath::Abs(angles[nearestIndex] - impactAngle)) {
                    nearestIndex = i;
                }
            }
            double_t t0 = averages[nearestIndex];
            for (Int_t i = 0; i < nBins; i++) {
                squareSum += (t0 + impactParam / TRay::fLightSpeed * TMath::Tan((TMath::Pi() - showerAngle - angles[i]) / 2) - averages[i]) * (t0 + impactParam / TRay::fLightSpeed * TMath::Tan((TMath::Pi() - showerAngle - angles[i]) / 2) - averages[i]);
            }
            if (squareSum < bestSquare) {
                bestImpactParam = impactParam;
                bestShowerAngle = showerAngle;
                bestSquare = squareSum;
            }
        }
    }
    std::vector<Double_t>* output = new std::vector<Double_t>();
    output->push_back(bestImpactParam);
    output->push_back(bestShowerAngle);
    return TRay(0, TVector3(0, 0, 0), TVector3(0, 0, 0));
}

TSegmentedData TObservatory::ParseData(TRawData data) {
    return fCamera.ParseData(data);
}