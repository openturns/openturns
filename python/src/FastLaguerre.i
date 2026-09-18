// SWIG file FastLaguerre.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/FastLaguerre.hxx"
#include "openturns/Point.hxx"
%}

%include FastLaguerre_doc.i

%inline %{
  OT::Point FastLaguerre(const OT::UnsignedInteger n,
                         const OT::Scalar k,
                         OT::Point & weightsOut)
  {
    if (n == 0) throw OT::InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (k <= 0.0) throw OT::InvalidArgumentException(HERE) << "Error: k must be > 0";
    OT::Point nodes(n);
    weightsOut = OT::Point(n);
    OT::FastLaguerre::ComputeNodesAndWeights(n, k, &nodes[0], &weightsOut[0]);
    return nodes;
  }
%}