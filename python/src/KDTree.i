// SWIG file KDTree.i

%{
#include "openturns/KDTree.hxx"
%}

%include openturns/KDTree.hxx
namespace OT { %extend KDTree { KDTree(const KDTree & other) { return new OT::KDTree(other); } } }
