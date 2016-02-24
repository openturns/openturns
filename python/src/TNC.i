// SWIG file TNC.i

%{
#include "openturns/TNC.hxx"
%}

%include TNC_doc.i

%include openturns/TNC.hxx
namespace OT{ %extend TNC { TNC(const TNC & other) { return new OT::TNC(other); } } }
