// SWIG file HSICVStat.i

%{
#include "openturns/HSICVStat.hxx"
%}

%include HSICVStat_doc.i

%include openturns/HSICVStat.hxx
namespace OT{ %extend HSICVStat { HSICVStat(const HSICVStat & other) { return new OT::HSICVStat(other); } } }
