// SWIG file TaylorExpansionsMoments.i

%{
#include "TaylorExpansionsMoments.hxx"
%}

%include TaylorExpansionsMoments_doc.i

%include TaylorExpansionsMoments.hxx
namespace OT{ %extend TaylorExpansionsMoments { TaylorExpansionsMoments(const TaylorExpansionsMoments & other) { return new OT::TaylorExpansionsMoments(other); } } }

%pythoncode %{
# deprecated
class QuadraticCumul(TaylorExpansionsMoments):
    def __init__(self, *args):
        super(QuadraticCumul, self).__init__(*args)
        openturns.common.Log.Warn('class QuadraticCumul is deprecated in favor of TaylorExpansionsMoments')
%}

