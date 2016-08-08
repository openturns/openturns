//                                               -*- C++ -*-
/**
 *  @brief The class FieldImplementation implements values indexed by
 *  the vertices of a Mesh
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<NumericalSample>);

static const Factory<PersistentCollection<NumericalSample> > Factory_PersistentCollection_NumericalSample;

CLASSNAMEINIT(FieldImplementation);

static const Factory<FieldImplementation> Factory_FieldImplementation;

/* Default constructor is private */
FieldImplementation::FieldImplementation()
  : PersistentObject()
  , mesh_()
  , values_(mesh_.getVerticesNumber(), 0)
  , description_(mesh_.getDescription())
  , spatialMean_(0)
  , isAlreadyComputedSpatialMean_(false)
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
  , spatialMean_(dim)
  , isAlreadyComputedSpatialMean_(false)
{
  // Build the default description
  Description description(mesh_.getVertices().getDescription());
  description.add(values_.getDescription());
  setDescription(description);
}

/* Constructor from a Mesh and a sample */
FieldImplementation::FieldImplementation(const Mesh & mesh,
    const NumericalSample & values)
  : PersistentObject()
  , mesh_(mesh)
  , values_(values)
  , description_()
  , spatialMean_(values.getDimension())
  , isAlreadyComputedSpatialMean_(false)
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
UnsignedInteger FieldImplementation::getSpatialDimension() const
{
  return mesh_.getDimension();
}

UnsignedInteger FieldImplementation::getDimension() const
{
  return values_.getDimension();
}

/* Mesh accessor */
const Mesh & FieldImplementation::getMesh() const
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
  isAlreadyComputedSpatialMean_ = false;
  return values_[index];
}

NSI_const_point FieldImplementation::operator[](const UnsignedInteger index) const
{
  return values_[index];
}

NumericalScalar & FieldImplementation::operator () (const UnsignedInteger i,
    const UnsignedInteger j)
{
  isAlreadyComputedSpatialMean_ = false;
#ifdef DEBUG_BOUNDCHECKING
  // No copyOnWrite() as the at() method already do it
  return at(i, j);
#else
  return (*this)[i][j];
#endif /* DEBUG_BOUNDCHECKING */
}

const NumericalScalar & FieldImplementation::operator () (const UnsignedInteger i,
    const UnsignedInteger j) const
{
#ifdef DEBUG_BOUNDCHECKING
  return at(i, j);
#else
  return (*this)[i][j];
#endif /* DEBUG_BOUNDCHECKING */
}


NSI_point FieldImplementation::at (const UnsignedInteger index)
{
  if (index >= getSize()) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  isAlreadyComputedSpatialMean_ = false;
  return (*this)[index];
}

NSI_const_point FieldImplementation::at (const UnsignedInteger index) const
{
  if (index >= getSize()) throw OutOfBoundException(HERE) << "Index (" << index << ") is not less than size (" << getSize() << ")";
  return (*this)[index];
}

NumericalScalar & FieldImplementation::at (const UnsignedInteger i,
    const UnsignedInteger j)
{
  if (i >= getSize()) throw OutOfBoundException(HERE) << "i (" << i << ") is not less than size (" << getSize() << ")";
  if (j >= getDimension()) throw OutOfBoundException(HERE) << "j (" << j << ") is not less than dimension (" << getDimension() << ")";
  isAlreadyComputedSpatialMean_ = false;
  return (*this)[i][j];
}

const NumericalScalar & FieldImplementation::at (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  if (i >= getSize()) throw OutOfBoundException(HERE) << "i (" << i << ") is not less than size (" << getSize() << ")";
  if (j >= getDimension()) throw OutOfBoundException(HERE) << "j (" << j << ") is not less than dimension (" << getDimension() << ")";
  return (*this)[i][j];
}

/* Data accessors */
NumericalPoint FieldImplementation::getValueAtIndex(const UnsignedInteger index) const
{
  return values_[index];
}

void FieldImplementation::setValueAtIndex(const UnsignedInteger index,
    const NumericalPoint & val)
{
  isAlreadyComputedSpatialMean_ = false;
  values_[index] = val;
}

NumericalPoint FieldImplementation::getValueAtNearestPosition(const NumericalPoint & position) const
{
  return values_[mesh_.getNearestVertexIndex(position)];
}

void FieldImplementation::setValueAtNearestPosition(const NumericalPoint & position,
    const NumericalPoint & val)
{
  isAlreadyComputedSpatialMean_ = false;
  values_[mesh_.getNearestVertexIndex(position)] = val;
}


NumericalPoint FieldImplementation::getValueAtNearestTime(const NumericalScalar timestamp) const
{
  return getValueAtNearestPosition(NumericalPoint(1, timestamp));
}

void FieldImplementation::setValueAtNearestTime(const NumericalScalar timestamp, const NumericalPoint & val)
{
  isAlreadyComputedSpatialMean_ = false;
  setValueAtNearestPosition(NumericalPoint(1, timestamp), val);
}

