// Utility.h
//
// Author: Matthew Dutson
//
// Contains generally useful methods. Has no knowledge of Reconstructor, MonteCarlo, or Simulator.

#ifndef UTILITY_H
#define UTILITY_H

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <TVector3.h>
#include <TRandom3.h>

namespace cherenkov_simulator
{
    class Utility
    {
    public:

        // Physics constants - MeV, cgs
        constexpr static double mass_e = 0.511;
        constexpr static double fine_struct = 0.007297;
        constexpr static double c_cent = 2.998e10;

        /*
         * Reads the string and converts it to a TVector3 object.
         */
        static TVector3 ToVector(std::string s);

        /*
         * Generates a randomly rotated vector perpendicular to the input. If the input vector is zero, (1, 0, 0) is
         * returned.
         */
        static TVector3 RandNormal(TVector3 vec, TRandom3* rng);

        /*
         * Reads the file with the specified filename and parses it to XML. Throws exceptions with an informative
         * message if there is a problem reading or parsing the XML file
         */
        static boost::property_tree::ptree ParseXMLFile(std::string filename);

        /*
         * Determines whether the xy projection of the vector lies within a disk centered at the origin.
         */
        static bool WithinXYDisk(TVector3 vec, double radius);

        /*
         * Constructs the rotation used by MonteCarlo and Simulator classes.
         */
        static TRotation MakeRotation(double elevation_angle);

        /*
         * Returns a random, linearly distributed value constrained between zero and some maximum.
         */
        static double RandLinear(TRandom3* rng, double max);

    private:

        /*
         * A helper method which parses to double everything from the beginning of the string to the first occurence of the
         * secified character. The method then erases everything up to and including the specified character.
         */
        static double ParseTo(std::string* s, char c);
    };
}


#endif