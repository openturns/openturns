// SWIG file PostAnalyticalSimulation.i

%{
#include "openturns/PostAnalyticalSimulation.hxx"
%}

%include PostAnalyticalSimulation_doc.i

%include openturns/PostAnalyticalSimulation.hxx

namespace OT{ %extend PostAnalyticalSimulation { PostAnalyticalSimulation(const PostAnalyticalSimulation & other) { return new OT::PostAnalyticalSimulation(other); } } }
