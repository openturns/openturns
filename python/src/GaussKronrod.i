// SWIG file GaussKronrod.i

%{
#include "GaussKronrod.hxx"
%}

%include GaussKronrod_doc.i

%include GaussKronrod.hxx
namespace OT { %extend GaussKronrod { GaussKronrod(const GaussKronrod & other) { return new OT::GaussKronrod(other); } } }
