//                                               -*- C++ -*-
/*
 * @brief Graph implements graphic devices for plotting through R
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>
#include <fstream>

#include "openturns/GraphImplementation.hxx"
#include "openturns/Rfunctions.hxx"
#include "openturns/Path.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Os.hxx"
#include "openturns/Pie.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<Drawable>)

static const Factory<PersistentCollection<Drawable> > Factory_PersistentCollection_Drawable;

CLASSNAMEINIT(GraphImplementation)

static const Factory<GraphImplementation> Factory_GraphImplementation;

Bool GraphImplementation::IsFirstInitialization = true;


Description GraphImplementation::ValidLegendPositions;

/* Initialize valid legend positions */
void GraphImplementation::InitializeValidLegendPositions()
{
  ValidLegendPositions.setName("ValidLegendPositions");
  ValidLegendPositions.add("");
  ValidLegendPositions.add("bottomright");
  ValidLegendPositions.add("bottom");
  ValidLegendPositions.add("bottomleft");
  ValidLegendPositions.add("left");
  ValidLegendPositions.add("topleft");
  ValidLegendPositions.add("top");
  ValidLegendPositions.add("topright");
  ValidLegendPositions.add("right");
  ValidLegendPositions.add("center");
}

/* Default constructor */
GraphImplementation::GraphImplementation(const String & title)
  : PersistentObject()
  , title_(title)
  , legendPosition_()
  , legendFontSize_(ResourceMap::GetAsScalar("Graph-DefaultLegendFontSize"))
  , xTitle_()
  , yTitle_()
  , showAxes_(false)
  , logScale_(NONE)
  , showGrid_(false)
  , gridColor_("gray")
  , xMargin_(ResourceMap::GetAsScalar("Graph-DefaultHorizontalMargin"))
  , yMargin_(ResourceMap::GetAsScalar("Graph-DefaultVerticalMargin"))
  , automaticBoundingBox_(true)
  , boundingBox_(4)
  , drawablesCollection_(0)
{
  if(IsFirstInitialization)
  {
    InitializeValidLegendPositions();
    IsFirstInitialization = false;
  }
}

/* Constructor with parameters */
GraphImplementation::GraphImplementation(const String & title,
    const String & xTitle,
    const String & yTitle,
    const Bool showAxes,
    const String & legendPosition,
    const Scalar legendFontSize,
    const LogScale logScale)
  : PersistentObject()
  , title_(title)
  , legendFontSize_(legendFontSize)
  , xTitle_(xTitle)
  , yTitle_(yTitle)
  , showAxes_(showAxes)
  , logScale_(NONE)
  , showGrid_(true)
  , gridColor_("gray")
  , xMargin_(ResourceMap::GetAsScalar("Graph-DefaultHorizontalMargin"))
  , yMargin_(ResourceMap::GetAsScalar("Graph-DefaultVerticalMargin"))
  , automaticBoundingBox_(true)
  , boundingBox_(4)
  , drawablesCollection_(0)
{
  setName(title);
  if(IsFirstInitialization)
  {
    InitializeValidLegendPositions();
    IsFirstInitialization = false;
  }
  // Check if the given legend position is valid

  if (!IsValidLegendPosition(legendPosition)) throw InvalidArgumentException(HERE) << "The given legend position = " << legendPosition << " is invalid";
  if (!(legendFontSize > 0.0)) throw InvalidArgumentException(HERE) << "The given legend font size = " << legendFontSize << " is invalid, it must be > 0";
  legendPosition_ = legendPosition;
  setLogScale(logScale);
}

/* Virtual constructor */
GraphImplementation * GraphImplementation::clone() const
{
  return new GraphImplementation(*this);
}

/* String converter */
String GraphImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GraphImplementation::GetClassName()
      << " name=" << getName()
      << " title=" << title_
      << " xTitle=" << xTitle_
      << " yTitle=" << yTitle_
      << " axes=" << (showAxes_ ? "ON" : "OFF")
      << " grid=" << (showGrid_ ? "ON" : "OFF")
      << " legendposition=" << legendPosition_
      << " legendFontSize=" << legendFontSize_
      << " drawables=" << drawablesCollection_;
  return oss;
}

/* Adds a drawable instance to the collection of drawables contained in GraphImplementation */
void GraphImplementation::add(const Drawable & aDrawable)
{
  drawablesCollection_.add(aDrawable);
}

