// SWIG file GaussKronrod.i

%{
#include "openturns/GaussKronrod.hxx"
%}

%include GaussKronrod_doc.i

%include openturns/GaussKronrod.hxx
namespace OT { %extend GaussKronrod { GaussKronrod(const GaussKronrod & other) { return new OT::GaussKronrod(other); } } }
