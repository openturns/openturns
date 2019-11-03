//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Drawable
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "openturns/DrawableImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DrawableImplementation)

static const Factory<DrawableImplementation> Factory_DrawableImplementation;

/* default graphic paramaters */

Bool DrawableImplementation::IsFirstInitialization          = true;

/* A map  matching keys with R codes for point symbols */
std::map<String, UnsignedInteger> DrawableImplementation::SymbolCodes;
/* A map  matching keys with R codes for point symbols */

std::map<String, String> DrawableImplementation::ColorCodes;

/* Accepted line styles */
Description DrawableImplementation::ValidLineStyles;

/* Accepted fill styles */
Description DrawableImplementation::ValidFillStyles;

void DrawableImplementation::InitializeValidParameterList()
{
  /* A map  matching keys with R codes for point symbols */
  SymbolCodes["square"]       = 0;
  SymbolCodes["circle"]       = 1;
  SymbolCodes["triangleup"]   = 2;
  SymbolCodes["plus"]         = 3;
  SymbolCodes["times"]        = 4;
  SymbolCodes["diamond"]      = 5;
  SymbolCodes["triangledown"] = 6;
  SymbolCodes["star"]         = 8;
  SymbolCodes["fsquare"]      = 15;
  SymbolCodes["fcircle"]      = 16;
  SymbolCodes["ftriangleup"]  = 17;
  SymbolCodes["fdiamond"]     = 18;
  SymbolCodes["bullet"]       = 20;
  SymbolCodes["dot"]          = 127;
  SymbolCodes["none"]         = 256;

  /* Accepted colors */
  ColorCodes["white"]                = "#FFFFFF";
  ColorCodes["aliceblue"]            = "#F0F8FF";
  ColorCodes["antiquewhite"]         = "#FAEBD7";
  ColorCodes["antiquewhite1"]        = "#FFEFDB";
  ColorCodes["antiquewhite2"]        = "#EEDFCC";
  ColorCodes["antiquewhite3"]        = "#CDC0B0";
  ColorCodes["antiquewhite4"]        = "#8B8378";
  ColorCodes["aquamarine"]           = "#7FFFD4";
  ColorCodes["aquamarine1"]          = "#7FFFD4";
  ColorCodes["aquamarine2"]          = "#76EEC6";
  ColorCodes["aquamarine3"]          = "#66CDAA";
  ColorCodes["aquamarine4"]          = "#458B74";
  ColorCodes["azure"]                = "#F0FFFF";
  ColorCodes["azure1"]               = "#F0FFFF";
  ColorCodes["azure2"]               = "#E0EEEE";
  ColorCodes["azure3"]               = "#C1CDCD";
  ColorCodes["azure4"]               = "#838B8B";
  ColorCodes["beige"]                = "#F5F5DC";
  ColorCodes["bisque"]               = "#FFE4C4";
  ColorCodes["bisque1"]              = "#FFE4C4";
  ColorCodes["bisque2"]              = "#EED5B7";
  ColorCodes["bisque3"]              = "#CDB79E";
  ColorCodes["bisque4"]              = "#8B7D6B";
  ColorCodes["black"]                = "#000000";
  ColorCodes["blanchedalmond"]       = "#FFEBCD";
  ColorCodes["blue"]                 = "#0000FF";
  ColorCodes["blue1"]                = "#0000FF";
  ColorCodes["blue2"]                = "#0000EE";
  ColorCodes["blue3"]                = "#0000CD";
  ColorCodes["blue4"]                = "#00008B";
  ColorCodes["blueviolet"]           = "#8A2BE2";
  ColorCodes["brown"]                = "#A52A2A";
  ColorCodes["brown1"]               = "#FF4040";
  ColorCodes["brown2"]               = "#EE3B3B";
  ColorCodes["brown3"]               = "#CD3333";
  ColorCodes["brown4"]               = "#8B2323";
  ColorCodes["burlywood"]            = "#DEB887";
  ColorCodes["burlywood1"]           = "#FFD39B";
  ColorCodes["burlywood2"]           = "#EEC591";
  ColorCodes["burlywood3"]           = "#CDAA7D";
  ColorCodes["burlywood4"]           = "#8B7355";
  ColorCodes["cadetblue"]            = "#5F9EA0";
  ColorCodes["cadetblue1"]           = "#98F5FF";
  ColorCodes["cadetblue2"]           = "#8EE5EE";
  ColorCodes["cadetblue3"]           = "#7AC5CD";
  ColorCodes["cadetblue4"]           = "#53868B";
  ColorCodes["chartreuse"]           = "#7FFF00";
  ColorCodes["chartreuse1"]          = "#7FFF00";
  ColorCodes["chartreuse2"]          = "#76EE00";
  ColorCodes["chartreuse3"]          = "#66CD00";
  ColorCodes["chartreuse4"]          = "#458B00";
  ColorCodes["chocolate"]            = "#D2691E";
  ColorCodes["chocolate1"]           = "#FF7F24";
  ColorCodes["chocolate2"]           = "#EE7621";
  ColorCodes["chocolate3"]           = "#CD661D";
  ColorCodes["chocolate4"]           = "#8B4513";
  ColorCodes["coral"]                = "#FF7F50";
  ColorCodes["coral1"]               = "#FF7256";
  ColorCodes["coral2"]               = "#EE6A50";
  ColorCodes["coral3"]               = "#CD5B45";
  ColorCodes["coral4"]               = "#8B3E2F";
  ColorCodes["cornflowerblue"]       = "#6495ED";
  ColorCodes["cornsilk"]             = "#FFF8DC";
  ColorCodes["cornsilk1"]            = "#FFF8DC";
  ColorCodes["cornsilk2"]            = "#EEE8CD";
  ColorCodes["cornsilk3"]            = "#CDC8B1";
  ColorCodes["cornsilk4"]            = "#8B8878";
  ColorCodes["cyan"]                 = "#00FFFF";
  ColorCodes["cyan1"]                = "#00FFFF";
  ColorCodes["cyan2"]                = "#00EEEE";
  ColorCodes["cyan3"]                = "#00CDCD";
  ColorCodes["cyan4"]                = "#008B8B";
  ColorCodes["darkblue"]             = "#00008B";
  ColorCodes["darkcyan"]             = "#008B8B";
  ColorCodes["darkgoldenrod"]        = "#B8860B";
  ColorCodes["darkgoldenrod1"]       = "#FFB90F";
  ColorCodes["darkgoldenrod2"]       = "#EEAD0E";
  ColorCodes["darkgoldenrod3"]       = "#CD950C";
  ColorCodes["darkgoldenrod4"]       = "#8B6508";
  ColorCodes["darkgray"]             = "#A9A9A9";
  ColorCodes["darkgreen"]            = "#006400";
  ColorCodes["darkgrey"]             = "#A9A9A9";
  ColorCodes["darkkhaki"]            = "#BDB76B";
  ColorCodes["darkmagenta"]          = "#8B008B";
  ColorCodes["darkolivegreen"]       = "#556B2F";
  ColorCodes["darkolivegreen1"]      = "#CAFF70";
  ColorCodes["darkolivegreen2"]      = "#BCEE68";
  ColorCodes["darkolivegreen3"]      = "#A2CD5A";
  ColorCodes["darkolivegreen4"]      = "#6E8B3D";
  ColorCodes["darkorange"]           = "#FF8C00";
  ColorCodes["darkorange1"]          = "#FF7F00";
  ColorCodes["darkorange2"]          = "#EE7600";
  ColorCodes["darkorange3"]          = "#CD6600";
  ColorCodes["darkorange4"]          = "#8B4500";
  ColorCodes["darkorchid"]           = "#9932CC";
  ColorCodes["darkorchid1"]          = "#BF3EFF";
  ColorCodes["darkorchid2"]          = "#B23AEE";
  ColorCodes["darkorchid3"]          = "#9A32CD";
  ColorCodes["darkorchid4"]          = "#68228B";
  ColorCodes["darkred"]              = "#8B0000";
  ColorCodes["darksalmon"]           = "#E9967A";
  ColorCodes["darkseagreen"]         = "#8FBC8F";
  ColorCodes["darkseagreen1"]        = "#C1FFC1";
  ColorCodes["darkseagreen2"]        = "#B4EEB4";
  ColorCodes["darkseagreen3"]        = "#9BCD9B";
  ColorCodes["darkseagreen4"]        = "#698B69";
  ColorCodes["darkslateblue"]        = "#483D8B";
  ColorCodes["darkslategray"]        = "#2F4F4F";
  ColorCodes["darkslategray1"]       = "#97FFFF";
  ColorCodes["darkslategray2"]       = "#8DEEEE";
  ColorCodes["darkslategray3"]       = "#79CDCD";
  ColorCodes["darkslategray4"]       = "#528B8B";
  ColorCodes["darkslategrey"]        = "#2F4F4F";
  ColorCodes["darkturquoise"]        = "#00CED1";
  ColorCodes["darkviolet"]           = "#9400D3";
  ColorCodes["deeppink"]             = "#FF1493";
  ColorCodes["deeppink1"]            = "#FF1493";
  ColorCodes["deeppink2"]            = "#EE1289";
  ColorCodes["deeppink3"]            = "#CD1076";
  ColorCodes["deeppink4"]            = "#8B0A50";
  ColorCodes["deepskyblue"]          = "#00BFFF";
  ColorCodes["deepskyblue1"]         = "#00BFFF";
  ColorCodes["deepskyblue2"]         = "#00B2EE";
  ColorCodes["deepskyblue3"]         = "#009ACD";
  ColorCodes["deepskyblue4"]         = "#00688B";
  ColorCodes["dimgray"]              = "#696969";
  ColorCodes["dimgrey"]              = "#696969";
  ColorCodes["dodgerblue"]           = "#1E90FF";
  ColorCodes["dodgerblue1"]          = "#1E90FF";
  ColorCodes["dodgerblue2"]          = "#1C86EE";
  ColorCodes["dodgerblue3"]          = "#1874CD";
  ColorCodes["dodgerblue4"]          = "#104E8B";
  ColorCodes["firebrick"]            = "#B22222";
  ColorCodes["firebrick1"]           = "#FF3030";
  ColorCodes["firebrick2"]           = "#EE2C2C";
  ColorCodes["firebrick3"]           = "#CD2626";
  ColorCodes["firebrick4"]           = "#8B1A1A";
  ColorCodes["floralwhite"]          = "#FFFAF0";
  ColorCodes["forestgreen"]          = "#228B22";
  ColorCodes["gainsboro"]            = "#DCDCDC";
  ColorCodes["ghostwhite"]           = "#F8F8FF";
  ColorCodes["gold"]                 = "#FFD700";
  ColorCodes["gold1"]                = "#FFD700";
  ColorCodes["gold2"]                = "#EEC900";
  ColorCodes["gold3"]                = "#CDAD00";
  ColorCodes["gold4"]                = "#8B7500";
  ColorCodes["goldenrod"]            = "#DAA520";
  ColorCodes["goldenrod1"]           = "#FFC125";
  ColorCodes["goldenrod2"]           = "#EEB422";
  ColorCodes["goldenrod3"]           = "#CD9B1D";
  ColorCodes["goldenrod4"]           = "#8B6914";
  {
    ColorCodes["gray"]                 = "#BEBEBE";
    ColorCodes["gray0"]                = "#000000";
    ColorCodes["gray1"]                = "#030303";
    ColorCodes["gray2"]                = "#050505";
    ColorCodes["gray3"]                = "#080808";
    ColorCodes["gray4"]                = "#0A0A0A";
    ColorCodes["gray5"]                = "#0D0D0D";
    ColorCodes["gray6"]                = "#0F0F0F";
    ColorCodes["gray7"]                = "#121212";
    ColorCodes["gray8"]                = "#141414";
    ColorCodes["gray9"]                = "#171717";
    ColorCodes["gray10"]               = "#1A1A1A";
    ColorCodes["gray11"]               = "#1C1C1C";
    ColorCodes["gray12"]               = "#1F1F1F";
    ColorCodes["gray13"]               = "#212121";
    ColorCodes["gray14"]               = "#242424";
    ColorCodes["gray15"]               = "#262626";
    ColorCodes["gray16"]               = "#292929";
    ColorCodes["gray17"]               = "#2B2B2B";
    ColorCodes["gray18"]               = "#2E2E2E";
    ColorCodes["gray19"]               = "#303030";
    ColorCodes["gray20"]               = "#333333";
    ColorCodes["gray21"]               = "#363636";
    ColorCodes["gray22"]               = "#383838";
    ColorCodes["gray23"]               = "#3B3B3B";
    ColorCodes["gray24"]               = "#3D3D3D";
    ColorCodes["gray25"]               = "#404040";
    ColorCodes["gray26"]               = "#424242";
    ColorCodes["gray27"]               = "#454545";
    ColorCodes["gray28"]               = "#474747";
    ColorCodes["gray29"]               = "#4A4A4A";
    ColorCodes["gray30"]               = "#4D4D4D";
    ColorCodes["gray31"]               = "#4F4F4F";
    ColorCodes["gray32"]               = "#525252";
    ColorCodes["gray33"]               = "#545454";
    ColorCodes["gray34"]               = "#575757";
    ColorCodes["gray35"]               = "#595959";
    ColorCodes["gray36"]               = "#5C5C5C";
    ColorCodes["gray37"]               = "#5E5E5E";
    ColorCodes["gray38"]               = "#616161";
    ColorCodes["gray39"]               = "#636363";
    ColorCodes["gray40"]               = "#666666";
    ColorCodes["gray41"]               = "#696969";
    ColorCodes["gray42"]               = "#6B6B6B";
    ColorCodes["gray43"]               = "#6E6E6E";
    ColorCodes["gray44"]               = "#707070";
    ColorCodes["gray45"]               = "#737373";
    ColorCodes["gray46"]               = "#757575";
    ColorCodes["gray47"]               = "#787878";
    ColorCodes["gray48"]               = "#7A7A7A";
    ColorCodes["gray49"]               = "#7D7D7D";
    ColorCodes["gray50"]               = "#7F7F7F";
    ColorCodes["gray51"]               = "#828282";
    ColorCodes["gray52"]               = "#858585";
    ColorCodes["gray53"]               = "#878787";
    ColorCodes["gray54"]               = "#8A8A8A";
    ColorCodes["gray55"]               = "#8C8C8C";
    ColorCodes["gray56"]               = "#8F8F8F";
    ColorCodes["gray57"]               = "#919191";
    ColorCodes["gray58"]               = "#949494";
    ColorCodes["gray59"]               = "#969696";
    ColorCodes["gray60"]               = "#999999";
    ColorCodes["gray61"]               = "#9C9C9C";
    ColorCodes["gray62"]               = "#9E9E9E";
    ColorCodes["gray63"]               = "#A1A1A1";
    ColorCodes["gray64"]               = "#A3A3A3";
    ColorCodes["gray65"]               = "#A6A6A6";
    ColorCodes["gray66"]               = "#A8A8A8";
    ColorCodes["gray67"]               = "#ABABAB";
    ColorCodes["gray68"]               = "#ADADAD";
    ColorCodes["gray69"]               = "#B0B0B0";
    ColorCodes["gray70"]               = "#B3B3B3";
    ColorCodes["gray71"]               = "#B5B5B5";
    ColorCodes["gray72"]               = "#B8B8B8";
    ColorCodes["gray73"]               = "#BABABA";
    ColorCodes["gray74"]               = "#BDBDBD";
    ColorCodes["gray75"]               = "#BFBFBF";
    ColorCodes["gray76"]               = "#C2C2C2";
    ColorCodes["gray77"]               = "#C4C4C4";
    ColorCodes["gray78"]               = "#C7C7C7";
    ColorCodes["gray79"]               = "#C9C9C9";
    ColorCodes["gray80"]               = "#CCCCCC";
    ColorCodes["gray81"]               = "#CFCFCF";
    ColorCodes["gray82"]               = "#D1D1D1";
    ColorCodes["gray83"]               = "#D4D4D4";
    ColorCodes["gray84"]               = "#D6D6D6";
    ColorCodes["gray85"]               = "#D9D9D9";
    ColorCodes["gray86"]               = "#DBDBDB";
    ColorCodes["gray87"]               = "#DEDEDE";
    ColorCodes["gray88"]               = "#E0E0E0";
    ColorCodes["gray89"]               = "#E3E3E3";
    ColorCodes["gray90"]               = "#E5E5E5";
    ColorCodes["gray91"]               = "#E8E8E8";
    ColorCodes["gray92"]               = "#EBEBEB";
    ColorCodes["gray93"]               = "#EDEDED";
    ColorCodes["gray94"]               = "#F0F0F0";
    ColorCodes["gray95"]               = "#F2F2F2";
    ColorCodes["gray96"]               = "#F5F5F5";
    ColorCodes["gray97"]               = "#F7F7F7";
    ColorCodes["gray98"]               = "#FAFAFA";
    ColorCodes["gray99"]               = "#FCFCFC";
    ColorCodes["gray100"]              = "#FFFFFF";
    ColorCodes["green"]                = "#00FF00";
    ColorCodes["green1"]               = "#00FF00";
    ColorCodes["green2"]               = "#00EE00";
    ColorCodes["green3"]               = "#00CD00";
    ColorCodes["green4"]               = "#008B00";
    ColorCodes["greenyellow"]          = "#ADFF2F";
    ColorCodes["grey"]                 = "#BEBEBE";
    ColorCodes["grey0"]                = "#000000";
    ColorCodes["grey1"]                = "#030303";
    ColorCodes["grey2"]                = "#050505";
    ColorCodes["grey3"]                = "#080808";
    ColorCodes["grey4"]                = "#0A0A0A";
    ColorCodes["grey5"]                = "#0D0D0D";
    ColorCodes["grey6"]                = "#0F0F0F";
    ColorCodes["grey7"]                = "#121212";
    ColorCodes["grey8"]                = "#141414";
    ColorCodes["grey9"]                = "#171717";
    ColorCodes["grey10"]               = "#1A1A1A";
    ColorCodes["grey11"]               = "#1C1C1C";
    ColorCodes["grey12"]               = "#1F1F1F";
    ColorCodes["grey13"]               = "#212121";
    ColorCodes["grey14"]               = "#242424";
    ColorCodes["grey15"]               = "#262626";
    ColorCodes["grey16"]               = "#292929";
    ColorCodes["grey17"]               = "#2B2B2B";
    ColorCodes["grey18"]               = "#2E2E2E";
    ColorCodes["grey19"]               = "#303030";
    ColorCodes["grey20"]               = "#333333";
    ColorCodes["grey21"]               = "#363636";
    ColorCodes["grey22"]               = "#383838";
    ColorCodes["grey23"]               = "#3B3B3B";
    ColorCodes["grey24"]               = "#3D3D3D";
    ColorCodes["grey25"]               = "#404040";
    ColorCodes["grey26"]               = "#424242";
    ColorCodes["grey27"]               = "#454545";
    ColorCodes["grey28"]               = "#474747";
    ColorCodes["grey29"]               = "#4A4A4A";
    ColorCodes["grey30"]               = "#4D4D4D";
    ColorCodes["grey31"]               = "#4F4F4F";
    ColorCodes["grey32"]               = "#525252";
    ColorCodes["grey33"]               = "#545454";
    ColorCodes["grey34"]               = "#575757";
    ColorCodes["grey35"]               = "#595959";
    ColorCodes["grey36"]               = "#5C5C5C";
    ColorCodes["grey37"]               = "#5E5E5E";
    ColorCodes["grey38"]               = "#616161";
    ColorCodes["grey39"]               = "#636363";
    ColorCodes["grey40"]               = "#666666";
    ColorCodes["grey41"]               = "#696969";
    ColorCodes["grey42"]               = "#6B6B6B";
    ColorCodes["grey43"]               = "#6E6E6E";
    ColorCodes["grey44"]               = "#707070";
    ColorCodes["grey45"]               = "#737373";
    ColorCodes["grey46"]               = "#757575";
    ColorCodes["grey47"]               = "#787878";
    ColorCodes["grey48"]               = "#7A7A7A";
    ColorCodes["grey49"]               = "#7D7D7D";
    ColorCodes["grey50"]               = "#7F7F7F";
    ColorCodes["grey51"]               = "#828282";
    ColorCodes["grey52"]               = "#858585";
    ColorCodes["grey53"]               = "#878787";
    ColorCodes["grey54"]               = "#8A8A8A";
    ColorCodes["grey55"]               = "#8C8C8C";
    ColorCodes["grey56"]               = "#8F8F8F";
    ColorCodes["grey57"]               = "#919191";
    ColorCodes["grey58"]               = "#949494";
    ColorCodes["grey59"]               = "#969696";
    ColorCodes["grey60"]               = "#999999";
    ColorCodes["grey61"]               = "#9C9C9C";
    ColorCodes["grey62"]               = "#9E9E9E";
    ColorCodes["grey63"]               = "#A1A1A1";
    ColorCodes["grey64"]               = "#A3A3A3";
    ColorCodes["grey65"]               = "#A6A6A6";
    ColorCodes["grey66"]               = "#A8A8A8";
    ColorCodes["grey67"]               = "#ABABAB";
    ColorCodes["grey68"]               = "#ADADAD";
    ColorCodes["grey69"]               = "#B0B0B0";
    ColorCodes["grey70"]               = "#B3B3B3";
    ColorCodes["grey71"]               = "#B5B5B5";
    ColorCodes["grey72"]               = "#B8B8B8";
    ColorCodes["grey73"]               = "#BABABA";
    ColorCodes["grey74"]               = "#BDBDBD";
    ColorCodes["grey75"]               = "#BFBFBF";
    ColorCodes["grey76"]               = "#C2C2C2";
    ColorCodes["grey77"]               = "#C4C4C4";
    ColorCodes["grey78"]               = "#C7C7C7";
    ColorCodes["grey79"]               = "#C9C9C9";
    ColorCodes["grey80"]               = "#CCCCCC";
    ColorCodes["grey81"]               = "#CFCFCF";
    ColorCodes["grey82"]               = "#D1D1D1";
    ColorCodes["grey83"]               = "#D4D4D4";
    ColorCodes["grey84"]               = "#D6D6D6";
    ColorCodes["grey85"]               = "#D9D9D9";
    ColorCodes["grey86"]               = "#DBDBDB";
    ColorCodes["grey87"]               = "#DEDEDE";
    ColorCodes["grey88"]               = "#E0E0E0";
    ColorCodes["grey89"]               = "#E3E3E3";
    ColorCodes["grey90"]               = "#E5E5E5";
    ColorCodes["grey91"]               = "#E8E8E8";
    ColorCodes["grey92"]               = "#EBEBEB";
    ColorCodes["grey93"]               = "#EDEDED";
    ColorCodes["grey94"]               = "#F0F0F0";
    ColorCodes["grey95"]               = "#F2F2F2";
    ColorCodes["grey96"]               = "#F5F5F5";
    ColorCodes["grey97"]               = "#F7F7F7";
    ColorCodes["grey98"]               = "#FAFAFA";
    ColorCodes["grey99"]               = "#FCFCFC";
    ColorCodes["grey100"]              = "#FFFFFF";
  }
  ColorCodes["honeydew"]             = "#F0FFF0";
  ColorCodes["honeydew1"]            = "#F0FFF0";
  ColorCodes["honeydew2"]            = "#E0EEE0";
  ColorCodes["honeydew3"]            = "#C1CDC1";
  ColorCodes["honeydew4"]            = "#838B83";
  ColorCodes["hotpink"]              = "#FF69B4";
  ColorCodes["hotpink1"]             = "#FF6EB4";
  ColorCodes["hotpink2"]             = "#EE6AA7";
  ColorCodes["hotpink3"]             = "#CD6090";
  ColorCodes["hotpink4"]             = "#8B3A62";
  ColorCodes["indianred"]            = "#CD5C5C";
  ColorCodes["indianred1"]           = "#FF6A6A";
  ColorCodes["indianred2"]           = "#EE6363";
  ColorCodes["indianred3"]           = "#CD5555";
  ColorCodes["indianred4"]           = "#8B3A3A";
  ColorCodes["ivory"]                = "#FFFFF0";
  ColorCodes["ivory1"]               = "#FFFFF0";
  ColorCodes["ivory2"]               = "#EEEEE0";
  ColorCodes["ivory3"]               = "#CDCDC1";
  ColorCodes["ivory4"]               = "#8B8B83";
  ColorCodes["khaki"]                = "#F0E68C";
  ColorCodes["khaki1"]               = "#FFF68F";
  ColorCodes["khaki2"]               = "#EEE685";
  ColorCodes["khaki3"]               = "#CDC673";
  ColorCodes["khaki4"]               = "#8B864E";
  ColorCodes["lavender"]             = "#E6E6FA";
  ColorCodes["lavenderblush"]        = "#FFF0F5";
  ColorCodes["lavenderblush1"]       = "#FFF0F5";
  ColorCodes["lavenderblush2"]       = "#EEE0E5";
  ColorCodes["lavenderblush3"]       = "#CDC1C5";
  ColorCodes["lavenderblush4"]       = "#8B8386";
  ColorCodes["lawngreen"]            = "#7CFC00";
  ColorCodes["lemonchiffon"]         = "#FFFACD";
  ColorCodes["lemonchiffon1"]        = "#FFFACD";
  ColorCodes["lemonchiffon2"]        = "#EEE9BF";
  ColorCodes["lemonchiffon3"]        = "#CDC9A5";
  ColorCodes["lemonchiffon4"]        = "#8B8970";
  ColorCodes["lightblue"]            = "#ADD8E6";
  ColorCodes["lightblue1"]           = "#BFEFFF";
  ColorCodes["lightblue2"]           = "#B2DFEE";
  ColorCodes["lightblue3"]           = "#9AC0CD";
  ColorCodes["lightblue4"]           = "#68838B";
  ColorCodes["lightcoral"]           = "#F08080";
  ColorCodes["lightcyan"]            = "#E0FFFF";
  ColorCodes["lightcyan1"]           = "#E0FFFF";
  ColorCodes["lightcyan2"]           = "#D1EEEE";
  ColorCodes["lightcyan3"]           = "#B4CDCD";
  ColorCodes["lightcyan4"]           = "#7A8B8B";
  ColorCodes["lightgoldenrod"]       = "#EEDD82";
  ColorCodes["lightgoldenrod1"]      = "#FFEC8B";
  ColorCodes["lightgoldenrod2"]      = "#EEDC82";
  ColorCodes["lightgoldenrod3"]      = "#CDBE70";
  ColorCodes["lightgoldenrod4"]      = "#8B814C";
  ColorCodes["lightgoldenrodyellow"] = "#FAFAD2";
  ColorCodes["lightgray"]            = "#D3D3D3";
  ColorCodes["lightgreen"]           = "#90EE90";
  ColorCodes["lightgrey"]            = "#D3D3D3";
  ColorCodes["lightpink"]            = "#FFB6C1";
  ColorCodes["lightpink1"]           = "#FFAEB9";
  ColorCodes["lightpink2"]           = "#EEA2AD";
  ColorCodes["lightpink3"]           = "#CD8C95";
  ColorCodes["lightpink4"]           = "#8B5F65";
  ColorCodes["lightsalmon"]          = "#FFA07A";
  ColorCodes["lightsalmon1"]         = "#FFA07A";
  ColorCodes["lightsalmon2"]         = "#EE9572";
  ColorCodes["lightsalmon3"]         = "#CD8162";
  ColorCodes["lightsalmon4"]         = "#8B5742";
  ColorCodes["lightseagreen"]        = "#20B2AA";
  ColorCodes["lightskyblue"]         = "#87CEFA";
  ColorCodes["lightskyblue1"]        = "#B0E2FF";
  ColorCodes["lightskyblue2"]        = "#A4D3EE";
  ColorCodes["lightskyblue3"]        = "#8DB6CD";
  ColorCodes["lightskyblue4"]        = "#607B8B";
  ColorCodes["lightslateblue"]       = "#8470FF";
  ColorCodes["lightslategray"]       = "#778899";
  ColorCodes["lightslategrey"]       = "#778899";
  ColorCodes["lightsteelblue"]       = "#B0C4DE";
  ColorCodes["lightsteelblue1"]      = "#CAE1FF";
  ColorCodes["lightsteelblue2"]      = "#BCD2EE";
  ColorCodes["lightsteelblue3"]      = "#A2B5CD";
  ColorCodes["lightsteelblue4"]      = "#6E7B8B";
  ColorCodes["lightyellow"]          = "#FFFFE0";
  ColorCodes["lightyellow1"]         = "#FFFFE0";
  ColorCodes["lightyellow2"]         = "#EEEED1";
  ColorCodes["lightyellow3"]         = "#CDCDB4";
  ColorCodes["lightyellow4"]         = "#8B8B7A";
  ColorCodes["limegreen"]            = "#32CD32";
  ColorCodes["linen"]                = "#FAF0E6";
  ColorCodes["magenta"]              = "#FF00FF";
  ColorCodes["magenta1"]             = "#FF00FF";
  ColorCodes["magenta2"]             = "#EE00EE";
  ColorCodes["magenta3"]             = "#CD00CD";
  ColorCodes["magenta4"]             = "#8B008B";
  ColorCodes["maroon"]               = "#B03060";
  ColorCodes["maroon1"]              = "#FF34B3";
  ColorCodes["maroon2"]              = "#EE30A7";
  ColorCodes["maroon3"]              = "#CD2990";
  ColorCodes["maroon4"]              = "#8B1C62";
  ColorCodes["mediumaquamarine"]     = "#66CDAA";
  ColorCodes["mediumblue"]           = "#0000CD";
  ColorCodes["mediumorchid"]         = "#BA55D3";
  ColorCodes["mediumorchid1"]        = "#E066FF";
  ColorCodes["mediumorchid2"]        = "#D15FEE";
  ColorCodes["mediumorchid3"]        = "#B452CD";
  ColorCodes["mediumorchid4"]        = "#7A378B";
  ColorCodes["mediumpurple"]         = "#9370DB";
  ColorCodes["mediumpurple1"]        = "#AB82FF";
  ColorCodes["mediumpurple2"]        = "#9F79EE";
  ColorCodes["mediumpurple3"]        = "#8968CD";
  ColorCodes["mediumpurple4"]        = "#5D478B";
  ColorCodes["mediumseagreen"]       = "#3CB371";
  ColorCodes["mediumslateblue"]      = "#7B68EE";
  ColorCodes["mediumspringgreen"]    = "#00FA9A";
  ColorCodes["mediumturquoise"]      = "#48D1CC";
  ColorCodes["mediumvioletred"]      = "#C71585";
  ColorCodes["midnightblue"]         = "#191970";
  ColorCodes["mintcream"]            = "#F5FFFA";
  ColorCodes["mistyrose"]            = "#FFE4E1";
  ColorCodes["mistyrose1"]           = "#FFE4E1";
  ColorCodes["mistyrose2"]           = "#EED5D2";
  ColorCodes["mistyrose3"]           = "#CDB7B5";
  ColorCodes["mistyrose4"]           = "#8B7D7B";
  ColorCodes["moccasin"]             = "#FFE4B5";
  ColorCodes["navajowhite"]          = "#FFDEAD";
  ColorCodes["navajowhite1"]         = "#FFDEAD";
  ColorCodes["navajowhite2"]         = "#EECFA1";
  ColorCodes["navajowhite3"]         = "#CDB38B";
  ColorCodes["navajowhite4"]         = "#8B795E";
  ColorCodes["navy"]                 = "#000080";
  ColorCodes["navyblue"]             = "#000080";
  ColorCodes["oldlace"]              = "#FDF5E6";
  ColorCodes["olivedrab"]            = "#6B8E23";
  ColorCodes["olivedrab1"]           = "#C0FF3E";
  ColorCodes["olivedrab2"]           = "#B3EE3A";
  ColorCodes["olivedrab3"]           = "#9ACD32";
  ColorCodes["olivedrab4"]           = "#698B22";
  ColorCodes["orange"]               = "#FFA500";
  ColorCodes["orange1"]              = "#FFA500";
  ColorCodes["orange2"]              = "#EE9A00";
  ColorCodes["orange3"]              = "#CD8500";
  ColorCodes["orange4"]              = "#8B5A00";
  ColorCodes["orangered"]            = "#FF4500";
  ColorCodes["orangered1"]           = "#FF4500";
  ColorCodes["orangered2"]           = "#EE4000";
  ColorCodes["orangered3"]           = "#CD3700";
  ColorCodes["orangered4"]           = "#8B2500";
  ColorCodes["orchid"]               = "#DA70D6";
  ColorCodes["orchid1"]              = "#FF83FA";
  ColorCodes["orchid2"]              = "#EE7AE9";
  ColorCodes["orchid3"]              = "#CD69C9";
  ColorCodes["orchid4"]              = "#8B4789";
  ColorCodes["palegoldenrod"]        = "#EEE8AA";
  ColorCodes["palegreen"]            = "#98FB98";
  ColorCodes["palegreen1"]           = "#9AFF9A";
  ColorCodes["palegreen2"]           = "#90EE90";
  ColorCodes["palegreen3"]           = "#7CCD7C";
  ColorCodes["palegreen4"]           = "#548B54";
  ColorCodes["paleturquoise"]        = "#AFEEEE";
  ColorCodes["paleturquoise1"]       = "#BBFFFF";
  ColorCodes["paleturquoise2"]       = "#AEEEEE";
  ColorCodes["paleturquoise3"]       = "#96CDCD";
  ColorCodes["paleturquoise4"]       = "#668B8B";
  ColorCodes["palevioletred"]        = "#DB7093";
  ColorCodes["palevioletred1"]       = "#FF82AB";
  ColorCodes["palevioletred2"]       = "#EE799F";
  ColorCodes["palevioletred3"]       = "#CD6889";
  ColorCodes["palevioletred4"]       = "#8B475D";
  ColorCodes["papayawhip"]           = "#FFEFD5";
  ColorCodes["peachpuff"]            = "#FFDAB9";
  ColorCodes["peachpuff1"]           = "#FFDAB9";
  ColorCodes["peachpuff2"]           = "#EECBAD";
  ColorCodes["peachpuff3"]           = "#CDAF95";
  ColorCodes["peachpuff4"]           = "#8B7765";
  ColorCodes["peru"]                 = "#CD853F";
  ColorCodes["pink"]                 = "#FFC0CB";
  ColorCodes["pink1"]                = "#FFB5C5";
  ColorCodes["pink2"]                = "#EEA9B8";
  ColorCodes["pink3"]                = "#CD919E";
  ColorCodes["pink4"]                = "#8B636C";
  ColorCodes["plum"]                 = "#DDA0DD";
  ColorCodes["plum1"]                = "#FFBBFF";
  ColorCodes["plum2"]                = "#EEAEEE";
  ColorCodes["plum3"]                = "#CD96CD";
  ColorCodes["plum4"]                = "#8B668B";
  ColorCodes["powderblue"]           = "#B0E0E6";
  ColorCodes["purple"]               = "#A020F0";
  ColorCodes["purple1"]              = "#9B30FF";
  ColorCodes["purple2"]              = "#912CEE";
  ColorCodes["purple3"]              = "#7D26CD";
  ColorCodes["purple4"]              = "#551A8B";
  ColorCodes["red"]                  = "#FF0000";
  ColorCodes["red1"]                 = "#FF0000";
  ColorCodes["red2"]                 = "#EE0000";
  ColorCodes["red3"]                 = "#CD0000";
  ColorCodes["red4"]                 = "#8B0000";
  ColorCodes["rosybrown"]            = "#BC8F8F";
  ColorCodes["rosybrown1"]           = "#FFC1C1";
  ColorCodes["rosybrown2"]           = "#EEB4B4";
  ColorCodes["rosybrown3"]           = "#CD9B9B";
  ColorCodes["rosybrown4"]           = "#8B6969";
  ColorCodes["royalblue"]            = "#4169E1";
  ColorCodes["royalblue1"]           = "#4876FF";
  ColorCodes["royalblue2"]           = "#436EEE";
  ColorCodes["royalblue3"]           = "#3A5FCD";
  ColorCodes["royalblue4"]           = "#27408B";
  ColorCodes["saddlebrown"]          = "#8B4513";
  ColorCodes["salmon"]               = "#FA8072";
  ColorCodes["salmon1"]              = "#FF8C69";
  ColorCodes["salmon2"]              = "#EE8262";
  ColorCodes["salmon3"]              = "#CD7054";
  ColorCodes["salmon4"]              = "#8B4C39";
  ColorCodes["sandybrown"]           = "#F4A460";
  ColorCodes["seagreen"]             = "#2E8B57";
  ColorCodes["seagreen1"]            = "#54FF9F";
  ColorCodes["seagreen2"]            = "#4EEE94";
  ColorCodes["seagreen3"]            = "#43CD80";
  ColorCodes["seagreen4"]            = "#2E8B57";
  ColorCodes["seashell"]             = "#FFF5EE";
  ColorCodes["seashell1"]            = "#FFF5EE";
  ColorCodes["seashell2"]            = "#EEE5DE";
  ColorCodes["seashell3"]            = "#CDC5BF";
  ColorCodes["seashell4"]            = "#8B8682";
  ColorCodes["sienna"]               = "#A0522D";
  ColorCodes["sienna1"]              = "#FF8247";
  ColorCodes["sienna2"]              = "#EE7942";
  ColorCodes["sienna3"]              = "#CD6839";
  ColorCodes["sienna4"]              = "#8B4726";
  ColorCodes["skyblue"]              = "#87CEEB";
  ColorCodes["skyblue1"]             = "#87CEFF";
  ColorCodes["skyblue2"]             = "#7EC0EE";
  ColorCodes["skyblue3"]             = "#6CA6CD";
  ColorCodes["skyblue4"]             = "#4A708B";
  ColorCodes["slateblue"]            = "#6A5ACD";
  ColorCodes["slateblue1"]           = "#836FFF";
  ColorCodes["slateblue2"]           = "#7A67EE";
  ColorCodes["slateblue3"]           = "#6959CD";
  ColorCodes["slateblue4"]           = "#473C8B";
  ColorCodes["slategray"]            = "#708090";
  ColorCodes["slategray1"]           = "#C6E2FF";
  ColorCodes["slategray2"]           = "#B9D3EE";
  ColorCodes["slategray3"]           = "#9FB6CD";
  ColorCodes["slategray4"]           = "#6C7B8B";
  ColorCodes["slategrey"]            = "#708090";
  ColorCodes["snow"]                 = "#FFFAFA";
  ColorCodes["snow1"]                = "#FFFAFA";
  ColorCodes["snow2"]                = "#EEE9E9";
  ColorCodes["snow3"]                = "#CDC9C9";
  ColorCodes["snow4"]                = "#8B8989";
  ColorCodes["springgreen"]          = "#00FF7F";
  ColorCodes["springgreen1"]         = "#00FF7F";
  ColorCodes["springgreen2"]         = "#00EE76";
  ColorCodes["springgreen3"]         = "#00CD66";
  ColorCodes["springgreen4"]         = "#008B45";
  ColorCodes["steelblue"]            = "#4682B4";
  ColorCodes["steelblue1"]           = "#63B8FF";
  ColorCodes["steelblue2"]           = "#5CACEE";
  ColorCodes["steelblue3"]           = "#4F94CD";
  ColorCodes["steelblue4"]           = "#36648B";
  ColorCodes["tan"]                  = "#D2B48C";
  ColorCodes["tan1"]                 = "#FFA54F";
  ColorCodes["tan2"]                 = "#EE9A49";
  ColorCodes["tan3"]                 = "#CD853F";
  ColorCodes["tan4"]                 = "#8B5A2B";
  ColorCodes["thistle"]              = "#D8BFD8";
  ColorCodes["thistle1"]             = "#FFE1FF";
  ColorCodes["thistle2"]             = "#EED2EE";
  ColorCodes["thistle3"]             = "#CDB5CD";
  ColorCodes["thistle4"]             = "#8B7B8B";
  ColorCodes["tomato"]               = "#FF6347";
  ColorCodes["tomato1"]              = "#FF6347";
  ColorCodes["tomato2"]              = "#EE5C42";
  ColorCodes["tomato3"]              = "#CD4F39";
  ColorCodes["tomato4"]              = "#8B3626";
  ColorCodes["turquoise"]            = "#40E0D0";
  ColorCodes["turquoise1"]           = "#00F5FF";
  ColorCodes["turquoise2"]           = "#00E5EE";
  ColorCodes["turquoise3"]           = "#00C5CD";
  ColorCodes["turquoise4"]           = "#00868B";
  ColorCodes["violet"]               = "#EE82EE";
  ColorCodes["violetred"]            = "#D02090";
  ColorCodes["violetred1"]           = "#FF3E96";
  ColorCodes["violetred2"]           = "#EE3A8C";
  ColorCodes["violetred3"]           = "#CD3278";
  ColorCodes["violetred4"]           = "#8B2252";
  ColorCodes["wheat"]                = "#F5DEB3";
  ColorCodes["wheat1"]               = "#FFE7BA";
  ColorCodes["wheat2"]               = "#EED8AE";
  ColorCodes["wheat3"]               = "#CDBA96";
  ColorCodes["wheat4"]               = "#8B7E66";
  ColorCodes["whitesmoke"]           = "#F5F5F5";
  ColorCodes["yellow"]               = "#FFFF00";
  ColorCodes["yellow1"]              = "#FFFF00";
  ColorCodes["yellow2"]              = "#EEEE00";
  ColorCodes["yellow3"]              = "#CDCD00";
  ColorCodes["yellow4"]              = "#8B8B00";
  ColorCodes["yellowgreen"]          = "#9ACD32";



  /* Accepted line styles */
  ValidLineStyles.setName("ValidLineStyles");
  ValidLineStyles.add("blank");
  ValidLineStyles.add("solid");
  ValidLineStyles.add("dashed");
  ValidLineStyles.add("dotted");
  ValidLineStyles.add("dotdash");
  ValidLineStyles.add("longdash");
  ValidLineStyles.add("twodash");

  /* Accepted fill styles */
  ValidFillStyles.setName("ValidFillStyles");
  ValidFillStyles.add("solid");
  ValidFillStyles.add("shaded");
}

