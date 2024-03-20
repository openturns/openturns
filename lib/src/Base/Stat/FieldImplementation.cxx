//                                               -*- C++ -*-
/**
 *  @brief The class FieldImplementation implements values indexed by
 *  the vertices of a Mesh
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

#include <algorithm>
#include <string>
#include "openturns/OTconfig.hxx"
#include "openturns/FieldImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Path.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/PolygonArray.hxx"
#include "openturns/Os.hxx"
#include "openturns/PlatformInfo.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<Sample>)

static const Factory<PersistentCollection<Sample> > Factory_PersistentCollection_Sample;

CLASSNAMEINIT(FieldImplementation)

static const Factory<FieldImplementation> Factory_FieldImplementation;

/* Default constructor is private */
FieldImplementation::FieldImplementation()
  : PersistentObject()
  , mesh_()
  , values_(mesh_.getVerticesNumber(), 0)
  , description_(mesh_.getDescription())
  , inputMean_(0)
  , isAlreadyComputedInputMean_(false)
{
  // Nothing to do
}

/* Standard constructor */
FieldImplementation::FieldImplementation(const Mesh & mesh,
    const UnsignedInteger dim)
  : PersistentObject()
  , mesh_(mesh)
  , values_(mesh.getVerticesNumber(), dim)
  , description_(0)
  , inputMean_(dim)
  , isAlreadyComputedInputMean_(false)
{
  // Build the default description
  Description description(mesh_.getVertices().getDescription());
  description.add(values_.getDescription());
  setDescription(description);
}