/* Erase a drawable instance from the collection of drawables contained in GraphImplementation */
void GraphImplementation::erase(const UnsignedInteger i)
{
  drawablesCollection_.erase(drawablesCollection_.begin() + i);
}

/* Adds a collection of drawable instances to the collection of drawables contained in GraphImplementation */
void GraphImplementation::add(const DrawableCollection & drawableCollection)
{
  for (UnsignedInteger i = 0; i < drawableCollection.getSize(); ++i) drawablesCollection_.add(drawableCollection[i]);
}

/* Adds a collection of drawable instances to the collection of drawables contained in GraphImplementation */
void GraphImplementation::add(const GraphImplementation & graphImplementation)
{
  // Add the drawables
  add(graphImplementation.getDrawables());
}

/* Drawables accessor */
GraphImplementation::DrawableCollection GraphImplementation::getDrawables() const
{
  return drawablesCollection_;
}

void GraphImplementation::setDrawables(const DrawableCollection & drawableCollection)
{
  drawablesCollection_ = drawableCollection;
}

/* Individual drawable accessor */
Drawable GraphImplementation::getDrawable(const UnsignedInteger index) const
{
  if (index >= drawablesCollection_.getSize()) throw InvalidRangeException(HERE) << "Error: trying to get a drawable at position " << index << " from a collection of size " << drawablesCollection_.getSize();
  return drawablesCollection_[index];
}

void GraphImplementation::setDrawable(const Drawable & drawable,
                                      const UnsignedInteger index)
{
  if (index >= drawablesCollection_.getSize()) throw InvalidRangeException(HERE) << "Error: trying to set a drawable at position " << index << " into a collection of size " << drawablesCollection_.getSize();
  drawablesCollection_[index] = drawable;
}

/** Global color accessor */
Description GraphImplementation::getColors() const
{
  const UnsignedInteger size = drawablesCollection_.getSize();
  Description colors(size);
  for (UnsignedInteger i = 0; i < size; ++i) colors[i] = drawablesCollection_[i].getColor();
  return colors;
}

void GraphImplementation::setColors(const Description & colors)
{
  const UnsignedInteger size = drawablesCollection_.getSize();
  const UnsignedInteger inputSize = colors.getSize();
  if (inputSize > 0)
    for (UnsignedInteger i = 0; i < size; ++i) drawablesCollection_[i].setColor(colors[i % inputSize]);
}

void GraphImplementation::setDefaultColors()
{
  const UnsignedInteger size = drawablesCollection_.getSize();
  setColors(Drawable::BuildDefaultPalette(size));
}

/** Global legend accessor */
Description GraphImplementation::getLegends() const
{
  const UnsignedInteger size = drawablesCollection_.getSize();
  Description legends(size);
  for (UnsignedInteger i = 0; i < size; ++i) legends[i] = drawablesCollection_[i].getLegend();
  return legends;
}

void GraphImplementation::setLegends(const Description & legends)
{
  const UnsignedInteger size = drawablesCollection_.getSize();
  const UnsignedInteger inputSize = legends.getSize();
  if (inputSize > 0)
    for (UnsignedInteger i = 0; i < size; ++i) drawablesCollection_[i].setLegend(legends[i % inputSize]);
}


/* Hide or show x and y axes */
void GraphImplementation::setAxes(const Bool showAxes)
{
  showAxes_ = showAxes;
}

/* Accessor for showAxes_ */
Bool GraphImplementation::getAxes() const
{
  return showAxes_;
}

/* Ticks location flag accessor */
void GraphImplementation::setTickLocation(const TickLocation tickLocation)
{
  tickLocation_ = tickLocation;
}

GraphImplementation::TickLocation GraphImplementation::getTickLocation() const
{
  return tickLocation_;
}

/* Set log scale for x, y both or none axes */
void GraphImplementation::setLogScale(const LogScale logScale)
{
  if (logScale > LOGXY) logScale_ = NONE;
  else logScale_ = logScale;
}

/* Accessor for logScale_ */
GraphImplementation::LogScale GraphImplementation::getLogScale() const
{
  return logScale_;
}

/* Hide or show x grid */
void GraphImplementation::setGrid(const Bool showGrid)
{
  showGrid_ = showGrid;
}

/* Accessor for showGrid_ */
Bool GraphImplementation::getGrid() const
{
  return showGrid_;
}