/* Give the colors name */
Description DrawableImplementation::GetValidColors()
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  Description validColors;
  std::map<String, String>::const_iterator it(ColorCodes.begin());
  for (it = ColorCodes.begin(); it != ColorCodes.end(); ++it)
  {
    validColors.add(it->first);
  }
  return validColors;
}

/* Give the line style names */
Description DrawableImplementation::GetValidLineStyles()
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  return ValidLineStyles;
}

/* Give the fill style names */
Description DrawableImplementation::GetValidFillStyles()
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  return ValidFillStyles;
}

/* Give the point style names */
Description DrawableImplementation::GetValidPointStyles()
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  Description validPointStyle;
  std::map<String, UnsignedInteger>::const_iterator it(SymbolCodes.begin());
  for (it = SymbolCodes.begin(); it != SymbolCodes.end(); ++it) validPointStyle.add(it->first);
  return validPointStyle;
}


/* Convert a color name to  a valid hexadecimal code */
String DrawableImplementation::ConvertFromName(const String & name)
{
  if (IsValidColorCode(name)) return name;
  if (IsValidColorName(name)) return ColorCodes[name];
  LOGWARN(OSS() << "The given name=" << name << " is not a valid color name. Default to black.");
  return "#000000";
}

/* Convert an hexadecimal code into an RGB triplet */
Indices DrawableImplementation::ConvertToRGB(const String & key)
{
  Indices rgba;
  ScanColorCode(key, rgba);
  // Drop the alpha component
  rgba.resize(3);
  return rgba;
}

/* Convert an hexadecimal code into an RGBA quadruplet */
Indices DrawableImplementation::ConvertToRGBA(const String & key)
{
  Indices rgba;
  ScanColorCode(key, rgba);
  return rgba;
}

/* Convert an RGB triplet to a valid hexadecimal code */
String DrawableImplementation::ConvertFromRGB(const UnsignedInteger red,
    const UnsignedInteger green,
    const UnsignedInteger blue)
{
  return OSS() << "#"
         << std::hex << std::setw(2) << std::setfill('0') << std::right << std::min(red, 255UL)
         << std::hex << std::setw(2) << std::setfill('0') << std::right << std::min(green, 255UL)
         << std::hex << std::setw(2) << std::setfill('0') << std::right << std::min(blue, 255UL);
}

/* Convert an RGBA quadruplet to a valid hexadecimal code */
String DrawableImplementation::ConvertFromRGBA(const UnsignedInteger red,
    const UnsignedInteger green,
    const UnsignedInteger blue,
    const UnsignedInteger alpha)
{
  return OSS() << ConvertFromRGB(red, green, blue)
         << std::hex << std::setw(2) << std::setfill('0') << std::right << std::min(alpha, 255UL);
}

/* Convert an RGB triplet to a valid hexadecimal code */
String DrawableImplementation::ConvertFromRGB(const Scalar red,
    const Scalar green,
    const Scalar blue)
{
  return ConvertFromRGB(static_cast<UnsignedInteger>(round(255 * red)),
                        static_cast<UnsignedInteger>(round(255 * green)),
                        static_cast<UnsignedInteger>(round(255 * blue)));
}

