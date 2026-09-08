// SWIG file FastHermite.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/FastHermite.hxx"
#include "openturns/Point.hxx"
%}

%include FastHermite_doc.i

%inline %{
  OT::Point FastHermite(const OT::UnsignedInteger n,
                        OT::Point & weightsOut)
  {
    if (n == 0) throw OT::InvalidArgumentException(HERE) << "Error: n must be > 0";
    OT::Point nodes(n);
    weightsOut = OT::Point(n);
    OT::FastHermite::ComputeNodesAndWeights(n, &nodes[0], &weightsOut[0]);
    return nodes;
  }
%}