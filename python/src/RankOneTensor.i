// SWIG file RankOneTensor.i

%{
#include "openturns/RankOneTensor.hxx"
%}

//%include RankOneTensor_doc.i

%include openturns/RankOneTensor.hxx
namespace OT{ %extend RankOneTensor { RankOneTensor(const RankOneTensor & other) { return new OT::RankOneTensor(other); } } }