/* Description Accessor */
void FieldImplementation::setDescription(const Description & description)
{
  if (description.getSize() != (getSpatialDimension() + getDimension())) throw InvalidArgumentException(HERE) << "Error: the given description does not match the field input+output dimension.";
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
  NumericalSample data(mesh_.getVertices());
  data.stack(values_);
  return data.__str__(offset);
}

/* TBB functor to speed-up spatial mean computation */
struct FieldSpatialMeanFunctor
{
  const FieldImplementation & field_;
  NumericalScalar volumeAccumulator_;
  NumericalPoint accumulator_;

  FieldSpatialMeanFunctor(const FieldImplementation & field)
    : field_(field), volumeAccumulator_(0.0), accumulator_(field.getDimension(), 0.0) {}

  FieldSpatialMeanFunctor(const FieldSpatialMeanFunctor & other, TBB::Split)
    : field_(other.field_), volumeAccumulator_(0.0), accumulator_(other.field_.getDimension(), 0.0) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r)
  {
    const UnsignedInteger meshDimension = field_.getSpatialDimension();
    const UnsignedInteger dimension = field_.getDimension();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const NumericalScalar volume = field_.mesh_.computeSimplexVolume(i);
      const Indices simplex(field_.mesh_.getSimplex(i));
      NumericalPoint meanValue(dimension, 0.0);
      for (UnsignedInteger j = 0; j <= meshDimension; ++j) meanValue += field_.values_[simplex[j]];
      volumeAccumulator_ += volume;
      accumulator_ += (volume / dimension) * meanValue;
    }
  }

  void join(const FieldSpatialMeanFunctor & other)
  {
    volumeAccumulator_ += other.volumeAccumulator_;
    accumulator_ += other.accumulator_;
  }

}; /* end struct FieldSpatialMeanFunctor */

/* Compute the spatial mean of the field */
void FieldImplementation::computeSpatialMean() const
{
  FieldSpatialMeanFunctor functor( *this );
  TBB::ParallelReduce( 0, mesh_.getSimplicesNumber(), functor );
  if (functor.volumeAccumulator_ == 0.0) throw InternalException(HERE) << "Error: cannot compute the spatial mean of a field supported by a mesh of zero volume.";
  spatialMean_ = functor.accumulator_ / functor.volumeAccumulator_;
  isAlreadyComputedSpatialMean_ = true;
}


/* Compute the spatial mean of the field */
NumericalPoint FieldImplementation::getSpatialMean() const
{
  if (!isAlreadyComputedSpatialMean_) computeSpatialMean();
  return spatialMean_;
}

