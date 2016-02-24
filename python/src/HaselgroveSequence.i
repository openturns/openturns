// SWIG file HaselgroveSequence.i

%{
#include "openturns/HaselgroveSequence.hxx"
%}

%include HaselgroveSequence_doc.i

%include openturns/HaselgroveSequence.hxx
namespace OT{ %extend HaselgroveSequence { HaselgroveSequence(const HaselgroveSequence & other) { return new OT::HaselgroveSequence(other); } } }