/* Grid color accessors */
void GraphImplementation::setGridColor(const String & color)
{
  if(!DrawableImplementation::IsValidColor(color)) throw InvalidArgumentException(HERE) << "Given color = " << color << " is incorrect";

  gridColor_ = color;
}

/* Accessor for showGrid_ */
String GraphImplementation::getGridColor() const
{
  return gridColor_;
}

/* Accesor for xTitle */
String GraphImplementation::getXTitle() const
{
  return xTitle_;
}

/* Accessor for xTitle */
void GraphImplementation::setXTitle(const String & title)
{
  xTitle_ = title;
}

/* Accessor for yTitle */
String GraphImplementation::getYTitle() const
{
  return yTitle_;
}

/* Accessor for yTitle */
void GraphImplementation::setYTitle(const String & title)
{
  yTitle_ = title;
}

/* Accesor for title */
String GraphImplementation::getTitle() const
{
  return title_;
}

/* Accesor for title */
void GraphImplementation::setTitle(const String & title)
{
  title_ = title;
}

/* Build the R command corresponding to the legend */
String GraphImplementation::makeRLegendCommand() const
{
  OSS labels, colors, lines, points, fill;
  labels << "c(";
  colors << "c(";
  lines << "c(";
  points << "c(";
  fill << "c(";

  Bool drawLegend = false;

  for(DrawableCollection::const_iterator it = drawablesCollection_.begin(); it != drawablesCollection_.end(); ++it)
  {
    if (it->getLegend().size() != 0)
    {
      drawLegend = true;
      labels << "\"" << it->getLegend() << "\",";

      if(it->getColor().size() == 0)
        colors << "NA,";
      else colors << "\"" << it->getColor() << "\",";

      if(it->getFillStyle().size() == 0)
        fill << "NA,";
      else fill << "\"" << it->getFillStyle() << "\",";

      if(it->getPointStyle().size() == 0 || it->getFillStyle().size() != 0) //cannot merge fill and point symbol
        points << "NA,";
      else points << it->getPointCode(it->getPointStyle()) << ",";

      if(it->getLineStyle().size() == 0 || it->getFillStyle().size() != 0 ) //cannot merge line and fill symbol
        lines << "NA,";
      else lines << "\"" << it->getLineStyle() << "\",";
    }
  }
  if (drawLegend)
  {
    String labels_str(labels);
    labels_str.replace(labels_str.length() - 1, 1, ")");

    String colors_str(colors);
    colors_str.replace(colors_str.length() - 1, 1, ")");

    String lines_str(lines);
    lines_str.replace(lines_str.length() - 1, 1, ")");

    String points_str(points);
    points_str.replace(points_str.length() - 1, 1, ")");

    String fill_str(fill);
    fill_str.replace(fill_str.length() - 1, 1, ")");

    OSS rCommand;
    rCommand << "legend(\"" << legendPosition_ << "\","
             << "legend=" << labels_str << ","
             << "col=" << colors_str << ","
             << "lty=" << lines_str << ","
             << "pch=" << points_str << ","
             << "fill=" << fill_str << ","
             << "cex=" << legendFontSize_ << ","
             << "bg=\"grey90\",merge=TRUE,density=40)";

    return rCommand;
  }

  else return String();
}

/* Get the R command corresponding to the graph */
String GraphImplementation::getRCommand() const
{
  return OSS() << makeRHeaderCommand() << makeRCoreCommand();
}

/* Make R header commande */
String GraphImplementation::makeRHeaderCommand() const
{
  OSS oss;
  // Check if we want to print a legend and if there is a legend to print before to include the corresponding R code
  // First, do we want a legend?
  if (legendPosition_ != "")
  {
    // Secodn, is there a legend to print?
    for (UnsignedInteger i = 0; i < drawablesCollection_.getSize(); ++i)
    {
      if (drawablesCollection_[i].getLegend().size() > 0)
      {
        oss << R_LEGEND << "\n";
        break;
      }
    }
  }
  // Check if we want to draw a Pie before to include the corresponding R code
  for (UnsignedInteger i = 0; i < drawablesCollection_.getSize(); ++i)
  {
    if (drawablesCollection_[i].getImplementation()->getClassName() == Pie::GetClassName())
    {
      oss << "\n" << R_PIE << "\n";
      break;
    }
  }
  oss << "options(digits=17)" << "\n" << "options(warn=-1)" << "\n";
  return oss;
}

