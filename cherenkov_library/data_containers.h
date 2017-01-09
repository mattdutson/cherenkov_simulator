// data_containers.h
// cherenkov_simulator
//
// Created by Matthew Dutson on 9/8/16.
//
//

#ifndef data_containers_h
#define data_containers_h

#include "TVector3.h"
#include "TRandom3.h"

namespace cherenkov_simulator
{

    class SignalIterator
    {

    public:

        /*
         * The only constructor. Takes a 2D vector of bools which can be used to determine which pixels are valid. The
         * user is responsible for ensuring that all sub-vectors are non-null.
         */
        SignalIterator(std::vector<std::vector<bool>> validPixels);

        /*
         * Returns the current x index of the iterator.
         */
        int X();

        /*
         * Returns the current y index of the iterator.
         */
        int Y();

        /*
         * Moves to the next photomultiplier signal. Returns false if the iterator has reached the end of the
         * collection (the last photomultiplier).
         */
        bool Next();

        /*
         * Returns the iterator to its starting state.
         */
        void Reset();

    private:

        // A 2D vector of bools which can be used to determine which pixels are valid.
        std::vector<std::vector<bool>> valid_pixels;

        // The current x index of the iterator
        int x_current;

        // The current y index of the iterator
        int y_current;
    };

    class PhotonCount
    {

    public:

        /*
         * Creates a 2d indexed collection of vectors (a vector of vectors of vectors). Each vector represents the time
         * series for a particular photomultiplier. Takes the width/height of the array in number of photomultipliers,
         * the starting time for the series, the width of a time bin, and the angular width of a photomultiplier.
         */
        PhotonCount(int n_pmt_across, double start_time, double time_bin, double pmt_angular_size,
                    double pmt_linear_size);

        /*
         * Increments the photon count at some time for the photomultiplier pointing in the specified direction.
         */
        void AddPhoton(double time, TVector3 direction);

        /*
         * Adds some number of noise photons to the channel referenced by the iterator. The noise rate represents the
         * number of photons per second per steradian per square centimeter. These photons are randomly scattered
         * throughout the time bins using the random number generator.
         */
        void AddNoise(double noise_rate, SignalIterator current, TRandom3 rng);

        /*
         * Determines the direction of the photomultiplier referenced by the iterator.
         */
        TVector3 Direction(SignalIterator current);

        /*
         * Returns an object for iterating through the pixels.
         */
        SignalIterator Iterator();

        /*
         * Sums all bins in the channel referenced by the iterator.
         */
        int SumBins(SignalIterator iter);

    private:

        /*
         * Determines whether the pixel at the specified indices lies within the central circle.
         */
        bool ValidPixel(int x_index, int y_index);

        // The underlying data structure to store photon counts
        std::vector<std::vector<std::vector<int>>> photon_counts;

        // A 2D vector of bools which can be used to determine which pixels are valid.
        std::vector<std::vector<bool>> valid_pixels;

        // The time at the beginning of the zeroth bin
        double start_time;

        // The size of time bins
        double time_bin;

        // The angle viewed by each photomultiplier
        double pmt_angular_size;

        // The side length of each photomultiplier
        double pmt_linear_size;
    };
}

#endif
