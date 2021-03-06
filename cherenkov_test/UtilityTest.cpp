// MiscellaneousTest.cpp
//
// Author: Matthew Dutson
//
// Tests of Utility.h

#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <TFile.h>
#include <TH1I.h>

#include "MonteCarlo.h"

using namespace std;
using namespace boost::property_tree;

namespace cherenkov_simulator
{
    TEST(MiscellaneousTest, PowerLaw)
    {
        /*
         * Make sure values generated by the random power law match expectations.
         */
        TFile file("PowerLaw.root", "RECREATE");
        TH1I power_histo = TH1I("power_law", "Power Law Value", 10000, 1e2, 1e8);
        for (int i = 0; i < 1e8; i++)
        {
            double value = Utility::RandPower(1e2, 1e8, -1);
            power_histo.Fill(value);
        }
        power_histo.Write("power_histo");
    }
}