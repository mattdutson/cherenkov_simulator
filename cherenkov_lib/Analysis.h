// Analysis.h
//
// Author: Matthew Dutson
//
// Contains methods which may be useful for analyzing data produced by methods in the Simulator, MonteCarlo, and
// Reconstructor classes. Declared separately from Utility.h to prevent circular dependencies.

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <vector>
#include <TH2.h>
#include <TGraph.h>

#include "DataStructures.h"
#include "Geometric.h"

namespace cherenkov_simulator
{
    class Analysis
    {
    public:

        /*
         * Makes a TGraph of the collapsed time profile for the simulation.
         */
        static TGraph MakeProfileGraph(PhotonCount data);

        /*
         * Makes a 2D histogram with the sum of each bin
         */
        static TH2I MakeSumMap(PhotonCount data);

        /*
         * Creates a 2D histogram with a 1 for valid cells and a 0 for invalid cells.
         */
        static TH2C GetValidMap(PhotonCount data);

        /*
         * Creates a 2D histogram with a 1 for true values and a 0 for false values.
         */
        static TH2C GetBooleanMap(std::vector<std::vector<bool>> valid);

    private:

        /*
         * Finds the superimposed time profile of all photomultiplier signals. It is assumed that all signals will have the
         * same time length (if AddNoise wasn't called this won't be the case).
         */
        static void SuperimposeTimes(PhotonCount data, std::vector<double>* times, std::vector<double>* counts);
    };
}

#endif