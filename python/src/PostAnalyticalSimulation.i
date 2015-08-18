// SWIG file PostAnalyticalSimulation.i

%{
#include "PostAnalyticalSimulation.hxx"
%}

%include PostAnalyticalSimulation_doc.i

%include PostAnalyticalSimulation.hxx

namespace OT{ %extend PostAnalyticalSimulation { PostAnalyticalSimulation(const PostAnalyticalSimulation & other) { return new OT::PostAnalyticalSimulation(other); } } }
