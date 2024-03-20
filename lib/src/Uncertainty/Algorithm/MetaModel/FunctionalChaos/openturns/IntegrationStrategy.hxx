//                                               -*- C++ -*-
/**
 *  @brief An implementation of projection strategy as a leastSquares
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INTEGRATIONSTRATEGY_HXX
#define OPENTURNS_INTEGRATIONSTRATEGY_HXX

#include "openturns/ProjectionStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class IntegrationStrategy
 *
 * An implementation of projection strategy as a leastSquares
 */

class OT_API IntegrationStrategy
  : public ProjectionStrategyImplementation
{
  CLASSNAME
public:
  typedef ProjectionStrategyImplementation::FunctionCollection FunctionCollection;


  /** Default constructor */
  IntegrationStrategy();

  /** Parameter constructor */
  IntegrationStrategy(const Distribution & measure);

  /** Parameter constructor */
  IntegrationStrategy(const WeightedExperiment & weightedExperiment);

  /** Parameter constructor */
  IntegrationStrategy(const Distribution & measure,
                      const WeightedExperiment & weightedExperiment);

  /** Parameter constructor */
  IntegrationStrategy(const Sample & inputSample,
                      const Point & weights,
                      const Sample & outputSample);

  /** Parameter constructor */
  IntegrationStrategy(const Sample & inputSample,
                      const Sample & outputSample);

  /** Virtual constructor */
  IntegrationStrategy * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Selection/error history accessor */
  Collection<Indices> getSelectionHistory(Collection<Point> & coefficientsHistory) const override;
  Point getErrorHistory() const override;

protected:
  /** Compute the components alpha_k_p_ by projecting the model on the partial L2 basis */
  void computeCoefficients(const Function & function,
                           const FunctionCollection & basis,
                           const Indices & indices,
                           const Indices & addedRanks,
                           const Indices & conservedRanks,
                           const Indices & removedRanks,
                           const UnsignedInteger marginalIndex = 0) override;
private:
  Collection<Indices> indicesHistory_;
  PersistentCollection<Point> coefficientsHistory_;
} ; /* class IntegrationStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTEGRATIONSTRATEGY_HXX */
