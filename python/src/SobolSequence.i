// SWIG file SobolSequence.i

%{
#include "openturns/SobolSequence.hxx"
%}

%include SobolSequence_doc.i

%include openturns/SobolSequence.hxx
namespace OT{ %extend SobolSequence { SobolSequence(const SobolSequence & other) { return new OT::SobolSequence(other); } } }
