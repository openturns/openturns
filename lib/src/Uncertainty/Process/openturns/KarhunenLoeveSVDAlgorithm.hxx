//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         SVD decomposition of a process sample.
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
#ifndef OPENTURNS_KARHUNENLOEVESVDALGORITHM_HXX
#define OPENTURNS_KARHUNENLOEVESVDALGORITHM_HXX

#include "openturns/KarhunenLoeveAlgorithmImplementation.hxx"
#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveSVDAlgorithm
 */

class OT_API KarhunenLoeveSVDAlgorithm
  : public KarhunenLoeveAlgorithmImplementation
{

  CLASSNAME

public:

  /** Default constructor without parameters */
  KarhunenLoeveSVDAlgorithm();

  /** Parameters constructor */
  KarhunenLoeveSVDAlgorithm(const ProcessSample & sample,
                            const Scalar threshold = 0.0,
                            const Bool centeredSample = false);

  KarhunenLoeveSVDAlgorithm(const ProcessSample & sample,
                            const Point & verticesWeights,
                            const Scalar threshold = 0.0,
                            const Bool centeredSample = false);

  KarhunenLoeveSVDAlgorithm(const ProcessSample & sample,
                            const Point & verticesWeights,
                            const Point & sampleWeights,
                            const Scalar threshold = 0.0,
                            const Bool centeredSample = false);

  /** Virtual copy constructor */
  KarhunenLoeveSVDAlgorithm * clone() const override;

  /** Solve the Fredholm eigenvalues problem:
   * find (\phi_k, \lambda_k) such that
   * \int_{D} C(s,t)\phi_k(s)ds=\lambda_k\phi_k(t)
   * where C is a given covariance model, using SVD approximation
   */
  void run() override;

  /** Process sample accessor */
  ProcessSample getSample() const;

  /** Vertices weights accessor */
  Point getVerticesWeights() const;
private:
  void setVerticesWeights(const Point & verticesWeights);
public:

  /** Sample weights accessor */
  Point getSampleWeights() const;
private:
  void setSampleWeights(const Point & sampleWeights);
public:

  /** Covariance model accessors */
  CovarianceModel getCovarianceModel() const override;
private:
  /** Hide the covariance model accessor as it has no meaning in the SVD context */
  void setCovarianceModel(const CovarianceModel & covariance) override;
public:

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Underlying sample */
  ProcessSample sample_;

  /** Flag to tell if the sample is centered */
  Bool centeredSample_;

  /** Potential vertices weights */
  Point verticesWeights_;

  /** Flag to tell if the weights are uniform */
  Bool uniformVerticesWeights_;

  /** Potential sample weights */
  Point sampleWeights_;

  /** Flag to tell if the sample weights are uniform */
  Bool uniformSampleWeights_;
} ; /* class KarhunenLoeveSVDAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVESVDALGORITHM_HXX */
