// SWIG file FFTImplementation.i

%{
#include "openturns/FFTImplementation.hxx"
%}

%include FFTImplementation_doc.i

%include openturns/FFTImplementation.hxx
namespace OT { %extend FFTImplementation { FFTImplementation(const FFTImplementation & other) { return new OT::FFTImplementation(other); } } }
