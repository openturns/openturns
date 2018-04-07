// SWIG file EmpiricalBernsteinCopula.i

%{
#include "openturns/EmpiricalBernsteinCopula.hxx"
%}

%include EmpiricalBernsteinCopula_doc.i

%include openturns/EmpiricalBernsteinCopula.hxx
namespace OT { %extend EmpiricalBernsteinCopula { EmpiricalBernsteinCopula(const EmpiricalBernsteinCopula & other) { return new OT::EmpiricalBernsteinCopula(other); } } }
