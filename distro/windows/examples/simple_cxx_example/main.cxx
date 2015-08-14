//                                               -*- C++ -*-
/**
 *  @file  main.cxx
 *  @brief A simple example
 */
#include "OT.hxx"

int main(int argc, char *argv[])
{
  // create a gaussian distribution from mean and standard deviation
  OT::Normal myDist(5., 1.);

  // print it
  std::cout << "Distribution=" << myDist << std::endl;

  // get a realization
  OT::NumericalPoint x(myDist.getRealization());

  // print it
  std::cout << "Realization=" << x << std::endl;

  return 0;
}
