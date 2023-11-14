//                                               -*- C++ -*-
/**
 *  @brief This class allows one to compute integrals of a function over a mesh.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/SimplicialCubature.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimplicialCubature
 */

CLASSNAMEINIT(SimplicialCubature)

static const Factory<SimplicialCubature> Factory_SimplicialCubature;

/* Default constructor */
SimplicialCubature::SimplicialCubature()
  : PersistentObject()
  , rule_(ResourceMap::GetAsUnsignedInteger("SimplicialCubature-DefaultRule"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("SimplicialCubature-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("SimplicialCubature-DefaultMaximumRelativeError"))
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("SimplicialCubature-DefaultMaximumEvaluationNumber"))
{
  // Nothing to do
}

/* Virtual constructor */
SimplicialCubature * SimplicialCubature::clone() const
{
  return new SimplicialCubature(*this);
}

void SimplicialCubature::setRule(const UnsignedInteger rule)
{
  if ((rule < 1) || (rule > 4))
    throw InvalidArgumentException(HERE) << "rule must be in [1, 4]";
  rule_ = rule;
}

UnsignedInteger SimplicialCubature::getRule() const
{
  return rule_;
}

/* Maximum absolute error accessor */
void SimplicialCubature::setMaximumAbsoluteError(const Scalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}

Scalar SimplicialCubature::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

/* Maximum relative error accessor */
void SimplicialCubature::setMaximumRelativeError(const Scalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}

Scalar SimplicialCubature::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

/* Maximum evaluations number accessor */
void SimplicialCubature::setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber)
{
  maximumEvaluationNumber_ = maximumEvaluationNumber;
}

UnsignedInteger SimplicialCubature::getMaximumEvaluationNumber() const
{
  return maximumEvaluationNumber_;
}


/* String converter */
String SimplicialCubature::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SimplicialCubature::GetClassName()
      << " rule=" << rule_;
  return oss;
}

/* String converter */
String SimplicialCubature::__str__(const String & ) const
{
  OSS oss(false);
  oss << SimplicialCubature::GetClassName()
      << "(rule = " << rule_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SimplicialCubature::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("rule_", rule_);
}

/* Method load() reloads the object from the StorageManager */
void SimplicialCubature::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("rule_", rule_);
}

UnsignedInteger SimplicialCubature::getNodeNumber(const UnsignedInteger dimension) const
{
  UnsignedInteger nodeNumber = 0;
  switch (rule_)
  {
    case 1:
      nodeNumber = 2 * dimension + 3;
      break;
    case 2:
      nodeNumber = (dimension + 3) * (dimension + 2) / 2 + 2 * (dimension + 1);
      break;
    case 3:
      nodeNumber = (dimension + 4) * (dimension + 3) * (dimension + 2) / 6 + (dimension + 2) * (dimension + 1);
      break;
    case 4:
      nodeNumber = (dimension + 5) * (dimension + 4) * (dimension + 3) * (dimension + 2) / 24 + 5 * (dimension + 2) * (dimension + 1) / 2;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid integration rule";
  }
  return nodeNumber;
}


/* Compute an approximation of \int_a^b f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an n-D interval.
 */
