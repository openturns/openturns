// SWIG file CovarianceModelImplementation.i

%{
#include "openturns/CovarianceModelImplementation.hxx"
%}

%include CovarianceModelImplementation_doc.i

%ignore OT::CovarianceModelImplementation::isParallel;

%copyctor OT::CovarianceModelImplementation;

%include openturns/CovarianceModelImplementation.hxx
