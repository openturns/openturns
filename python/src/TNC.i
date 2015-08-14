// SWIG file TNC.i

%{
#include "TNC.hxx"
%}

%include TNC.hxx
namespace OT{ %extend TNC { TNC(const TNC & other) { return new OT::TNC(other); } } }