/* Constructor from a Mesh and a sample */
FieldImplementation::FieldImplementation(const Mesh & mesh,
    const Sample & values)
  : PersistentObject()
  , mesh_(mesh)
  , values_(values)
  , description_()
  , inputMean_(values.getDimension())
  , isAlreadyComputedInputMean_(false)
{
  if (mesh.getVerticesNumber() != values.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot build a Field with a number of values=" << values.getSize() << " different from the number of vertices=" << mesh.getVerticesNumber();
  Description description(mesh_.getVertices().getDescription());
  description.add(values_.getDescription());
  setDescription(description);
}

/* Virtual constructor */
FieldImplementation * FieldImplementation::clone() const
{
  return new FieldImplementation(*this);
}

/* Size accessor */
UnsignedInteger FieldImplementation::getSize() const
{
  return values_.getSize();
}

/* Dimension accessor */
UnsignedInteger FieldImplementation::getInputDimension() const
{
  return mesh_.getDimension();
}

UnsignedInteger FieldImplementation::getOutputDimension() const
{
  return values_.getDimension();
}

/* Mesh accessor */
Mesh FieldImplementation::getMesh() const
{
  return mesh_;
}

RegularGrid FieldImplementation::getTimeGrid() const
{
  return mesh_;
}

/* Individual value accessor */
NSI_point FieldImplementation::operator[](const UnsignedInteger index)
{
  isAlreadyComputedInputMean_ = false;
  return values_[index];
}

NSI_const_point FieldImplementation::operator[](const UnsignedInteger index) const
{
  return values_[index];
}

Scalar & FieldImplementation::operator () (const UnsignedInteger i,
    const UnsignedInteger j)
{
  isAlreadyComputedInputMean_ = false;
#ifdef DEBUG_BOUNDCHECKING
  // No copyOnWrite() as the at() method already do it
  return at(i, j);
#else
  return values_(i, j);
#endif /* DEBUG_BOUNDCHECKING */
}

const Scalar & FieldImplementation::operator () (const UnsignedInteger i,
    const UnsignedInteger j) const
{
#ifdef DEBUG_BOUNDCHECKING
  return at(i, j);
#else
  return values_(i, j);
#endif /* DEBUG_BOUNDCHECKING */
}


NSI_point FieldImplementation::at (const UnsignedInteger index)
{
  if (!(index < getSize())) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  isAlreadyComputedInputMean_ = false;
  return (*this)[index];
}

NSI_const_point FieldImplementation::at (const UnsignedInteger index) const
{
  if (!(index < getSize())) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  return (*this)[index];
}

Scalar & FieldImplementation::at (const UnsignedInteger i,
                                  const UnsignedInteger j)
{
  if (!(i < getSize())) throw OutOfBoundException(HERE) << "i (" << i << ") is not less than size (" << getSize() << ")";
  if (!(j < getOutputDimension())) throw OutOfBoundException(HERE) << "j (" << j << ") is not less than dimension (" << getOutputDimension() << ")";
  isAlreadyComputedInputMean_ = false;
  return values_(i, j);
}

const Scalar & FieldImplementation::at (const UnsignedInteger i,
                                        const UnsignedInteger j) const
{
  if (!(i < getSize())) throw OutOfBoundException(HERE) << "i (" << i << ") is not less than size (" << getSize() << ")";
  if (!(j < getOutputDimension())) throw OutOfBoundException(HERE) << "j (" << j << ") is not less than dimension (" << getOutputDimension() << ")";
  return values_(i, j);
}

/* Data accessors */
Point FieldImplementation::getValueAtIndex(const UnsignedInteger index) const
{
  return values_[index];
}

void FieldImplementation::setValueAtIndex(const UnsignedInteger index,
    const Point & val)
{
  isAlreadyComputedInputMean_ = false;
  values_[index] = val;
}

/* Get the i-th marginal field */
FieldImplementation FieldImplementation::getMarginal(const UnsignedInteger index) const
{
  return FieldImplementation(mesh_, values_.getMarginal(index));
}

/* Get the marginal field corresponding to indices dimensions */
FieldImplementation FieldImplementation::getMarginal(const Indices & indices) const
{
  return FieldImplementation(mesh_, values_.getMarginal(indices));
}


/* Description Accessor */
void FieldImplementation::setDescription(const Description & description)
{
  if (description.getSize() != (getInputDimension() + getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the given description does not match the field input+output dimension.";
  description_ = description;
}

/* Description Accessor */
Description FieldImplementation::getDescription() const
{
  return description_;
}

/* Comparison function */
Bool FieldImplementation::operator ==(const FieldImplementation & other) const
{
  if (this == &other) return true;
  return (mesh_ == other.mesh_) && (values_ == other.values_);
}

/* String converter */
String FieldImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FieldImplementation::GetClassName()
      << " name=" << getName()
      << " mesh=" << mesh_
      << " values=" << values_;
  return oss;
}

String FieldImplementation::__str__(const String & offset) const
{
  Sample data(mesh_.getVertices());
  data.stack(values_);
  return data.__str__(offset);
}

/* TBB functor to speed-up input mean computation */
struct FieldInputMeanFunctor
{
  const Point & volumes_;
  const FieldImplementation & field_;
  Point accumulator_;

  FieldInputMeanFunctor(const Point & volumes, const FieldImplementation & field)
    : volumes_(volumes), field_(field), accumulator_(field.getOutputDimension(), 0.0) {}

  FieldInputMeanFunctor(const FieldInputMeanFunctor & other, TBBImplementation::Split)
    : volumes_(other.volumes_), field_(other.field_), accumulator_(other.field_.getOutputDimension(), 0.0) {}

  void operator() (const TBBImplementation::BlockedRange<UnsignedInteger> & r)
  {
    const UnsignedInteger meshDimension = field_.getInputDimension();
    const UnsignedInteger dimension = field_.getOutputDimension();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const Indices simplex(field_.mesh_.getSimplex(i));
      Point meanValue(dimension, 0.0);
      for (UnsignedInteger j = 0; j <= meshDimension; ++j) meanValue += field_.values_[simplex[j]];
      accumulator_ += (volumes_[i] / dimension) * meanValue;
    }
  }

  void join(const FieldInputMeanFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; /* end struct FieldInputMeanFunctor */

/* Compute the input mean of the field */
void FieldImplementation::computeInputMean() const
{
  const Point simplicesVolume(mesh_.computeSimplicesVolume());
  const Scalar totalVolume(simplicesVolume.norm1());
  if (!(totalVolume > 0.0)) throw InternalException(HERE) << "Error: cannot compute the input mean of a field supported by a mesh of zero volume.";
  FieldInputMeanFunctor functor( simplicesVolume, *this );
  TBBImplementation::ParallelReduce( 0, mesh_.getSimplicesNumber(), functor );
  inputMean_ = functor.accumulator_ / totalVolume;
  isAlreadyComputedInputMean_ = true;
}


/* Compute the input mean of the field */
Point FieldImplementation::getInputMean() const
{
  if (!isAlreadyComputedInputMean_) computeInputMean();
  return inputMean_;
}

/* Compute the input mean of the field */
Point FieldImplementation::getOutputMean() const
{
  if (!mesh_.isRegular() || (mesh_.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the temporal mean is defined only when the mesh is regular and of dimension 1.";
  return values_.computeMean();
}

/* l2 norm */
Scalar FieldImplementation::norm() const
{
  const UnsignedInteger size = values_.getSize();
  const Point w(mesh_.computeWeights());
  Scalar normSquare = 0.0;
  for (UnsignedInteger i = 0; i < size; ++ i)
    normSquare += w[i] * Point(values_[i]).normSquare();
  return std::sqrt(normSquare);
}

Sample FieldImplementation::getValues() const
{
  return values_;
}

void FieldImplementation::setValues(const Sample & values)
{
  if (values.getSize() != mesh_.getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: expected a sample of size=" << mesh_.getVerticesNumber() << ", got size=" << values.getSize();
  values_ = values;
}

/* Return the field as a sample, ie its values and positions */
Sample FieldImplementation::asSample() const
{
  Sample data(mesh_.getVertices());
  data.stack(values_);
  return data;
}

/* Return the field as a defomed mesh, ie the dimension of the vertices is augmented using zero coordinates at the positions given by the first Indices, the dimension of the values is augmented the same way by adding zero values at the positions given by the second indices, then the vertices are translated by the values */
Mesh FieldImplementation::asDeformedMesh(const Indices & verticesPadding,
    const Indices & valuesPadding) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger verticesPaddingSize = verticesPadding.getSize();
  const UnsignedInteger augmentedInputDimension = inputDimension + verticesPaddingSize;
  if (!verticesPadding.check(augmentedInputDimension)) throw InvalidArgumentException(HERE) << "Error: the given indices=" << verticesPadding << " of null coordinates for padding have ties or leave gaps in the augmented coordinates.";
  const UnsignedInteger outputDimension = getOutputDimension();
  const UnsignedInteger valuesPaddingSize = valuesPadding.getSize();
  const UnsignedInteger augmentedOutputDimension = outputDimension + valuesPaddingSize;
  if (!valuesPadding.check(augmentedOutputDimension)) throw InvalidArgumentException(HERE) << "Error: the given indices=" << valuesPadding << " of null values for padding have ties or leave gaps in the augmented values.";
  // Most common situation: no dimension augmentation
  if ((verticesPaddingSize == 0) && (valuesPaddingSize == 0))
  {
    if (inputDimension != outputDimension) throw InternalException(HERE) << "Error: cannot deform the mesh if the dimension of the values=" << outputDimension << " does not match the mesh dimension=" << inputDimension << ". Maybe you should augment one of the dimensions by padding zeros.";
    return Mesh(mesh_.getVertices() + values_, mesh_.getSimplices());
  }
  const Indices nonZeroCoordinates(verticesPadding.complement(augmentedInputDimension));
  const UnsignedInteger nonZeroCoordinatesSize = nonZeroCoordinates.getSize();
  const Indices nonZeroValues(valuesPadding.complement(augmentedOutputDimension));
  const UnsignedInteger nonZeroValuesSize = nonZeroValues.getSize();
  const UnsignedInteger size = values_.getSize();
  Sample vertices(mesh_.getVertices());
  Sample deformedVertices(size, augmentedInputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < nonZeroCoordinatesSize; ++j)
      deformedVertices(i, nonZeroCoordinates[j]) = vertices(i, j);
    for (UnsignedInteger j = 0; j < nonZeroValuesSize; ++j)
      deformedVertices(i, nonZeroValues[j]) += values_(i, j);
  }
  // If the input dimension has not changed we can reuse the topology
  const IndicesCollection oldSimplices(mesh_.getSimplices());
  if (verticesPaddingSize == 0) return Mesh(deformedVertices, oldSimplices);
  // Otherwise we have to adapt the topology
  IndicesCollection newSimplices(oldSimplices.getSize(), augmentedInputDimension + 1);
  for (UnsignedInteger i = 0; i < oldSimplices.getSize(); ++i)
  {
    // First copy the old vertices indices
    for (UnsignedInteger j = 0; j <= inputDimension; ++j)
      newSimplices(i, j) = oldSimplices(i, j);
    // Then duplicate the last index to indicate a lower dimensional manifold
    for (UnsignedInteger j = inputDimension + 1; j <= augmentedInputDimension; ++j)
      newSimplices(i, j) = oldSimplices(i, inputDimension);
  } // i
  return Mesh(deformedVertices, newSimplices);
}

/* Draw a marginal of the Field */
Graph FieldImplementation::draw() const
{
  // Specific drawing method for bidimensional fields indexed by a scalar
  if ((getInputDimension() == 1) && (getOutputDimension() == 2))
  {
    const String title(OSS() << getName());
    Graph graph(title, description_[0], description_[1], true, "");
    const Curve curveSerie(getValues());
    graph.add(curveSerie);
    return graph;
  }
  // Specific drawing method for bidimensional fields indexed by a 2d-point
  if ((getInputDimension() == 2) && (getOutputDimension() == 2))
  {
    const String title(OSS() << getName());
    Graph graph(title, description_[0], description_[1], true, "");
    // Get the bounding box of the mesh to set the head size of the arrow
    // It must be independent from the values as we want the same size for
    // all the arrows
    const Sample vertices(mesh_.getVertices());
    const Point xMin(vertices.getMin());
    const Point xMax(vertices.getMax());
    // To take into account the user-defined scaling factor
    const Bool automaticScaling = ResourceMap::GetAsBool("Field-AutomaticScaling");
    Scalar rho = ResourceMap::GetAsScalar("Field-ArrowScaling");
    // To take into account the domain size in the scaling
    const Scalar scaling = std::min(xMax[0] - xMin[0], xMax[1] - xMin[1]);
    if (automaticScaling) rho *= 2.0 * scaling;
    const Scalar delta = scaling * ResourceMap::GetAsScalar("Field-ArrowRatio");
    const UnsignedInteger size = values_.getSize();
    // To take into account the field values in the scaling
    Sample normValues(size, 1);
    for (UnsignedInteger i = 0; i < size; ++i)
      normValues(i, 0) = Point(values_[i]).norm();
    Scalar normMin = normValues.getMin()[0];
    Scalar normMax = normValues.getMax()[0];
    if (normMax == normMin) normMax = normMin + 1.0;
    if (automaticScaling) rho /= normMax;
    // To take into account the vertices number
    if (automaticScaling) rho /= std::sqrt(static_cast<Scalar>(vertices.getSize()));
    const UnsignedInteger levelsNumber = ResourceMap::GetAsUnsignedInteger("Field-LevelNumber");
    Description palette(levelsNumber);
    for (UnsignedInteger i = 0; i < levelsNumber; ++i)
      palette[i] = Curve::ConvertFromHSV((270.0 * (levelsNumber - i - 1)) / levelsNumber, 1.0, 1.0);

    for (UnsignedInteger i = 0; i < values_.getSize(); ++i)
    {
      const Point x(vertices[i]);
      Point v(values_[i]);
      Scalar arrowLength = v.norm();
      const UnsignedInteger paletteIndex = static_cast<UnsignedInteger>((levelsNumber - 0.5) * (arrowLength - normMin) / (normMax - normMin));
      const String color = palette[paletteIndex];
      v *= rho;
      arrowLength *= rho;
      // Draw the arrow head only if the arrow is large enough
      if (arrowLength > delta)
      {
        Sample data(6, 2);
        const Point u(v / arrowLength);
        data[0] = x;
        data[1] = x + v - u * delta;
        data(2, 0) = data(1, 0) + u[1] * (-0.5 * delta);
        data(2, 1) = data(1, 1) + u[0] * ( 0.5 * delta);
        data[3] = x + v;
        data(4, 0) = data(1, 0) + u[1] * ( 0.5 * delta);
        data(4, 1) = data(1, 1) + u[0] * (-0.5 * delta);
        data[5] = data[1];
        Curve curve(data);
        curve.setColor(color);
        graph.add(curve);
      } // arrowLength > delta
      else
      {
        Sample data(2, 2);
        data[0] = x;
        data[1] = x + v;
        Curve curve(data);
        curve.setColor(color);
        graph.add(curve);
      } // arrowLength <= delta
    } // for i
    return graph;
  }
  return drawMarginal(0, false);
}

Graph FieldImplementation::drawMarginal(const UnsignedInteger index,
                                        const Bool interpolate) const
{
  if (!(index < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error : indice should be between [0, " << getOutputDimension() - 1 << "]";
  const UnsignedInteger meshDimension = getInputDimension();
  if (!(meshDimension <= 2)) throw NotYetImplementedException(HERE) << "In FieldImplementation::drawMarginal(const UnsignedInteger index, const Bool interpolate) const: cannot draw a Field of mesh dimension greater than 2. Try the export to VTK for higher dimension.";
  const Sample marginalValues(values_.getMarginal(index));
  const String title(OSS() << getName() << " - " << index << " marginal" );
  Graph graph(title, description_[0], description_[index + 1], true, "topright");
  if (meshDimension == 1)
  {
    // Discretization of the x axis
    if (interpolate)
    {
      const Curve curveSerie(mesh_.getVertices(), marginalValues);
      graph.add(curveSerie);
    }
    else
    {
      Cloud cloudSerie(mesh_.getVertices(), marginalValues);
      cloudSerie.setPointStyle("bullet");
      graph.add(cloudSerie);
    }
  }
  else if (meshDimension == 2)
  {
    graph.setYTitle(description_[1]);
    const UnsignedInteger levelsNumber = ResourceMap::GetAsUnsignedInteger("Field-LevelNumber");
    if (interpolate)
    {
      // Compute the iso-values
      Point levels(levelsNumber);
      Description palette(levelsNumber);
      for (UnsignedInteger i = 0; i < levelsNumber; ++i)
      {
        const Scalar q = (i + 1.0) / (levelsNumber + 1.0);
        levels[i] = marginalValues.computeQuantile(q)[0];
        palette[i] = Curve::ConvertFromHSV((270.0 * (levelsNumber - i - 1)) / levelsNumber, 1.0, 1.0);
      }
      // Loop over the simplices to draw the segments (if any) associated with the different levels
      const UnsignedInteger simplicesNumber = mesh_.getSimplicesNumber();
      for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
      {
        const Indices currentSimplex(mesh_.getSimplex(i));
        UnsignedInteger i0 = currentSimplex[0];
        UnsignedInteger i1 = currentSimplex[1];
        UnsignedInteger i2 = currentSimplex[2];
        Scalar v0 = marginalValues(i0, 0);
        Scalar v1 = marginalValues(i1, 0);
        Scalar v2 = marginalValues(i2, 0);
        // Sort the vertices such that v0 <= v1 <= v2
        if (v0 > v1)
        {
          std::swap(v0, v1);
          std::swap(i0, i1);
        }
        if (v1 > v2)
        {
          std::swap(v1, v2);
          std::swap(i1, i2);
        }
        if (v0 > v1)
        {
          std::swap(v0, v1);
          std::swap(i0, i1);
        }
        // If the current simplex is constant, nothing to draw
        if (v0 == v2) continue;
        // For the current simplex, check all levels
        for (UnsignedInteger j = 0; j < levelsNumber; ++j)
        {
          const Scalar level = levels[j];
          if ((level >= v0) && (level <= v2))
          {
            const Point x0(mesh_.getVertex(i0));
            const Point x1(mesh_.getVertex(i1));
            const Point x2(mesh_.getVertex(i2));
            Sample data(2, 2);
            // The first point is on the [x0, x2] segment as v0 <= level <= v2 and v0 < v2
            data[0] = x0 + ((level - v0) / (v2 - v0)) * (x2 - x0);
            // if level <= v1, the second point is on the [x0, x1] segment
            if (level <= v1)
            {
              if (v1 == v0) data[1] = x1;
              else data[1] = x0 + ((level - v0) / (v1 - v0)) * (x1 - x0);
            }
            // if level >= v1, the second point is on the [x1, x2] segment
            else
            {
              if (v2 == v1) data[1] = x1;
              else data[1] = x2 + ((level - v2) / (v1 - v2)) * (x1 - x2);
            }
            graph.add(Curve(data, palette[j], "solid"));
          } // (level >= v0) && (level <= v2)
        } // j
      } // i
      // Simple colorbar
      const Scalar minValue = marginalValues.getMin()[0];
      const Scalar maxValue = marginalValues.getMax()[0];
      const Point xMin(mesh_.getVertices().getMin());
      for (SignedInteger i = levelsNumber - 1; i >= 0; --i)
      {
        Cloud point(Sample(1, xMin));
        point.setPointStyle("none");
        point.setColor(palette[i]);
        if ((i == static_cast<SignedInteger>(levelsNumber) - 1) || (i == 0)) point.setLegend(String(OSS() << 0.001 * round(1000.0 * (minValue + i * (maxValue - minValue) / (levelsNumber - 1)))));
        else point.setLegend(" ");
        graph.add(point);
      }
    } // interpolate
    else
    {
      const UnsignedInteger size = marginalValues.getSize();
      Description palette(size);
      for (UnsignedInteger i = 0; i < size; ++i) palette[i] = Curve::ConvertFromHSV((270.0 * (size - i - 1)) / size, 1.0, 1.0);
      const Scalar minValue = marginalValues.getMin()[0];
      const Scalar maxValue = marginalValues.getMax()[0];
      const UnsignedInteger simplicesNumber = mesh_.getSimplicesNumber();
      if (simplicesNumber > 0)
      {
        Sample data(0, 2);
        Description colors(0);
        for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
        {
          const Indices simplex(mesh_.getSimplex(i));
          data.add(mesh_.getVertex(simplex[0]));
          data.add(mesh_.getVertex(simplex[1]));
          data.add(mesh_.getVertex(simplex[2]));
          const Scalar meanValue = (marginalValues(simplex[0], 0) + marginalValues(simplex[1], 0) + marginalValues(simplex[2], 0)) / 3.0;
          const String color(palette[static_cast<UnsignedInteger>(round((size - 1) * (meanValue - minValue) / (maxValue - minValue)))]);
          colors.add(color);
        }
        graph.add(PolygonArray(data, 3, colors));
        graph.setGrid(false);
      } // Simplices
      else
      {
        for (UnsignedInteger i = 0; i < size; ++i)
        {
          Cloud point(Sample(1, mesh_.getVertex(i)));
          const String color(palette[static_cast<UnsignedInteger>(round((size - 1) * (marginalValues(i, 0) - minValue) / (maxValue - minValue)))]);
          point.setColor(color);
          point.setPointStyle("bullet");
          graph.add(point);
        }
      } // No simplex
      // Simple colorbar
      const Point xMin(mesh_.getVertices().getMin());
      for (SignedInteger i = levelsNumber - 1; i >= 0; --i)
      {
        Cloud point(Sample(1, xMin));
        point.setPointStyle("none");
        point.setColor(palette[(i * (size - 1)) / (levelsNumber - 1)]);
        if ((i == static_cast<SignedInteger>(levelsNumber) - 1) || (i == 0)) point.setLegend(String(OSS() << 0.001 * round(1000.0 * (minValue + i * (maxValue - minValue) / (levelsNumber - 1)))));
        else point.setLegend(" ");
        graph.add(point);
      }
    } // !interpolate
  } // meshDimension == 2
  return graph;
}


/* Method save() stores the object through the StorageManager */
void FieldImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "mesh_", mesh_);
  adv.saveAttribute( "values_", values_);
  adv.saveAttribute( "description_", description_);
  adv.saveAttribute( "inputMean_", inputMean_);
  adv.saveAttribute( "isAlreadyComputedInputMean_", isAlreadyComputedInputMean_);
}


/* Method load() reloads the object from the StorageManager */
void FieldImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "mesh_", mesh_);
  adv.loadAttribute( "values_", values_);
  adv.loadAttribute( "description_", description_);
  adv.loadAttribute( "inputMean_", inputMean_);
  adv.loadAttribute( "isAlreadyComputedInputMean_", isAlreadyComputedInputMean_);
}

/* Export to VTK file */
void FieldImplementation::exportToVTKFile(const String & fileName) const
{
  std::ofstream file(fileName.c_str(), std::ios::out);
  if (!file) throw FileNotFoundException(HERE) << "Error: can't open file " << fileName;
  const String content(mesh_.streamToVTKFormat());
  const UnsignedInteger oldPrecision = PlatformInfo::GetNumericalPrecision();
  PlatformInfo::SetNumericalPrecision(16);
  file << content << "\nPOINT_DATA " << getSize() << "\n";

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    String fieldName(getDescription()[getInputDimension() + i]);
    replace(fieldName.begin(), fieldName.end(), ' ', '~');
    if (fieldName.size() == 0) fieldName = String(OSS() << "v_" << i);
    file << "SCALARS " << fieldName << " float\nLOOKUP_TABLE default\n";
    for (UnsignedInteger j = 0; j < getSize(); ++j) file << values_(j, i) << "\n";
  }
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  file.close();
}

END_NAMESPACE_OPENTURNS
