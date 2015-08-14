// SWIG file ImportanceSampling.i

%{
#include "ImportanceSampling.hxx"
%}

%include ImportanceSampling_doc.i

%include ImportanceSampling.hxx
namespace OT{ %extend ImportanceSampling { ImportanceSampling(const ImportanceSampling & other) { return new OT::ImportanceSampling(other); } } }