Point SimplicialCubature::integrate(const Function & F, const Mesh & mesh) const
{
  const UnsignedInteger outputDimension = F.getOutputDimension();
  const UnsignedInteger MXFS = maximumEvaluationNumber_;
  const Scalar EA = maximumAbsoluteError_;
  const Scalar ER = maximumRelativeError_;
  const UnsignedInteger dimension = mesh.getDimension();
  UnsignedInteger flatSize = mesh.getSimplicesNumber();
  const UnsignedInteger nodeNumber = getNodeNumber(dimension);

  UnsignedInteger NV = 0;
  const UnsignedInteger DFCOST = 1 + 2 * dimension * (dimension + 1);
  Point VL(outputDimension);
  Point AE(outputDimension);

  Matrix W, G;
  Indices evalBudget(initializeBasicRule(dimension, W, G));
  Sample VLS(flatSize, outputDimension);
  Sample AES(flatSize, outputDimension);
  Point volume(mesh.computeSimplicesVolume());

  Point error;
  Point value;

  // flat mesh, will be modified in-place
  Collection<Sample> flatVertices(flatSize, Sample(0, dimension));
  for (UnsignedInteger K = 0; K < flatSize; ++ K)
  {
    Indices simplex(mesh.getSimplex(K));
    for (UnsignedInteger i = 0; i < simplex.getSize(); ++ i)
      flatVertices[K].add(mesh.getVertex(simplex[i]));

    // Apply basic rule over each simplex.
    computeRuleValueAndError(dimension, flatVertices[K], volume[K], outputDimension, F, G, W, evalBudget, value, error);

    AES[K] = error;
    VLS[K] = value;

    VL += value;
    AE += error;
    NV += nodeNumber;
  }

  Bool FL = false;
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    if (AE[i] > std::max(EA, ER * abs(VL[i])))
    {
      FL = true;
      break;
    }

  while (FL && (NV + DFCOST + 4 * nodeNumber <= MXFS))
  {
    // select simplex with biggest abs. error
    UnsignedInteger ID = flatSize;
    Scalar maxAES = -SpecFunc::MaxScalar;
    for (UnsignedInteger i = 0; i < flatSize; ++ i)
      for (UnsignedInteger j = 0; j < outputDimension; ++ j)
        if (AES(i, j) > maxAES)
        {
          ID = i;
          maxAES = AES(i, j);
        }

    VL -= VLS[ID];
    AE -= AES[ID];
    const UnsignedInteger NEW = computeNewSubregions(dimension, outputDimension, F, ID, flatSize, flatVertices);
    Scalar VI = volume[ID] / NEW;

    //     Apply basic rule, and add new contributions to VL and AE.
    volume.resize(flatSize + NEW - 1);
    VLS.add(Sample(NEW - 1, outputDimension));
    AES.add(Sample(NEW - 1, outputDimension));

    // same as for loop below but K=ID
    volume[ID] = VI;
    computeRuleValueAndError(dimension, flatVertices[ID], VI, outputDimension, F, G, W, evalBudget, value, error);
    VLS[ID] = value;
    AES[ID] = error;
    VL += VLS[ID];
    AE += AES[ID];
    NV += nodeNumber;

    for (UnsignedInteger K = flatSize; K < flatSize + NEW - 1; ++K)
    {
      volume[K] = VI;
      computeRuleValueAndError(dimension, flatVertices[K], VI, outputDimension, F, G, W, evalBudget, value, error);
      VLS[K] = value;
      AES[K] = error;
      VL += VLS[K];
      AE += AES[K];
      NV += nodeNumber;
    }

    NV += DFCOST;
    flatSize += NEW - 1;

    // Check for error termination.
    FL = false;
    for (UnsignedInteger i = 0; i < outputDimension; ++ i)
      if (AE[i] > std::max(EA, ER * abs(VL[i])))
      {
        FL = true;
        break;
      }
  }
  return VL;
}

Point SimplicialCubature::computePermutationSums(const UnsignedInteger dimension, const Sample & simplexVertices,
    const UnsignedInteger outputDimension, const Function & function,
    const Point & GConst) const
{
  Point result(outputDimension, 0.0);
  Point G(GConst);
  std::sort(G.begin(), G.end(), std::greater<Scalar>());

  Bool pr = true;
  // Compute integrand value for permutations of G
  while (pr)
  {
    // TODO: better matrix conversion ?
    Matrix VERTEXM(simplexVertices.getDimension(), simplexVertices.getSize());
    for (UnsignedInteger i = 0; i < simplexVertices.getSize(); ++ i)
      for (UnsignedInteger j = 0; j < simplexVertices.getDimension(); ++ j)
        VERTEXM(j, i) = simplexVertices(i, j);

    result += function(VERTEXM * G);
    pr = false;

    for (UnsignedInteger I = 1; I < dimension + 1; ++ I)
    {
      const Scalar GI = G[I];
      if (G[I - 1] > GI)
      {
        UnsignedInteger IX = I - 1;
        UnsignedInteger LX = 0;
        for (UnsignedInteger L = 0; L < ((IX + 2) / 2); ++ L)
        {
          const Scalar GL = G[L];
          if (GL <= GI)
            --IX;
          G[L] = G[I - L - 1];
          G[I - L - 1] = GL;
          if (G[L] > GI)
            LX = L;
        }
        if (G[IX] <= GI)
          IX = LX;
        G[I] = G[IX];
        G[IX] = GI;
        pr = true;
        break;
      }
    }
  }
  return result;
}

