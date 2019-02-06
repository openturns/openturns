// SWIG file DiscreteMarkovChain.i

%{
#include "openturns/DiscreteMarkovChain.hxx"
%}

%include DiscreteMarkovChain_doc.i

%include openturns/DiscreteMarkovChain.hxx
namespace OT { %extend DiscreteMarkovChain { DiscreteMarkovChain(const DiscreteMarkovChain & other) { return new OT::DiscreteMarkovChain(other); } } }
