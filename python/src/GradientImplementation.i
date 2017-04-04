// SWIG file GradientImplementation.i

%{
#include "openturns/GradientImplementation.hxx"
%}

%include GradientImplementation_doc.i

%include openturns/GradientImplementation.hxx
namespace OT { %extend GradientImplementation { GradientImplementation(const GradientImplementation & other) { return new OT::GradientImplementation(other); } } }