void SimplicialCubature::computeRuleValueAndError(const UnsignedInteger dimension, const Sample & simplexVertices,
    const Scalar volume, const UnsignedInteger outputDimension,
    const Function & function, const Matrix & G, const Matrix & W,
    const Indices & evalBudget, Point & value, Point & error) const
{
  const Scalar RTMN = 1e-1;
  const Scalar SMALL = SpecFunc::Precision;
  const Scalar ERRCOF = 8.0;

  const UnsignedInteger WTS = W.getNbRows();
  const UnsignedInteger RLS = W.getNbColumns();
  Matrix RULE(outputDimension, RLS);

  for (UnsignedInteger K = 0; K < WTS; ++ K)
    if (evalBudget[K] > 0)
    {
      const Point sym(computePermutationSums(dimension, simplexVertices, outputDimension, function, G.getColumn(K)*Point(1, 1.0)));
      for (UnsignedInteger i = 0; i < outputDimension; ++ i)
      {
        const Scalar scaledSymI = volume * sym[i];
        for (UnsignedInteger j = 0; j < RLS; ++ j)
          RULE(i, j) += scaledSymI * W(K, j);
      }
    }
  // Scale integral values and compute the error estimates.
  value = Point(outputDimension);
  error = Point(outputDimension);
  for (UnsignedInteger I = 0; I < outputDimension; ++ I)
  {
    value[I] = RULE(I, 0);
    const Scalar NMBS = std::abs(value[I]);
    Scalar NMCP = 0.0;
    Scalar RT = RTMN;
    for (SignedInteger K = RLS - 1 - ((RLS + 1) % 2); K >= 2; K -= 2)
    {
      const Scalar NMRL = std::max(std::abs(RULE(I, K)), std::abs(RULE(I, K - 1)));
      if ((NMRL > SMALL * NMBS) && (K < (SignedInteger)RLS - 1))
        RT = std::max(NMRL / NMCP, RT);
      error[I] = std::max(NMRL, error[I]);
      NMCP = NMRL;
    }
    if ((RT < 1.0) && (RLS > 3))
      error[I] = RT * NMCP;
    error[I] = std::max(ERRCOF * error[I], SMALL * NMBS);
  }
}

