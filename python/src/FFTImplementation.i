// SWIG file FFTImplementation.i

%{
#include "FFTImplementation.hxx"
%}

%include FFTImplementation_doc.i

%include FFTImplementation.hxx
namespace OT { %extend FFTImplementation { FFTImplementation(const FFTImplementation & other) { return new OT::FFTImplementation(other); } } }
