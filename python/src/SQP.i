// SWIG file SQP.i

%{
#include "openturns/SQP.hxx"
%}

%include SQP_doc.i

%include openturns/SQP.hxx
namespace OT{ %extend SQP { SQP(const SQP & other) { return new OT::SQP(other); } } }
