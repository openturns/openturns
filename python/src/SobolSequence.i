// SWIG file SobolSequence.i

%{
#include "SobolSequence.hxx"
%}

%include SobolSequence_doc.i

%include SobolSequence.hxx
namespace OT{ %extend SobolSequence { SobolSequence(const SobolSequence & other) { return new OT::SobolSequence(other); } } }