/* Convert an RGBA quadruplet to a valid hexadecimal code */
String DrawableImplementation::ConvertFromRGBA(const Scalar red,
    const Scalar green,
    const Scalar blue,
    const Scalar alpha)
{
  return ConvertFromRGBA(static_cast<UnsignedInteger>(round(255 * red)),
                         static_cast<UnsignedInteger>(round(255 * green)),
                         static_cast<UnsignedInteger>(round(255 * blue)),
                         static_cast<UnsignedInteger>(round(255 * alpha)));
}

/* Convert an HSV triplet to a valid hexadecimal code */
Point DrawableImplementation::ConvertFromHSVIntoRGB(const Scalar hue,
    const Scalar saturation,
    const Scalar value)
{
  const UnsignedInteger h = static_cast<UnsignedInteger>(hue);
  const UnsignedInteger i = static_cast<UnsignedInteger>((h % 360 + hue - h) / 60.0) % 6;
  const Scalar f = hue / 60.0 - i;
  const Scalar l = value * (1.0 - saturation);
  const Scalar m = value * (1.0 - f * saturation);
  const Scalar n = value * (1.0 - (1.0 - f) * saturation);
  Point redGreenBlue(3);
  switch (i)
  {
    case 0:
      redGreenBlue[0] = value;
      redGreenBlue[1] = n;
      redGreenBlue[2] = l;
      break;
    case 1:
      redGreenBlue[0] = m;
      redGreenBlue[1] = value;
      redGreenBlue[2] = l;
      break;
    case 2:
      redGreenBlue[0] = l;
      redGreenBlue[1] = value;
      redGreenBlue[2] = n;
      break;
    case 3:
      redGreenBlue[0] = l;
      redGreenBlue[1] = m;
      redGreenBlue[2] = value;
      break;
    case 4:
      redGreenBlue[0] = n;
      redGreenBlue[1] = l;
      redGreenBlue[2] = value;
      break;
    case 5:
      redGreenBlue[0] = value;
      redGreenBlue[1] = l;
      redGreenBlue[2] = m;
      break;
    default:
      throw InternalException(HERE) << "Error: bad Hue index=" << i << ", should be in {0,...,5}";
  }
  return redGreenBlue;
}

