// SWIG file TNC.i

%{
#include "TNC.hxx"
%}

%include TNC_doc.i

%include TNC.hxx
namespace OT{ %extend TNC { TNC(const TNC & other) { return new OT::TNC(other); } } }