/* Make R core command */
String GraphImplementation::makeRCoreCommand() const
{
  // get the general bounding box
  const Interval boundingBox(getBoundingBox());
  const Point lowerBound(boundingBox.getLowerBound());
  const Point upperBound(boundingBox.getUpperBound());

  //load the R code attached to the general plot
  OSS graphCommand;
  graphCommand << "plot(c(" << lowerBound[0] << "," << upperBound[0] << "),"
               << "c(" << lowerBound[1] << "," << upperBound[1] << "),"
               << "type=\"n\",main=\"" << title_ << "\",";
  if (showAxes_)
  {
    graphCommand << "xlab=\"" << xTitle_ << "\",ylab=\"" << yTitle_ << "\","
                 << "axes=TRUE";
  }
  else
  {
    graphCommand << "xlab=\"\",ylab=\"\",axes=FALSE";
  }
  if (showGrid_)
  {
    graphCommand << ", panel.first=grid(col=\"" + gridColor_ + "\")";
  }
  switch (logScale_)
  {
    case NONE:
      break;
    case LOGX:
      graphCommand << ", log=\"x\"";
      break;
    case LOGY:
      graphCommand << ", log=\"y\"";
      break;
    case LOGXY:
      graphCommand << ", log=\"xy\"";
      break;
  }
  graphCommand << ", cex.main=2, cex.axis=1.5, cex.lab=1.5)\n";

  // add the R code attached to each drawable
  UnsignedInteger drawablesSize = drawablesCollection_.getSize();
  for(UnsignedInteger i = 0; i < drawablesSize; ++i)
  {
    if (drawablesCollection_[i].getData().getSize() != 0)
      graphCommand << drawablesCollection_[i].draw() << "\n";
  }
  // make the legend command
  graphCommand << (legendPosition_.size() == 0 ? String() : makeRLegendCommand());
  return graphCommand;
}


std::map<SignedInteger, String> GraphImplementation::GetExtensionMap()
{
  std::map<SignedInteger, String> m;
  m[PNG] = ".png";
  m[EPS] = ".eps";
  m[FIG] = ".fig";
  m[PDF] = ".pdf";
  return m;
}

/* The method that generates the graphic files */
void GraphImplementation::draw(const String & file,
                               const Scalar width,
                               const Scalar height,
                               SignedInteger drawingFormat)
{
  // Override format base on extension
  size_t pos = file.find_last_of(".");
  Bool matchedExtension = false;
  static std::map<SignedInteger, String> extensionMap = GetExtensionMap();
  if (pos != String::npos)
  {
    String extension = file.substr(pos, file.length());
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    for(std::map<SignedInteger, String>::iterator iter = extensionMap.begin(); iter != extensionMap.end(); ++iter)
    {
      if (!extension.compare(iter->second))
      {
        drawingFormat = iter->first;
        matchedExtension = true;
        break;
      }
    }
  }

  // Check the needed drawingFormat. If it is invalid, set it to ALL
  if (!(drawingFormat & (PNG | EPS | FIG | PDF)))
  {
    LOGINFO(OSS() << "Warning: invalid drawing format: " << drawingFormat << ", selecting all formats.");
    drawingFormat = ALL;
  }

  OSS rCommand;
  rCommand << makeRHeaderCommand();
  String rCoreCommand(makeRCoreCommand());
  if (drawingFormat & EPS)
  {
    rCommand << "postscript(\"" << file + (matchedExtension ? "" : extensionMap[EPS]) << "\", horizontal = FALSE, onefile = FALSE, paper = \"special\", height=" << height / 72. << ", width=" << width / 72. << ")" << "\n" << rCoreCommand << "\n" << "dev.off()" << "\n";
  }
  if (drawingFormat & PDF)
  {
    rCommand << "pdf(\"" << file + (matchedExtension ? "" : extensionMap[PDF]) << "\", onefile = FALSE, paper = \"special\", height=" << height / 72. << ", width=" << width / 72. << ")" << "\n" << rCoreCommand << "\n" << "dev.off()" << "\n";
  }
  if (drawingFormat & PNG)
  {
    rCommand << "png(\"" << file + (matchedExtension ? "" : extensionMap[PNG]) << "\",height=" << height << ", width=" << width << ",res=72)" << "\n" << rCoreCommand << "\n" << "dev.off()" << "\n";
  }
  if (drawingFormat & FIG)
  {
    rCommand << "xfig(\"" << file + (matchedExtension ? "" : extensionMap[FIG]) << "\", horizontal = FALSE, onefile = FALSE, paper = \"A4\", height=" << height / 72. << ", width=" << width / 72. << ")" << "\n" << rCoreCommand << "\n" << "dev.off()" << "\n";
  }
  LOGDEBUG(OSS() << "R command=" << String(rCommand));

  // system commands to write R code in a temporary file
  /* using mkstemp non standard C for temporary file generation */
  String temporaryFileName(Path::BuildTemporaryFileName("tmp_graph.R.XXXXXX"));
  std::ofstream cmdFile(temporaryFileName.c_str(), std::ios::out);
  cmdFile << String(rCommand);
  cmdFile.close();

  //execute R and load R script in temporary file
  const String RExecutable(ResourceMap::GetAsString("R-executable-command"));
  OSS systemCommand;
  if (RExecutable != "") systemCommand << "" << RExecutable << "" << " --no-save --silent < \"" << temporaryFileName << "\"" << Os::GetDeleteCommandOutput();
  else throw NotYetImplementedException(HERE) << "In GraphImplementation::draw(): needs R. Please install it and set the absolute path of the R executable in ResourceMap.";
  if (Os::ExecuteCommand(systemCommand) != 0) throw InternalException(HERE) << "GraphImplementation: error trying to execute R command=" << String(systemCommand);
  Os::Remove(temporaryFileName);
  clean();
}