UnsignedInteger SimplicialCubature::computeNewSubregions(const UnsignedInteger dimension, const UnsignedInteger outputDimension,
    const Function & function, const UnsignedInteger bestSimplex,
    const UnsignedInteger flatSize, Collection<Sample> & flatVertices) const
{
  (void)outputDimension;
  UnsignedInteger CUTTF = 2;
  UnsignedInteger CUTTB = 8;

  // Compute the differences.
  UnsignedInteger IS = 1;
  UnsignedInteger JS = 2;
  Scalar DFMX = 0;
  Scalar EMX = 0;
  Sample V(flatVertices[bestSimplex]);

  Point CN(V.computeMean());
  Point FC(function(CN));
  Scalar DFMD = FC.norm1();

  Matrix FRTHDF(dimension, dimension + 1);

  UnsignedInteger IE = 0;
  UnsignedInteger JE = 0;
  UnsignedInteger IT = 0;
  UnsignedInteger JT = 0;
  Scalar DFNX = 0.0;

  for(UnsignedInteger I = 0; I < dimension; ++ I)
  {
    for(UnsignedInteger J = I + 1; J < dimension + 1 ; ++ J)
    {
      Point H(2.0 * (V[I] - V[J]) / (5.0 * (dimension + 1.0)));
      Scalar EWD = H.norm1();
      if (EWD >= EMX)
      {
        IE = I;
        JE = J;
        EMX = EWD;
      }
      Scalar DFR = (function(CN - 2 * H) + function(CN + 2 * H) + 6 * FC - 4 * (function(CN - H) + function(CN + H))).norm1();
      if ((DFMD + DFR / 8.0) == DFMD)
        DFR = 0.0;
      DFR = DFR * EWD;
      if (DFR >= DFMX)
      {
        IT = IS;
        JT = JS;
        DFNX = DFMX;
        IS = I;
        JS = J;
        DFMX = DFR;
      }
      else
      {
        if (DFR >= DFNX)
        {
          IT = I;
          JT = J;
          DFNX = DFR;
        }
      }
      FRTHDF(I, J) = DFR;
    }
  }

  UnsignedInteger NEW = 0;
  UnsignedInteger LS = 0;
  if (DFNX > DFMX / CUTTF)
    NEW = 4;
  else
  {
    NEW = 3;
    if (DFMX == 0.0)
    {
      IS = IE;
      JS = JE;
    }
    else
    {
      Scalar DFSMX = 0;
      for(UnsignedInteger L = 0; L < dimension + 1; ++ L)
      {
        if ((L != IS) && (L != JS))
        {
          IT = std::min(std::min(L, IS), JS);
          JT = std::max(std::max(L, IS), JS);
          const UnsignedInteger LT = IS + JS + L - IT - JT;
          const Scalar DFR = FRTHDF(IT, LT) + FRTHDF(LT, JT);
          if (DFR >= DFSMX)
          {
            DFSMX = DFR;
            LS = L;
          }
        }
      }
      const Scalar DIFIL = FRTHDF(std::min(IS, LS), std::max(IS, LS));
      const Scalar DIFLJ = FRTHDF(std::min(JS, LS), std::max(JS, LS));
      DFNX = DIFIL + DIFLJ - std::min(DIFIL, DIFLJ);
      if ((DFMX / CUTTB < DFNX) && (DIFIL > DIFLJ))
      {
        IT = IS;
        IS = JS;
        JS = IT;
      }
    }
  }

  // Copy vertices and volume for bestSimplex to new subregions
  flatVertices.resize(flatSize + NEW - 1);
  for (UnsignedInteger L = flatSize; L < flatSize + NEW - 1; ++L)
    flatVertices[L] = Sample(V);

  Point VTI(V[IS]);
  Point VTJ(V[JS]);

  if (NEW == 4)
  {
    // Compute four new subregions.
    flatVertices[bestSimplex][JS] = (VTI + VTJ) / 2.0;
    flatVertices[flatSize][IS] = VTI;
    flatVertices[flatSize][JS] = (VTI + VTJ) / 2.0;
    flatVertices[flatSize + 1][IS] = (VTI + VTJ) * 0.5;
    flatVertices[flatSize + 1][JS] = VTJ;
    flatVertices[flatSize + 2][IS] = (VTI + VTJ) * 0.5;
    flatVertices[flatSize + 2][JS] = VTJ;
    VTI = flatVertices[bestSimplex][IT];
    VTJ = flatVertices[bestSimplex][JT];
    flatVertices[bestSimplex][JT] = (VTI + VTJ) * 0.5;
    flatVertices[flatSize][IT] = (VTI + VTJ) * 0.5;
    flatVertices[flatSize][JT] = VTJ;
    VTI = flatVertices[flatSize + 2][IT];
    VTJ = flatVertices[flatSize + 1][JT];
    flatVertices[flatSize + 1][JT] = ( VTI + VTJ ) * 0.5;
    flatVertices[flatSize + 2][IT] = ( VTI + VTJ ) * 0.5;
    flatVertices[flatSize + 2][JT] = VTJ;
  }
  else
  {
    // Compute three new subregions.
    flatVertices[bestSimplex][JS] = (2.0 * VTI + VTJ) / 3.0;
    flatVertices[flatSize][IS] = (2.0 * VTI + VTJ) / 3.0;
    if ( DFMX / CUTTF < DFNX )
    {
      flatVertices[flatSize][JS] = VTJ;
      flatVertices[flatSize + 1][IS] = (2.0 * VTI + VTJ) / 3.0;
      flatVertices[flatSize + 1][JS] = VTJ;
      VTJ = flatVertices[flatSize][JS];
      Point VTL = flatVertices[flatSize][LS];
      flatVertices[flatSize][LS] = (VTJ + VTL) * 0.5;
      flatVertices[flatSize + 1][JS] = (VTJ + VTL) * 0.5;
      flatVertices[flatSize + 1][LS] = VTL;
    }
    else
    {
      flatVertices[flatSize][JS] = (VTI + 2.0 * VTJ) / 3.0;
      flatVertices[flatSize + 1][IS] = (VTI + 2.0 * VTJ) / 3.0;
      flatVertices[flatSize + 1][JS] = VTJ;
    }
  }
  return NEW;
}