/* Compute the spatial mean of the field */
NumericalPoint FieldImplementation::getTemporalMean() const
{
  if (!mesh_.isRegular() || (mesh_.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the temporal mean is defined only when the mesh is regular and of dimension 1.";
  return values_.computeMean();
}

/* Return the values stored in the field as a sample */
NumericalSample FieldImplementation::getSample() const
{
  return values_;
}

NumericalSample FieldImplementation::getValues() const
{
  return values_;
}

void FieldImplementation::setValues(const NumericalSample & values)
{
  if (values.getSize() != mesh_.getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: expected a sample of size=" << mesh_.getVerticesNumber() << ", got size=" << values.getSize();
  values_ = values;
}

/* Return the field as a sample, ie its values and positions */
NumericalSample FieldImplementation::asSample() const
{
  NumericalSample data(mesh_.getVertices());
  data.stack(values_);
  return data;
}

/* Return the field as a defomed mesh, ie its values are added to the components of the vertices if the dimensions match */
Mesh FieldImplementation::asDeformedMesh() const
{
  if (getDimension() != getSpatialDimension()) throw InternalException(HERE) << "Error: cannot deform the mesh if the dimension of the values=" << values_.getDimension() << " does not match the mesh dimension=" << getSpatialDimension();
  NumericalSample data(mesh_.getVertices());
  data += values_;
  return Mesh(data, mesh_.getSimplices());
}

/* Draw a marginal of the Field */
Graph FieldImplementation::draw() const
{
  // Specific drawing method for bidimensional fields indexed by a scalar
  if ((getSpatialDimension() == 1) && (getDimension() == 2))
    {
      const String title(OSS() << getName());
      Graph graph(title, description_[0], description_[1], true, "");
      const Curve curveSerie(getValues());
      graph.add(curveSerie);
      return graph;
    }
  return drawMarginal(0, false);
}

Graph FieldImplementation::drawMarginal(const UnsignedInteger index,
                                        const Bool interpolate) const
{
  if (index >= getDimension() ) throw InvalidArgumentException(HERE) << "Error : indice should be between [0, " << getDimension() - 1 << "]";
  const UnsignedInteger meshDimension = getSpatialDimension();
  if (meshDimension > 2) throw NotYetImplementedException(HERE) << "In FieldImplementation::drawMarginal(const UnsignedInteger index, const Bool interpolate) const: cannot draw a Field of mesh dimension greater than 2. Try the export to VTK for higher dimension.";
  const NumericalSample marginalValues(values_.getMarginal(index));
  const String title(OSS() << getName() << " - " << index << " marginal" );
  Graph graph(title, description_[0], "Values", true, "topright");
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
      NumericalPoint levels(levelsNumber);
      Description palette(levelsNumber);
      for (UnsignedInteger i = 0; i < levelsNumber; ++i)
      {
        const NumericalScalar q = (i + 1.0) / (levelsNumber + 1.0);
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
        NumericalScalar v0 = marginalValues[i0][0];
        NumericalScalar v1 = marginalValues[i1][0];
        NumericalScalar v2 = marginalValues[i2][0];
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
        // If the current simplex is constant, nothing to draw
        if (v0 == v2) continue;
        // For the current simplex, check all levels
        for (UnsignedInteger j = 0; j < levelsNumber; ++j)
        {
          const NumericalScalar level = levels[j];
          if ((level >= v0) && (level <= v2))
          {
            const NumericalPoint x0(mesh_.getVertex(i0));
            const NumericalPoint x1(mesh_.getVertex(i1));
            const NumericalPoint x2(mesh_.getVertex(i2));
            NumericalSample data(2, 2);
            // The first point is on the [x0, x2] segment as v0 <= level <= v2 and v0 < v2
            data[0] = x0 + ((level - v0) / (v2 - v0)) * (x2 - x0);
            // if level < v1, the second point is on the [x0, x1] segment
            if (level < v1) data[1] = x0 + ((level - v0) / (v1 - v0)) * (x1 - x0);
            else if (v1 == v2) data[1] = 0.5 * (x1 + x2);
            else data[1] = x2 + ((level - v2) / (v1 - v2)) * (x1 - x2);
            graph.add(Curve(data, palette[j], "solid"));
          } // (level >= v0) && (level <= v2)
        } // j
      } // i
      // Simple colorbar
      const NumericalScalar minValue = marginalValues.getMin()[0];
      const NumericalScalar maxValue = marginalValues.getMax()[0];
      const NumericalPoint xMin(mesh_.getVertices().getMin());
      for (SignedInteger i = levelsNumber - 1; i >= 0; --i)
      {
        Cloud point(NumericalSample(1, xMin));
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
      const NumericalScalar minValue = marginalValues.getMin()[0];
      const NumericalScalar maxValue = marginalValues.getMax()[0];
      const UnsignedInteger simplicesNumber = mesh_.getSimplicesNumber();
      if (simplicesNumber > 0)
      {
        NumericalSample data(0, 2);
        Description colors(0);
        for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
        {
          const Indices simplex(mesh_.getSimplex(i));
          data.add(mesh_.getVertex(simplex[0]));
          data.add(mesh_.getVertex(simplex[1]));
          data.add(mesh_.getVertex(simplex[2]));
          const NumericalScalar meanValue = (marginalValues[simplex[0]][0] + marginalValues[simplex[1]][0] + marginalValues[simplex[2]][0]) / 3.0;
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
          Cloud point(NumericalSample(1, mesh_.getVertex(i)));
          const String color(palette[static_cast<UnsignedInteger>(round((size - 1) * (marginalValues[i][0] - minValue) / (maxValue - minValue)))]);
          point.setColor(color);
          point.setPointStyle("bullet");
          graph.add(point);
        }
      } // No simplex
      // Simple colorbar
      const NumericalPoint xMin(mesh_.getVertices().getMin());
      for (SignedInteger i = levelsNumber - 1; i >= 0; --i)
      {
        Cloud point(NumericalSample(1, xMin));
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
  adv.saveAttribute( "spatialMean_", spatialMean_);
  adv.saveAttribute( "isAlreadyComputedSpatialMean_", isAlreadyComputedSpatialMean_);
}


/* Method load() reloads the object from the StorageManager */
void FieldImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "mesh_", mesh_);
  adv.loadAttribute( "values_", values_);
  adv.loadAttribute( "description_", description_);
  adv.loadAttribute( "spatialMean_", spatialMean_);
  adv.loadAttribute( "isAlreadyComputedSpatialMean_", isAlreadyComputedSpatialMean_);
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

  for (UnsignedInteger i = 0; i < getDimension(); ++i)
  {
    String fieldName(getDescription()[getSpatialDimension() + i]);
    replace(fieldName.begin(), fieldName.end(), ' ', '~');
    if (fieldName.size() == 0) fieldName = String(OSS() << "v_" << i);
    file << "SCALARS " << fieldName << " float\nLOOKUP_TABLE default\n";
    for (UnsignedInteger j = 0; j < getSize(); ++j) file << values_[j][i] << "\n";
  }
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  file.close();
}

END_NAMESPACE_OPENTURNS