/* Clean temporary files */
void GraphImplementation::clean()
{
  UnsignedInteger drawableNumber = drawablesCollection_.getSize();
  // Clean all the temporary data created by the drawables during their drawing
  for (UnsignedInteger i = 0; i < drawableNumber; ++i)
  {
    if (drawablesCollection_[i].getData().getSize() != 0)
      drawablesCollection_[i].clean();
  }
}

/* Margin accessor */
void GraphImplementation::setXMargin(const Scalar xMargin)
{
  if (!(xMargin >= 0.0) || !(xMargin <= 1.0))
    throw InvalidArgumentException(HERE) << "Horizontal margin must be in [0, 1].";
  xMargin_ = xMargin;
}

void GraphImplementation::setYMargin(const Scalar yMargin)
{
  if (!(yMargin >= 0.0) || !(yMargin <= 1.0))
    throw InvalidArgumentException(HERE) << "Vertical margin must be in [0, 1].";
  yMargin_ = yMargin;
}

/* Get the bounding box of the whole plot */
Interval GraphImplementation::getBoundingBox() const
{
  if (automaticBoundingBox_) computeBoundingBox();
  return boundingBox_;
}

/* Set the bounding box of the whole plot */
void GraphImplementation::setBoundingBox(const Interval & boundingBox)
{
  if (boundingBox.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: the given bounding box must have a dimension equal to 2";
  boundingBox_ = boundingBox;
  automaticBoundingBox_ = false;
}

/* Automatic bounding box accessor */
Bool GraphImplementation::getAutomaticBoundingBox() const
{
  return automaticBoundingBox_;
}

void GraphImplementation::setAutomaticBoundingBox(const Bool automaticBoundingBox)
{
  automaticBoundingBox_ = automaticBoundingBox;
}

/* Compute the best bounding box to enclose all the drawables */
void GraphImplementation::computeBoundingBox() const
{
  const UnsignedInteger size = drawablesCollection_.getSize();

  // First exceptional case: no drawable, we default to default bounding box
  if (size == 0)
  {
    LOGINFO("Warning: cannot compute the bounding box of a graph with no drawable, switch to [0,1]x[0,1] default bounding box");
    boundingBox_ = Interval(2);
    return;
  }

  // compute the enclosing bounding box
  Sample minBoxes(0, 2);
  Sample maxBoxes(0, 2);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    if (drawablesCollection_[i].getData().getSize() != 0)
    {
      const Interval boundingBox(drawablesCollection_[i].getBoundingBox());
      minBoxes.add(boundingBox.getLowerBound());
      maxBoxes.add(boundingBox.getUpperBound());
    }
  }
  Point min(minBoxes.getMin());
  Point max(maxBoxes.getMax());

  // apply margins:
  if (logScale_ & LOGX)
  {
    if (min[0] > 0.0)
    {
      const Scalar margin = std::pow(10.0, xMargin_);
      min[0] /= margin;
      max[0] *= margin;
    }
    else
      LOGWARN("Negative x values in log-scale axis");
  }
  else
  {
    const Scalar delta = max[0] - min[0];
    const Scalar margin = delta > 0.0 ? xMargin_ * delta : 0.5;
    min[0] -= margin;
    max[0] += margin;
  }

  if (logScale_ & LOGY)
  {
    if (min[1] > 0.0)
    {
      const Scalar margin = std::pow(10.0, yMargin_);
      min[1] /= margin;
      max[1] *= margin;
    }
    else
      LOGWARN("Negative y values in log-scale axis");
  }
  else
  {
    const Scalar delta = max[1] - min[1];
    const Scalar margin = delta > 0.0 ? yMargin_ * delta : 0.5;
    min[1] -= margin;
    max[1] += margin;
  }
  boundingBox_ = Interval(min, max);
}

