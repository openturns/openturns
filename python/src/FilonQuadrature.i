// SWIG file FilonQuadrature.i

%{
#include "openturns/FilonQuadrature.hxx"
%}

%include FilonQuadrature_doc.i

%include openturns/FilonQuadrature.hxx
namespace OT { %extend FilonQuadrature { FilonQuadrature(const FilonQuadrature & other) { return new OT::FilonQuadrature(other); } } }
