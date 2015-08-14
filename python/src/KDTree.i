// SWIG file KDTree.i

%{
#include "KDTree.hxx"
%}

%include KDTree.hxx
namespace OT { %extend KDTree { KDTree(const KDTree & other) { return new OT::KDTree(other); } } }
