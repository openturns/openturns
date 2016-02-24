// SWIG file Geometric.i

%{
#include "openturns/Geometric.hxx"
%}

%include Geometric_doc.i

%include openturns/Geometric.hxx
namespace OT { %extend Geometric { Geometric(const Geometric & other) { return new OT::Geometric(other); } } }