/* Convert an HSV triplet to a valid hexadecimal code */
String DrawableImplementation::ConvertFromHSV(const Scalar hue,
    const Scalar saturation,
    const Scalar value)
{
  const Point redGreenBlue(ConvertFromHSVIntoRGB(hue, saturation, value));
  return ConvertFromRGB(redGreenBlue[0], redGreenBlue[1], redGreenBlue[2]);
}

/* Convert an HSVA quadruplet to a valid hexadecimal code */
String DrawableImplementation::ConvertFromHSVA(const Scalar hue,
    const Scalar saturation,
    const Scalar value,
    const Scalar alpha)
{
  const Point redGreenBlue(ConvertFromHSVIntoRGB(hue, saturation, value));
  return ConvertFromRGBA(redGreenBlue[0], redGreenBlue[1], redGreenBlue[2], alpha);
}

/* Default constructor */
DrawableImplementation::DrawableImplementation()
  : PersistentObject()
  , legend_("")
  , data_()
  , color_(ResourceMap::GetAsString("Drawable-DefaultColor"))
  , fillStyle_(ResourceMap::GetAsString("Drawable-DefaultFillStyle"))
  , lineStyle_(ResourceMap::GetAsString("Drawable-DefaultLineStyle"))
  , pointStyle_(ResourceMap::GetAsString("Drawable-DefaultPointStyle"))
  , lineWidth_(ResourceMap::GetAsScalar("Drawable-DefaultLineWidth"))
  , dataFileName_("")
{
  // Nothing to do
}

