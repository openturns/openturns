// SWIG file Geometric.i

%{
#include "Geometric.hxx"
%}

%include Geometric_doc.i

%include Geometric.hxx
namespace OT { %extend Geometric { Geometric(const Geometric & other) { return new OT::Geometric(other); } } }
