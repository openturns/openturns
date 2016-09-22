// SWIG file TaylorExpansionMoments.i

%{
#include "openturns/TaylorExpansionMoments.hxx"
%}

%include TaylorExpansionMoments_doc.i

%include openturns/TaylorExpansionMoments.hxx
namespace OT{ %extend TaylorExpansionMoments { TaylorExpansionMoments(const TaylorExpansionMoments & other) { return new OT::TaylorExpansionMoments(other); } } }

%pythoncode %{
# deprecated
class QuadraticCumul(TaylorExpansionMoments):
    def __init__(self, *args):
        super(QuadraticCumul, self).__init__(*args)
        openturns.common.Log.Warn('class QuadraticCumul is deprecated in favor of TaylorExpansionMoments')
%}