/* Default constructor */
DrawableImplementation::DrawableImplementation(const Sample & data,
    const String & legend)
  : PersistentObject(),
    legend_(legend),
    data_(data),
    color_(ResourceMap::GetAsString("Drawable-DefaultColor")),
    fillStyle_(ResourceMap::GetAsString("Drawable-DefaultFillStyle")),
    lineStyle_(ResourceMap::GetAsString("Drawable-DefaultLineStyle")),
    pointStyle_(ResourceMap::GetAsString("Drawable-DefaultPointStyle")),
    lineWidth_(ResourceMap::GetAsScalar("Drawable-DefaultLineWidth")),
    dataFileName_("")
{
  setName(legend);
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
}

/* Clone method */
DrawableImplementation * DrawableImplementation::clone() const
{
  return new DrawableImplementation(*this);
}

/* String converter */
String DrawableImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DrawableImplementation::GetClassName()
      << " name=" << getName()
      << " legend=" << legend_
      << " data=" << data_
      << " color=" << color_
      << " fillStyle=" << fillStyle_
      << " lineStyle=" << lineStyle_
      << " pointStyle=" << pointStyle_
      << " lineWidth=" << lineWidth_;
  return oss;
}

String DrawableImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName()
      << "(name=" << getName()
      << ", color=" << color_
      << ", fill=" << fillStyle_
      << ", line=" << lineStyle_
      << ", point=" << pointStyle_
      << ", width=" << lineWidth_
      << ", data=" << Os::GetEndOfLine() << offset << data_.__str__(offset);
  return oss;
}

