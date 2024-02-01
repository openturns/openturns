// SWIG file HMatrixImplementation.i

%{
#include "openturns/HMatrixImplementation.hxx"
%}

%ignore OT::HMatrixImplementation::operator=(const HMatrixImplementation &);

%ignore OT::HMatrixTensorRealAssemblyFunction;
%ignore OT::HMatrixRealAssemblyFunction;
%ignore OT::HMatrixClusterTree;
%ignore OT::CovarianceAssemblyFunction;
%ignore OT::CovarianceBlockAssemblyFunction;

%include HMatrixImplementation_doc.i

%copyctor OT::HMatrixImplementation;

%include openturns/HMatrixImplementation.hxx
