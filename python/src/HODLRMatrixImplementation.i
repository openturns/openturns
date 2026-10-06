// SWIG file HODLRMatrixImplementation.i

%{
#include "openturns/HODLRMatrixImplementation.hxx"
%}

%ignore OT::HODLREntryEvaluator;
%ignore OT::HODLRRealAssemblyFunction;
%ignore OT::HODLRFunctionEvaluator;
%ignore OT::HODLRCovarianceAssemblyFunction;
%ignore OT::HODLRMatrixImplementation::operator=(const HODLRMatrixImplementation &);

%include HODLRMatrixImplementation_doc.i

%copyctor OT::HODLRMatrixImplementation;

%include openturns/HODLRMatrixImplementation.hxx