/* Comparison operator */
Bool DrawableImplementation::operator == (const DrawableImplementation & ) const
{
  return false;
}

/* Legend name accessor */
String DrawableImplementation::getLegend() const
{
  return legend_;
}

void DrawableImplementation::setLegend(const String & legend)
{
  legend_ = legend;
}

/* Point code accessor */
UnsignedInteger DrawableImplementation::getPointCode(const String & key) const
{
  const std::map<String, UnsignedInteger>::const_iterator it(SymbolCodes.find(key));
  UnsignedInteger pointCode = 0;
  if(it != SymbolCodes.end()) pointCode = it->second;

  return pointCode;
}

/* Check validity of color */
Bool DrawableImplementation::IsValidColorName(const String & key)
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  const std::map<String, String>::iterator it(ColorCodes.find(key));

  return (it != ColorCodes.end());
}

void DrawableImplementation::ScanColorCode(const String & key,
    Indices & rgba)
{
  rgba = Indices(4, 0);
  // First, check if the color is given in RGB format
  const UnsignedInteger keySize = key.size();
  if (keySize == 0) throw InvalidArgumentException(HERE) << "Empty hex color";
  // Check if it is a #RRGGBB[AA] rgba
  if (key[0] != '#') throw InvalidArgumentException(HERE) << "Hex color should start with #";
  // First, check the key length:
  // 7 for #RRGGBB
  // 9 for #RRGGBBAA
  if ((keySize != 7) && (keySize != 9)) throw InvalidArgumentException(HERE) << "Hex color not in #RRGGBB nor #RRGGBBAA format";
  // Second, check that the values are ok
  for (UnsignedInteger i = 0; i < keySize - 1; ++i)
  {
    const UnsignedInteger shift(i % 2 == 0 ? 16 : 1);
    const char c(key[i + 1]);
    // If the current character is not a valid hexadecimal figure
    const Bool isNum = (c >= '0') && (c <= '9');
    const Bool isValidLower = (c >= 'a') && (c <= 'f');
    const Bool isValidUpper = (c >= 'A') && (c <= 'F');
    if ((!isNum) && !(isValidLower) && !(isValidUpper)) throw InvalidArgumentException(HERE) << "Invalid character in hex color";
    if (isNum)        rgba[i / 2] += (c - '0'     ) * shift;
    if (isValidLower) rgba[i / 2] += (c - 'a' + 10) * shift;
    if (isValidUpper) rgba[i / 2] += (c - 'A' + 10) * shift;
  }
  // If no A value, set it to 255
  if (keySize == 7) rgba[3] = 255;
}

Bool DrawableImplementation::IsValidColorCode(const String & key)
{
  Indices rgba;
  try
  {
    ScanColorCode(key, rgba);
    return true;
  }
  catch (InvalidArgumentException &)
  {
    return false;
  }
}

Bool DrawableImplementation::IsValidColor(const String & key)
{
  if (IsValidColorCode(key)) return true;
  return IsValidColorName(key);
}

/* Check validity of line style */
Bool DrawableImplementation::IsValidLineStyle(const String & key)
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  const Description::const_iterator it = std::find(ValidLineStyles.begin(), ValidLineStyles.end(), key);

  return (it != ValidLineStyles.end());
}

/* Check validity of point style */
Bool DrawableImplementation::IsValidPointStyle(const String & key)
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  const std::map<String, UnsignedInteger>::iterator it(SymbolCodes.find(key));

  return (it != SymbolCodes.end());
}

/* Check validity of fill style */
Bool DrawableImplementation::IsValidFillStyle(const String & key)
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  const Description::const_iterator it = std::find(ValidFillStyles.begin(), ValidFillStyles.end(), key);

  return (it != ValidFillStyles.end());
}

/* Check validity of pattern parameter */
Bool DrawableImplementation::IsValidPattern(const String & pattern)
{
  if(IsFirstInitialization)
  {
    InitializeValidParameterList();
    IsFirstInitialization = false;
  }
  return (pattern == ResourceMap::GetAsString("Drawable-DefaultPattern") || pattern == ResourceMap::GetAsString("Drawable-AlternativePattern"));
}

/* Check validity of the data */
void DrawableImplementation::checkData(const Sample & ) const
{
  throw NotYetImplementedException(HERE) << "DrawableImplementation::checkData(const Sample & data) const";
}

void DrawableImplementation::checkData(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DrawableImplementation::checkData(const Point & data) const";
}

/* Data Accessor */
Sample DrawableImplementation::getData() const
{
  return data_;
}

/* Data accessor */
void DrawableImplementation::setData(const Sample & data)
{
  checkData(data);
  data_ = data;
}

void DrawableImplementation::setData(const Point & data)
{
  checkData(data);
  const UnsignedInteger size = data.getDimension();
  data_ = Sample(size, 1);
  data_.getImplementation()->setData(data);
}

/* Bounding box accessor */
Interval DrawableImplementation::getBoundingBox() const
{
  Point min(data_.getMin());
  Point max(data_.getMax());
  return Interval(min, max);
}

/* Accessor for color */
String DrawableImplementation::getColor() const
{
  return color_;
}

String DrawableImplementation::getColorCode() const
{
  return ConvertFromName(color_);
}

/* Accessor for color */
void DrawableImplementation::setColor(const String & color)
{
  if(!IsValidColor(color)) throw InvalidArgumentException(HERE) << "Given color = " << color << " is incorrect";

  color_ = color;
}

/* Accessor for edge color */
String DrawableImplementation::getEdgeColor() const
{
  throw NotDefinedException(HERE) << "Error: no edge color in " << getClassName();
}

/* Accessor for line style */
String DrawableImplementation::getLineStyle() const
{
  return lineStyle_;
}

/* Accesor for line style */
void DrawableImplementation::setLineStyle(const String & lineStyle)
{
  if(!IsValidLineStyle(lineStyle))  throw InvalidArgumentException(HERE) << "Given line style=" << lineStyle << " is incorrect";
  lineStyle_ = lineStyle;
}

/* Accessor for fill style */
String DrawableImplementation::getFillStyle() const
{
  return fillStyle_;
}

/* Accessor for fill style */
void DrawableImplementation::setFillStyle(const String & fillStyle)
{
  if(!IsValidFillStyle(fillStyle)) throw InvalidArgumentException(HERE) << "Given fill style=" << fillStyle << " is incorrect";

  fillStyle_ = fillStyle;
}

/* Accessor for point style */
String DrawableImplementation::getPointStyle() const
{
  return pointStyle_;
}

/* Accessor for point style */
void DrawableImplementation::setPointStyle(const String & pointStyle)
{
  if(!IsValidPointStyle(pointStyle)) throw InvalidArgumentException(HERE) << "Given point symbol=" << pointStyle << " is incorrect";
  pointStyle_ = pointStyle;
}

