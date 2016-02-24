// SWIG file ImportanceSampling.i

%{
#include "openturns/ImportanceSampling.hxx"
%}

%include ImportanceSampling_doc.i

%include openturns/ImportanceSampling.hxx
namespace OT{ %extend ImportanceSampling { ImportanceSampling(const ImportanceSampling & other) { return new OT::ImportanceSampling(other); } } }