/* Get the legend position */
String GraphImplementation::getLegendPosition() const
{
  return legendPosition_;
}

/* Set the legend position */
void GraphImplementation::setLegendPosition(const String & position)
{
  if(!IsValidLegendPosition(position)) throw InvalidArgumentException(HERE) << "The given legend position = " << position << " is invalid";

  legendPosition_ = position;
}

/* Gives all the valid legend positions */
Description GraphImplementation::GetValidLegendPositions()
{
  if(IsFirstInitialization)
  {
    InitializeValidLegendPositions();
    IsFirstInitialization = false;
  }
  return ValidLegendPositions;
}

/* Get the legend font size */
Scalar GraphImplementation::getLegendFontSize() const
{
  return legendFontSize_;
}

/* Set the legend font size */
void GraphImplementation::setLegendFontSize(const Scalar legendFontSize)
{
  if(legendFontSize <= 0.0) throw InvalidArgumentException(HERE) << "The given legend font size = " << legendFontSize << " is invalid";

  legendFontSize_ = legendFontSize;
}

/* check for legend position validity */
Bool GraphImplementation::IsValidLegendPosition(const String & position)
{
  const Description::const_iterator it = find(ValidLegendPositions.begin(), ValidLegendPositions.end(), position);

  return (it != ValidLegendPositions.end());
}

/* Method save() stores the object through the StorageManager */
void GraphImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "title_", title_ );
  adv.saveAttribute( "legendPosition_", legendPosition_ );
  adv.saveAttribute( "legendFontSize_", legendFontSize_ );
  adv.saveAttribute( "xTitle_", xTitle_ );
  adv.saveAttribute( "yTitle_", yTitle_ );
  adv.saveAttribute( "showAxes_", showAxes_ );
  adv.saveAttribute( "tickLocation_", static_cast<UnsignedInteger>(tickLocation_) );
  adv.saveAttribute( "logScale_", static_cast<UnsignedInteger>(logScale_) );
  adv.saveAttribute( "showGrid_", showGrid_ );
  adv.saveAttribute( "gridColor_", gridColor_ );
  adv.saveAttribute( "xMargin_", xMargin_ );
  adv.saveAttribute( "yMargin_", yMargin_ );
  adv.saveAttribute( "automaticBoundingBox_", automaticBoundingBox_ );
  adv.saveAttribute( "boundingBox_", boundingBox_ );
  adv.saveAttribute( "drawablesCollection_", drawablesCollection_ );
}

/* Method load() reloads the object from the StorageManager */
void GraphImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "title_", title_ );
  adv.loadAttribute( "legendPosition_", legendPosition_ );
  adv.loadAttribute( "legendFontSize_", legendFontSize_ );
  adv.loadAttribute( "xTitle_", xTitle_ );
  adv.loadAttribute( "yTitle_", yTitle_ );
  adv.loadAttribute( "showAxes_", showAxes_ );
  UnsignedInteger tickLocation = 0;
  adv.loadAttribute( "tickLocation_", tickLocation );
  tickLocation_ = static_cast<TickLocation>(tickLocation);
  UnsignedInteger logScale = 0;
  adv.loadAttribute( "logScale_", logScale );
  logScale_ = static_cast<LogScale>(logScale);
  adv.loadAttribute( "showGrid_", showGrid_ );
  adv.loadAttribute( "gridColor_", gridColor_ );
  adv.loadAttribute( "xMargin_", xMargin_ );
  adv.loadAttribute( "yMargin_", yMargin_ );
  adv.loadAttribute( "automaticBoundingBox_", automaticBoundingBox_ );
  adv.loadAttribute( "boundingBox_", boundingBox_ );
  adv.loadAttribute( "drawablesCollection_", drawablesCollection_ );
}

END_NAMESPACE_OPENTURNS
