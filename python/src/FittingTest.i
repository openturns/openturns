// SWIG file FittingTest.i


// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::TestResult & bestResult ($*ltype temp) %{ temp = OT::TestResult(); $1 = &temp; %}
%typemap(argout) OT::TestResult & bestResult %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::TestResult(*$1), SWIG_TypeQuery("OT::TestResult *"), SWIG_POINTER_OWN |  0 )); %}
OT::Distribution OT::FittingTest::BestModelKolmogorov(const OT::Sample  & sample,
                                                      const OT::DistributionFactoryCollection & factoryCollection,
                                                      OT::TestResult & bestResult);
OT::Distribution OT::FittingTest::BestModelKolmogorov(const OT::Sample  & sample,
                                                      const OT::DistributionCollection & distributionCollection,
                                                      OT::TestResult & bestResult);
OT::Distribution OT::FittingTest::BestModelChiSquared(const OT::Sample  & sample,
                                                      const OT::DistributionFactoryCollection & factoryCollection,
                                                      OT::TestResult & bestResult);
OT::Distribution OT::FittingTest::BestModelChiSquared(const OT::Sample  & sample,
                                                      const OT::DistributionCollection & distributionCollection,
                                                      OT::TestResult & bestResult);

%{
#include "openturns/FittingTest.hxx"
%}

%include FittingTest_doc.i

%include openturns/FittingTest.hxx