/* Accessor for line width */
Scalar DrawableImplementation::getLineWidth() const
{
  return lineWidth_;
}

/* Accessor for line width */
void DrawableImplementation::setLineWidth(const Scalar lineWidth)
{
  if (!(lineWidth > 0.0)) throw InvalidArgumentException(HERE) << "Given line width=" << lineWidth << " is incorrect";
  lineWidth_ = lineWidth;
}

/* Accessor for pattern */
String DrawableImplementation::getPattern() const
{
  throw NotDefinedException(HERE) << "Error: no pattern in " << getClassName();
}

void DrawableImplementation::setPattern(const String )
{
  throw NotDefinedException(HERE) << "Error: no pattern in " << getClassName();
}

/* Accessor for center */
Point DrawableImplementation::getCenter() const
{
  throw NotDefinedException(HERE) << "Error: no center in " << getClassName();
}

void DrawableImplementation::setCenter(const Point & )
{
  throw NotDefinedException(HERE) << "Error: no center in " << getClassName();
}

/* Accessor for radius */
Scalar DrawableImplementation::getRadius() const
{
  throw NotDefinedException(HERE) << "Error: no radius in " << getClassName();
}

void DrawableImplementation::setRadius(const Scalar )
{
  throw NotDefinedException(HERE) << "Error: no radius in " << getClassName();
}

/* Accessor for labels */
Description DrawableImplementation::getLabels() const
{
  throw NotDefinedException(HERE) << "Error: no labels in " << getClassName();
}

void DrawableImplementation::setLabels(const Description & )
{
  throw NotDefinedException(HERE) << "Error: no labels in " << getClassName();
}

/* Accessor for color palette */
Description DrawableImplementation::getPalette() const
{
  throw NotDefinedException(HERE) << "Error: no palette in " << getClassName();
}

void DrawableImplementation::setPalette(const Description & )
{
  throw NotDefinedException(HERE) << "Error: no palette in " << getClassName();
}

Sample DrawableImplementation::getPaletteAsNormalizedRGBA() const
{
  const Description palette(getPalette());
  const UnsignedInteger size = palette.getSize();
  Sample normalizedRGBA(size, 4);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const String hexCode(ConvertFromName(palette[i]));
    const Indices rgba(ConvertToRGBA(hexCode));
    normalizedRGBA(i, 0) = rgba[0] / 255.0;
    normalizedRGBA(i, 1) = rgba[1] / 255.0;
    normalizedRGBA(i, 2) = rgba[2] / 255.0;
    normalizedRGBA(i, 3) = rgba[3] / 255.0;
  }
  return normalizedRGBA;
}

/* Accessor for origin */
Scalar DrawableImplementation::getOrigin() const
{
  throw NotDefinedException(HERE) << "Error: no origin in " << getClassName();
}

void DrawableImplementation::setOrigin(const Scalar )
{
  throw NotDefinedException(HERE) << "Error: no origin in " << getClassName();
}

/* Accessor for first coordinate */
Sample DrawableImplementation::getX() const
{
  throw NotDefinedException(HERE) << "Error: no sample X in " << getClassName();
}

void DrawableImplementation::setX(const Sample & )
{
  throw NotDefinedException(HERE) << "Error: no sample X in " << getClassName();
}

/* Accessor for second coordinate */
Sample DrawableImplementation::getY() const
{
  throw NotDefinedException(HERE) << "Error: no sample Y in " << getClassName();
}

void DrawableImplementation::setY(const Sample & )
{
  throw NotDefinedException(HERE) << "Error: no sample Y in " << getClassName();
}

/* Accessor for levels */
Point DrawableImplementation::getLevels() const
{
  throw NotDefinedException(HERE) << "Error: no levels in " << getClassName();
}

void DrawableImplementation::setLevels(const Point & )
{
  throw NotDefinedException(HERE) << "Error: no levels in " << getClassName();
}

/* Accessor for drawLabels */
Bool DrawableImplementation::getDrawLabels() const
{
  throw NotDefinedException(HERE) << "Error: no flag drawLabels in " << getClassName();
}

void DrawableImplementation::setDrawLabels(const Bool & )
{
  throw NotDefinedException(HERE) << "Error: no flag drawLabels in " << getClassName();
}

/* Accessor for textAnnotations */
Description DrawableImplementation::getTextAnnotations() const
{
  throw NotDefinedException(HERE) << "Error: no flag textAnnotations in " << getClassName();
}

void DrawableImplementation::setTextAnnotations(const Description & )
{
  throw NotDefinedException(HERE) << "Error: no flag textAnnotations in " << getClassName();
}

/* Accessor for textPositions */
Description DrawableImplementation::getTextPositions() const
{
  throw NotDefinedException(HERE) << "Error: no flag textPositions in " << getClassName();
}

void DrawableImplementation::setTextPositions(const Description & )
{
  throw NotDefinedException(HERE) << "Error: no flag textPositions in " << getClassName();
}

Scalar DrawableImplementation::getTextSize() const
{
  throw NotDefinedException(HERE) << "Error: no text size in " << getClassName();
}

void DrawableImplementation::setTextSize(const Scalar /*size*/)
{
  throw NotDefinedException(HERE) << "Error: no text size in " << getClassName();
}


/* R command generating method, for plotting through R */
String DrawableImplementation::draw() const
{
  const UnsignedInteger size = data_.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: trying to build a Drawable with empty data";
  // Two strategies: if data is small, it is inlined, else it is passed through a file
  const UnsignedInteger dimension = data_.getDimension();
  dataFileName_ = "";
  if (size * dimension > ResourceMap::GetAsUnsignedInteger("Drawable-DataThreshold"))
  {
    dataFileName_ = data_.storeToTemporaryFile();
    return OSS() << "dataOT <- data.matrix(read.table(\"" << dataFileName_ << "\", stringsAsFactors = F))" << "\n";
  }
  return OSS().setPrecision(20) << "dataOT <- " << data_.streamToRFormat() << "\n";
}

/* Clean all the temporary data created by draw() method */
void DrawableImplementation::clean() const
{
  if (dataFileName_ != "") Os::Remove(dataFileName_);
}

/* Build default palette
   Cycle through the hue wheel with 10 nuances and increasing darkness */
Description DrawableImplementation::BuildDefaultPalette(const UnsignedInteger size)
{
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the size must be > 0";
  Description palette(size);
  UnsignedInteger phase(ResourceMap::GetAsUnsignedInteger("Drawable-DefaultPalettePhase"));
  if (phase == 0) phase = 1;
  const UnsignedInteger divider = std::min(size + 1, phase);
  const Scalar multiplier = 360.0 / divider;
  const UnsignedInteger cycles = size / divider + 1;
  UnsignedInteger paletteIndex = 0;
  for (UnsignedInteger iCycle = 0; iCycle < cycles; ++iCycle)
  {
    const volatile Scalar p = iCycle / static_cast< Scalar >(cycles);
    const Scalar value = 1.0 - p;
    const UnsignedInteger iHueMax = std::min(size - paletteIndex, phase);
    for (UnsignedInteger iHue = 0; iHue < iHueMax; ++iHue)
    {
      const Scalar hue = multiplier * iHue;
      palette[paletteIndex] = ConvertFromHSV(hue, 1.0, value);
      ++paletteIndex;
    }
  }
  return palette;
}

/* Method save() stores the object through the StorageManager */
void DrawableImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "legend_", legend_ );
  adv.saveAttribute( "data_", data_ );
  adv.saveAttribute( "color_", color_ );
  adv.saveAttribute( "fillStyle_", fillStyle_ );
  adv.saveAttribute( "lineStyle_", lineStyle_ );
  adv.saveAttribute( "pointStyle_", pointStyle_ );
  adv.saveAttribute( "lineWidth_", lineWidth_ );
}

/* Method load() reloads the object from the StorageManager */
void DrawableImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "legend_", legend_ );
  adv.loadAttribute( "data_", data_ );
  adv.loadAttribute( "color_", color_ );
  adv.loadAttribute( "fillStyle_", fillStyle_ );
  adv.loadAttribute( "lineStyle_", lineStyle_ );
  adv.loadAttribute( "pointStyle_", pointStyle_ );
  adv.loadAttribute( "lineWidth_", lineWidth_ );
}



END_NAMESPACE_OPENTURNS
