// SWIG file SORM.i

%{
#include "openturns/SORM.hxx"
%}

%include SORM_doc.i

%include openturns/SORM.hxx
namespace OT{ %extend SORM { SORM(const SORM & other) { return new OT::SORM(other); } } }
