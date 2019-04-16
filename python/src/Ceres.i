// SWIG file Ceres.i

%{
#include "openturns/Ceres.hxx"
%}

%include Ceres_doc.i

%include openturns/Ceres.hxx

namespace OT { %extend Ceres { Ceres(const Ceres & other) { return new OT::Ceres(other); } } }

