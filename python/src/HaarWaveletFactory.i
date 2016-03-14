// SWIG file HaarWaveletFactory.i

%{
#include "openturns/HaarWaveletFactory.hxx"
%}

%include HaarWaveletFactory_doc.i

%include openturns/HaarWaveletFactory.hxx
namespace OT{ %extend HaarWaveletFactory { HaarWaveletFactory(const HaarWaveletFactory & other) { return new OT::HaarWaveletFactory(other); } } }
