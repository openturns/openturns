// SWIG file Gibbs.i

%{
#include "openturns/Gibbs.hxx"
%}

%include Gibbs_doc.i

%include openturns/Gibbs.hxx
namespace OT { %extend Gibbs { Gibbs(const Gibbs & other) { return new OT::Gibbs(other); } } }