Indices SimplicialCubature::initializeBasicRule(const UnsignedInteger dimension, Matrix & W, Matrix & G) const
{
  UnsignedInteger RLS = 0;
  UnsignedInteger GMS = 0;
  UnsignedInteger WTS = 0;
  switch (rule_)
  {
    case 1:
      RLS = 3;
      GMS = 2;
      WTS = 3;
      break;
    case 2:
      RLS = 5;
      GMS = 4;
      WTS = 6;
      break;
    case 3:
      RLS = 7;
      GMS = 7;
      WTS = 11;
      break;
    case 4:
      RLS = 7;
      GMS = 12;
      WTS = 21;
      if (dimension == 2)
      {
        GMS = 11;
        WTS = 20;
      }
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid rule: " << rule_;
  }
  W = Matrix(WTS, RLS);
  Indices evalBudget(WTS);
  G = Matrix(dimension + 1, WTS);

  const UnsignedInteger N = dimension;
  const UnsignedInteger NP = N + 1;
  const UnsignedInteger N2 = NP * (N + 2);
  const UnsignedInteger N4 = N2 * (N + 3) * (N + 4);
  const UnsignedInteger N6 = N4 * (N + 5) * (N + 6);
  const UnsignedInteger N8 = N6 * (N + 7) * (N + 8);

  for (UnsignedInteger j = 0; j < N + 1; ++ j)
    G(j, 0) = 1.0 / NP;
  evalBudget[0] = 1;
  Scalar R1 = (N + 4.0 - std::sqrt(15.0)) / ( N * N + 8 * N + 1.0 );
  Scalar S1 = 1.0 - N * R1;
  Scalar L1 = S1 - R1;

  G(0, GMS) = S1;
  for (UnsignedInteger i = 1; i < NP; ++ i)
    G(i, GMS) = R1;
  evalBudget[GMS] = N + 1;
  UnsignedInteger IW = RLS - 1;//index

  if (rule_ < 4)
  {
    W(0, IW) = 1;
    -- IW;
    W(GMS, IW) = 1.0 / NP;
    -- IW;
  }

  // Compute weights, generators and evalBudget for degree 3 rule.
  G(0, 1) = 3.0 / (N + 3.0);
  for (UnsignedInteger i = 1; i < NP; ++ i)
    G(i, 1) = 1.0 / (N + 3.0);
  evalBudget[1] = NP;

  W(1, IW) = std::pow((N + 3.0), 3.0) / (4.0 * N2 * (N + 3.0));
  Scalar L2 = 0.0;

  if (rule_ > 1)
  {
    -- IW;
    // Compute weights, generators and evalBudget for degree 3 and 5 rules.
    if (N == 2)
    {
      // Special degree 3 rule.
      L2 = 0.62054648267200632589046034361711;
      L1 = -std::sqrt(0.5 - L2 * L2);
      R1 = (1.0 - L1) / 3.0;
      S1 = 1.0 - 2.0 * R1;
      G(0, GMS) = S1;
      for (UnsignedInteger i = 1; i < NP; ++ i)
        G(i, GMS) = R1;
      evalBudget[GMS] = 3;
      W(GMS, IW) = 1.0 / 6.0;
      Scalar R2 = (1.0 - L2) / 3.0;
      Scalar S2 = 1.0 - 2 * R2;
      G(0, GMS + 1) = S2;
      for (UnsignedInteger i = 1; i < NP; ++ i)
        G(i, GMS + 1) = R2;
      evalBudget[GMS + 1] = 3;
      W(GMS + 1, IW) = 1.0 / 6.0;
    }
    else
    {
      // Degree 3 rule using Stroud points.
      Scalar R2 = (N + 4.0 + std::sqrt(15.0)) / (N * N + 8.0 * N + 1.0);
      Scalar S2 = 1.0 - N * R2;
      L2 = S2 - R2;
      G(0, GMS + 1) = S2;
      for (UnsignedInteger i = 1; i < NP; ++ i)
        G(i, GMS + 1) = R2;
      evalBudget[GMS + 1] = NP;
      W(GMS + 1, IW) = (2.0 / (N + 3.0) - L1 ) / (N2 * (L2 - L1) * L2 * L2);
      W(GMS, IW) = (2.0 / (N + 3.0) - L2) / (N2 * (L1 - L2) * L1 * L1);
    }
    --IW;

    // Grundmann-Moller degree 5 rule.
    G(0, 2) = 5.0 / (N + 5.0);
    for (UnsignedInteger i = 1; i < NP; ++ i)
      G(i, 2) = 1.0 / (N + 5.0);
    evalBudget[2] = NP;
    G(0, 3) = 3.0 / (N + 5.0);
    G(1, 3) = 3.0 / (N + 5.0);
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, 3) = 1.0 / (N + 5.0);
    evalBudget[3] = NP * N / 2;
    W(1, IW) = -std::pow(N + 3.0, 5.0) / (16.0 * N4);
    for (UnsignedInteger i = 2; i < 4; ++ i)
      W(i, IW) = std::pow(N + 5.0, 5.0) / (16.0 * N4 * (N + 5.0));
  }

  if (rule_ > 2)
  {
    -- IW;
    // Compute weights, generators and evalBudget for degree 5 and 7 rules.
    // Stroud degree 5 rule.
    Scalar U1 = (N + 7.0 + 2.0 * std::sqrt(15.0)) / (N * N + 14.0 * N - 11.0);
    Scalar V1 = (1.0 - (N - 1.0) * U1) / 2.0;
    Scalar D1 = V1 - U1;
    G(0, GMS + 2) = V1;
    G(1, GMS + 2) = V1;
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, GMS + 2) = U1;
    evalBudget[GMS + 2] = ((N + 1) * N) / 2;
    Scalar U2 = (N + 7.0 - 2 * std::sqrt(15.0)) / (N * N + 14.0 * N - 11.0);
    Scalar V2 = (1.0 - (N - 1.0) * U2) / 2.0;
    Scalar D2 = V2 - U2;
    G(0, GMS + 3) = V2;
    G(1, GMS + 3) = V2;
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, GMS + 3) = U2;
    evalBudget[GMS + 3] = ((N + 1) * N) / 2;
    if (N == 2)
    {
      W(GMS + 2, IW) = (155.0 - std::sqrt(15.0)) / 1200.0;
      W(GMS + 3, IW) = (155.0 + std::sqrt(15.0)) / 1200.0;
      W(0, IW) = 1.0 - 3.0 * (W(GMS + 2, IW) + W(GMS + 3, IW));
    }
    else if (N == 3)
    {
      W(GMS, IW) = (2665.0 + 14.0 * std::sqrt(15.0)) / 37800.0;
      W(GMS + 1, IW) = (2665.0 - 14.0 * std::sqrt(15.0)) / 37800.0;
      W(GMS + 2, IW) = 2.0 * 15.0 / 567.0;
      evalBudget[GMS + 3] = 0;
    }
    else
    {
      W(GMS, IW) = (2.0 * (27.0 - N) / (N + 5.0) - L2 * (13.0 - N) ) / ( std::pow(L1, 4.0) * (L1 - L2) * N4 );
      W(GMS + 1, IW) = (2.0 * (27.0 - N) / (N + 5.0) - L1 * (13 - N) ) / ( std::pow(L2, 4.0) * (L2 - L1) * N4 );
      W(GMS + 2, IW) = (2.0 / (N + 5.0) - D2 ) / ( N4 * ( D1 - D2 ) * std::pow(D1, 4.0) );
      W(GMS + 3, IW) = (2.0 / (N + 5.0) - D1 ) / ( N4 * ( D2 - D1 ) * std::pow(D2, 4.0) );
    }
    -- IW;
    // Grundmann-Moller degree 7 rule.
    G(0, 4) = 7.0 / ( N + 7.0 );
    for (UnsignedInteger i = 1; i < NP; ++ i)
      G(i, 4) = 1.0 / ( N + 7.0 );
    evalBudget[4] = NP;
    G(0, 5) = 5.0 / (N + 7.0);
    G(1, 5) = 3.0 / (N + 7.0);
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, 5) = 1.0 / (N + 7.0);
    evalBudget[5] = NP * N;
    for (UnsignedInteger i = 0; i < 3; ++ i)
      G(i, 6) = 3.0 / (N + 7.0);
    if(NP > 3)
      for (UnsignedInteger i = 3; i < NP; ++ i)
        G(i, 6) = 1.0 / (N + 7.0);
    evalBudget[6] = NP * N * (N - 1.0) / 6.0;
    W(1, IW) =  std::pow(N + 3.0, 7.0) / (2.0 * 64.0 * N4 * (N + 5.0));
    for (UnsignedInteger i = 2; i < 4; ++ i)
      W(i, IW) = -std::pow(N + 5.0, 7.0) / (64.0 * N6);
    for (UnsignedInteger i = 4; i < 7; ++ i)
      W(i, IW) = std::pow(N + 7, 7.0) / (64.0 * N6 * (N + 7.0));
  }

  if (rule_ == 4)
  {
    -- IW;
    //  Compute weights, generators and evalBudget for degree 7, 9 rules.
    // Mysovskikh degree 7 rule.
    Scalar SG = 1.0 / (23328.0 * N6);
    Scalar U5 = -std::pow(6.0, 3.0) * SG * ( 52212.0 - N * ( 6353.0 + N * ( 1934.0 - N * 27.0 ) ) );
    Scalar U6 =  std::pow(6.0, 4.0) * SG * (  7884.0 - N * ( 1541.0 - N * 9 ) );
    Scalar U7 = -std::pow(6.0, 5.0) * SG * (  8292.0 - N * ( 1139.0 - N * 3 ) ) / ( N + 7.0 );
    Scalar P0 = -144.0 * ( 142528.0 + N * ( 23073.0 - N * 115.0 ) );
    Scalar P1 = -12.0 * ( 6690556.0 + N * ( 2641189.0 + N * ( 245378.0 - N * 1495.0 ) ) );
    Scalar P2 = -16.0 * (6503401.0 + N * (4020794.0 + N * (787281.0 + N * (47323.0 - N * 385.0))));
    Scalar P3 = -( 6386660.0 + N * (4411997.0 + N * (951821.0 + N * (61659.0 - N * 665.0))) ) * ( N + 7.0 );
    Scalar A = P2 / (3.0 * P3);
    Scalar P = A * (P1 / P2 - A);
    Scalar Q = A * (2.0 * A * A - P1 / P3) + P0 / P3;
    Scalar R = std::sqrt(-std::pow(P, 3.0));
    Scalar TH = acos(-Q / (2.0 * R)) / 3.0;
    R = 2.0 * std::pow(R, 1.0 / 3.0);
    Scalar TP = 2.0 * M_PI / 3.0;
    Scalar A1 = -A + R * std::cos(TH);
    Scalar A2 = -A + R * std::cos(TH + 2 * TP);
    Scalar A3 = -A + R * std::cos(TH + TP);
    G(0, GMS + 4) = (1.0 - N * A1) / NP;
    for (UnsignedInteger i = 1; i < NP; ++ i)
      G(i, GMS + 4) = (1.0 + A1) / NP;
    evalBudget[GMS + 4] = NP;
    G(0, GMS + 5) = (1.0 - N * A2) / NP;
    for (UnsignedInteger i = 1; i < NP; ++ i)
      G(i, GMS + 5) = (1.0 + A2) / NP;
    evalBudget[GMS + 5] = NP;
    G(0, GMS + 6) = (1.0 - N * A3) / NP;
    for (UnsignedInteger i = 1; i < NP; ++ i)
      G(i, GMS + 6) = (1.0 + A3) / NP;
    evalBudget[GMS + 6] = NP;
    W(GMS + 4, IW) = ( U7 - (A2 + A3) * U6 + A2 * A3 * U5 ) / ( std::pow(A1, 2.0) - (A2 + A3) * A1 + A2 * A3 ) / std::pow(A1, 5.0);
    W(GMS + 5, IW) = ( U7 - (A1 + A3) * U6 + A1 * A3 * U5 ) / ( std::pow(A2, 2.0) - (A1 + A3) * A2 + A1 * A3 ) / std::pow(A2, 5.0);
    W(GMS + 6, IW) = ( U7 - (A2 + A1) * U6 + A2 * A1 * U5 ) / ( std::pow(A3, 2.0) - (A2 + A1) * A3 + A2 * A1 ) / std::pow(A3, 5.0);
    G(0, GMS + 7) = 4.0 / (N + 7.0);
    G(1, GMS + 7) = 4.0 / (N + 7.0);
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, GMS + 7) = 1.0 / (N + 7.0);
    evalBudget[GMS + 7] = NP * N / 2.0;
    W(GMS + 7, IW) = 10.0 * std::pow(N + 7.0, 6.0) / ( 729.0 * N6 );
    G(0, GMS + 8) = 11.0 / ( N + 7.0 ) / 2.0;
    G(1, GMS + 8) =  5.0 / ( N + 7.0 ) / 2.0;
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, GMS + 8) = 1.0 / (N + 7.0);
    evalBudget[GMS + 8] = NP * N;
    W(GMS + 8, IW) = 64.0 * std::pow(N + 7.0, 6.0) / ( 6561.0 * N6 );
    W(3, IW) = W(3, IW + 1);
    W(6, IW) = W(6, IW + 1);
    -- IW;

    // Grundmann-Moller degree 9 rule.
    G(0, 7) = 9.0 / (N + 9.0);
    for (UnsignedInteger i = 1; i < NP; ++ i)
      G(i, 7) = 1.0 / (N + 9.0);
    evalBudget[7] = NP;
    G(0, 8) = 7.0 / (N + 9.0);
    G(1, 8) = 3.0 / (N + 9.0);
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, 8) = 1.0 / (N + 9.0);
    evalBudget[8] = NP * N;
    for (UnsignedInteger i = 0; i < 2; ++ i)
      G(i, 9) = 5.0 / ( N + 9.0 );
    for (UnsignedInteger i = 2; i < NP; ++ i)
      G(i, 9) = 1.0 / ( N + 9.0 );
    evalBudget[9] = NP * N / 2;
    G(0, 10) = 5.0 / (N + 9.0);
    for (UnsignedInteger i = 1; i < 3; ++ i)
      G(i, 10) = 3.0 / (N + 9.0);
    if(NP > 3)
      for (UnsignedInteger i = 3; i < NP; ++ i)
        G(i, 10) = 1.0 / (N + 9.0);
    evalBudget[10] = NP * N * ( N - 1 ) / 2;
    W(1, IW) = -std::pow( N + 3.0, 9.0) / (6 * 256.0 * N6);
    for (UnsignedInteger i = 2; i < 4; ++ i)
      W(i, IW) =  std::pow( N + 5.0, 9.0) / (2 * 256.0 * N6 * (N + 7));
    for (UnsignedInteger i = 4; i < 7; ++ i)
      W(i, IW) = -std::pow( N + 7.0, 9.0) / (256.0 * N8);
    for (UnsignedInteger i = 7; i < 11; ++ i)
      W(i, IW) =  std::pow( N + 9.0, 9.0) / (256.0 * N8 * (N + 9));
    if (N > 2)
    {
      for (UnsignedInteger i = 0; i < 4; ++ i)
        G(i, 11) = 3.0 / (N + 9.0);
      if (NP > 4)
        for (UnsignedInteger i = 4; i < NP; ++ i)
          G(i, 11) = 1.0 / (N + 9.0);
      evalBudget[11] = NP * N * (N - 1) * (N - 2) / 24;
      W(11, IW) = W(7, IW);
    }
  }

  // Compute unnormalized weights.
  for (UnsignedInteger j = 0; j < RLS; ++ j)
  {
    Scalar sum = 0.0;
    for (UnsignedInteger i = 1; i < WTS; ++ i)
      sum += evalBudget[i] * W(i, j);
    W(0, j) = 1.0 - sum;
  }
  Scalar NB = 0.0;
  for (UnsignedInteger i = 0; i < WTS; ++ i)
    NB += evalBudget[i] * W(i, 0) * W(i, 0);
  for (UnsignedInteger j = 1; j < RLS; ++ j)
  {
    for (UnsignedInteger i = 0; i < WTS; ++ i)
      W(i, j) -= W(i, 0);
  }

  // Orthogonalize and normalize null rules.
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < WTS; ++ i)
    sum += evalBudget[i] * W(i, 1) * W(i, 1);
  for (UnsignedInteger i = 0; i < WTS; ++ i)
    W(i, 1) *= std::sqrt(NB / sum);

  for (UnsignedInteger K = 2; K < RLS; ++ K)
  {
    // recopy W without some columns on left and right
    Matrix Wkr(WTS, K - 1);
    for (UnsignedInteger j = 0; j < K - 1; ++ j)
      for (UnsignedInteger i = 0; i < WTS; ++ i)
        Wkr(i, j) = W(i, j + 1);

    Point wpts(WTS);
    for (UnsignedInteger i = 0; i < WTS; ++ i)
      wpts[i] = evalBudget[i] * W(i, K);
    Point sub(Wkr * Wkr.transpose() * wpts);
    for (UnsignedInteger i = 0; i < WTS; ++ i)
      W(i, K) -= sub[i] / NB;
    sum = 0.0;
    for (UnsignedInteger i = 0; i < WTS; ++ i)
      sum += evalBudget[i] * W(i, K) * W(i, K);
    for (UnsignedInteger i = 0; i < WTS; ++ i)
      W(i, K) *= std::sqrt(NB / sum);
  }
  return evalBudget;
}


END_NAMESPACE_OPENTURNS
