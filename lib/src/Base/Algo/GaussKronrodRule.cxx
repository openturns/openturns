//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GaussKronrodRule.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussKronrodRule
 */

CLASSNAMEINIT(GaussKronrodRule)

static const Factory<GaussKronrodRule> Factory_GaussKronrodRule;

/* Constructor without parameters */
GaussKronrodRule::GaussKronrodRule()
  : GaussKronrodRule(G11K23)
{
  // Nothing to do
}

/* Parameters constructor */
GaussKronrodRule::GaussKronrodRule(const GaussKronrodPair pair)
  : PersistentObject()
  , order_(0)
  , zeroGaussWeight_(0.0)
  , otherGaussWeights_(0)
  , otherKronrodNodes_(0)
  , zeroKronrodWeight_(0.0)
  , otherKronrodWeights_(0)
{
  // Select the parameters associated with the given pair
  switch (pair)
  {
    case G1K3:
      /*
       * Low order Gauss-Kronrod integration rule
       */
      // Order
      order_ = 1;
      // Central Gauss weight
      zeroGaussWeight_ = 2.0;
      // Gauss/Kronrod nodes
      otherKronrodNodes_.add(0.77459666924148337704);
      // Central Gauss/Kronrod weight, half of the central Gauss weight
      zeroKronrodWeight_ = 0.888888888888888888889;
      // Other Gauss/Kronrod weights
      otherKronrodWeights_.add(0.555555555555555555556);
      break;

    case G3K7:
      /*
       * Low order Gauss-Kronrod integration rule
       */
      // Order
      order_ = 3;
      // Central Gauss weight
      zeroGaussWeight_ = 8.0 / 9.0;
      // Symmetric Gauss weights
      otherGaussWeights_.add(5.0 / 9.0);
      // Gauss/Kronrod nodes
      otherKronrodNodes_.add(0.96049126870802028342);
      otherKronrodNodes_.add(0.77459666924148337704);
      otherKronrodNodes_.add(0.43424374934680255800);
      // Central Gauss/Kronrod weight, half of the central Gauss weight
      zeroKronrodWeight_ = 0.450916538658474142345;
      // Other Gauss/Kronrod weights
      otherKronrodWeights_.add(0.104656226026467265194);
      otherKronrodWeights_.add(0.268488089868333440729);
      otherKronrodWeights_.add(0.401397414775962222905);
      break;

    case G7K15:
      /*
       * Medium order Gauss-Kronrod integration rule
       */
      // Order
      order_ = 7;
      // Central Gauss weight
      zeroGaussWeight_ = 0.4179591836734693877551020;
      // Symmetric Gauss weights
      otherGaussWeights_.add(0.1294849661688696932706114);
      otherGaussWeights_.add(0.2797053914892766679014678);
      otherGaussWeights_.add(0.3818300505051189449503698);
      // Gauss/Kronrod nodes
      otherKronrodNodes_.add(0.9914553711208126392068547);
      otherKronrodNodes_.add(0.9491079123427585245261897);
      otherKronrodNodes_.add(0.8648644233597690727897128);
      otherKronrodNodes_.add(0.7415311855993944398638648);
      otherKronrodNodes_.add(0.5860872354676911302941448);
      otherKronrodNodes_.add(0.4058451513773971669066064);
      otherKronrodNodes_.add(0.2077849550078984676006894);
      // Central Gauss/Kronrod weight);
      zeroKronrodWeight_ = 0.2094821410847278280129992;
      // Other Gauss/Kronrod weights
      otherKronrodWeights_.add(0.0229353220105292249637320);
      otherKronrodWeights_.add(0.0630920926299785532907007);
      otherKronrodWeights_.add(0.1047900103222501838398763);
      otherKronrodWeights_.add(0.1406532597155259187451896);
      otherKronrodWeights_.add(0.1690047266392679028265834);
      otherKronrodWeights_.add(0.1903505780647854099132564);
      otherKronrodWeights_.add(0.2044329400752988924141620);
      break;

    case G11K23:
      /*
       * High order Gauss-Kronrod integration rule
       */
      // Order
      order_ = 11;
      // Central Gauss weight
      zeroGaussWeight_ = 0.2729250867779007;
      // Symmetric Gauss weights
      otherGaussWeights_.add(0.05566856711617449);
      otherGaussWeights_.add(0.1255803694649048);
      otherGaussWeights_.add(0.1862902109277352);
      otherGaussWeights_.add(0.2331937645919914);
      otherGaussWeights_.add(0.2628045445102478);
      // Gauss/Kronrod nodes
      otherKronrodNodes_.add(0.9963696138895427);
      otherKronrodNodes_.add(0.9782286581460570);
      otherKronrodNodes_.add(0.9416771085780681);
      otherKronrodNodes_.add(0.8870625997680953);
      otherKronrodNodes_.add(0.8160574566562211);
      otherKronrodNodes_.add(0.7301520055740492);
      otherKronrodNodes_.add(0.6305995201619651);
      otherKronrodNodes_.add(0.5190961292068118);
      otherKronrodNodes_.add(0.3979441409523776);
      otherKronrodNodes_.add(0.2695431559523450);
      otherKronrodNodes_.add(0.1361130007993617);
      // Central Gauss/Kronrod weight);
      zeroKronrodWeight_ = 0.1365777947111183;
      // Other Gauss/Kronrod weights
      otherKronrodWeights_.add(0.00976544104596129);
      otherKronrodWeights_.add(0.02715655468210443);
      otherKronrodWeights_.add(0.04582937856442671);
      otherKronrodWeights_.add(0.06309742475037484);
      otherKronrodWeights_.add(0.07866457193222764);
      otherKronrodWeights_.add(0.09295309859690074);
      otherKronrodWeights_.add(0.1058720744813894);
      otherKronrodWeights_.add(0.1167395024610472);
      otherKronrodWeights_.add(0.1251587991003195);
      otherKronrodWeights_.add(0.1312806842298057);
      otherKronrodWeights_.add(0.1351935727998845);
      break;

    case G15K31:
      /*
       * High order Gauss-Kronrod integration rule
       */
      // Order
      order_ = 15;
      // Central Gauss weight
      zeroGaussWeight_ = 0.2025782419255612728806202;
      // Symmetric Gauss weights
      otherGaussWeights_.add(0.0307532419961172683546284);
      otherGaussWeights_.add(0.0703660474881081247092674);
      otherGaussWeights_.add(0.1071592204671719350118695);
      otherGaussWeights_.add(0.1395706779261543144478048);
      otherGaussWeights_.add(0.1662692058169939335532009);
      otherGaussWeights_.add(0.1861610000155622110268006);
      otherGaussWeights_.add(0.1984314853271115764561183);
      // Gauss/Kronrod nodes
      otherKronrodNodes_.add(0.9980022986933970602851728);
      otherKronrodNodes_.add(0.9879925180204854284895657);
      otherKronrodNodes_.add(0.9677390756791391342573480);
      otherKronrodNodes_.add(0.9372733924007059043077589);
      otherKronrodNodes_.add(0.8972645323440819008825097);
      otherKronrodNodes_.add(0.8482065834104272162006483);
      otherKronrodNodes_.add(0.7904185014424659329676493);
      otherKronrodNodes_.add(0.7244177313601700474161861);
      otherKronrodNodes_.add(0.6509967412974169705337359);
      otherKronrodNodes_.add(0.5709721726085388475372267);
      otherKronrodNodes_.add(0.4850818636402396806936557);
      otherKronrodNodes_.add(0.3941513470775633698972074);
      otherKronrodNodes_.add(0.2991800071531688121667800);
      otherKronrodNodes_.add(0.2011940939974345223006283);
      otherKronrodNodes_.add(0.1011420669187174990270742);
      // Central Gauss/Kronrod weight);
      zeroKronrodWeight_ = 0.1013300070147915490173748;
      // Other Gauss/Kronrod weights
      otherKronrodWeights_.add(0.0053774798729233489877921);
      otherKronrodWeights_.add(0.0150079473293161225383748);
      otherKronrodWeights_.add(0.0254608473267153201868740);
      otherKronrodWeights_.add(0.0353463607913758462220379);
      otherKronrodWeights_.add(0.0445897513247648766082273);
      otherKronrodWeights_.add(0.0534815246909280872653431);
      otherKronrodWeights_.add(0.0620095678006706402851392);
      otherKronrodWeights_.add(0.0698541213187282587095201);
      otherKronrodWeights_.add(0.0768496807577203788944328);
      otherKronrodWeights_.add(0.0830805028231330210382892);
      otherKronrodWeights_.add(0.0885644430562117706472754);
      otherKronrodWeights_.add(0.0931265981708253212254869);
      otherKronrodWeights_.add(0.0966427269836236785051799);
      otherKronrodWeights_.add(0.0991735987217919593323932);
      otherKronrodWeights_.add(0.1007698455238755950449467);
      break;

    case G25K51:
      /*
       * High order Gauss-Kronrod integration rule
       */
      // Order
      order_ = 25;
      // Central Gauss weight
      zeroGaussWeight_ = 0.1231760537267154512039029;
      // Symmetric Gauss weights
      otherGaussWeights_.add(0.1222424429903100416889595);
      otherGaussWeights_.add(0.1194557635357847722281781);
      otherGaussWeights_.add(0.1148582591457116483393255);
      otherGaussWeights_.add(0.1085196244742636531160940);
      otherGaussWeights_.add(0.1005359490670506442022069);
      otherGaussWeights_.add(0.0910282619829636498114972);
      otherGaussWeights_.add(0.0801407003350010180132350);
      otherGaussWeights_.add(0.0680383338123569172071872);
      otherGaussWeights_.add(0.0549046959758351919259369);
      otherGaussWeights_.add(0.0409391567013063126556235);
      otherGaussWeights_.add(0.0263549866150321372619018);
      otherGaussWeights_.add(0.1222424429903100416889595);
      // Gauss/Kronrod nodes
      otherKronrodNodes_.add(0.9992621049926098341934575);
      otherKronrodNodes_.add(0.9955569697904980979087849);
      otherKronrodNodes_.add(0.9880357945340772476373310);
      otherKronrodNodes_.add(0.9766639214595175114983154);
      otherKronrodNodes_.add(0.9616149864258425124181300);
      otherKronrodNodes_.add(0.9429745712289743394140112);
      otherKronrodNodes_.add(0.9207471152817015617463461);
      otherKronrodNodes_.add(0.8949919978782753688510420);
      otherKronrodNodes_.add(0.8658470652932755954489970);
      otherKronrodNodes_.add(0.8334426287608340014210211);
      otherKronrodNodes_.add(0.7978737979985000594104109);
      otherKronrodNodes_.add(0.7592592630373576305772829);
      otherKronrodNodes_.add(0.7177664068130843881866541);
      otherKronrodNodes_.add(0.6735663684734683644851206);
      otherKronrodNodes_.add(0.6268100990103174127881227);
      otherKronrodNodes_.add(0.5776629302412229677236898);
      otherKronrodNodes_.add(0.5263252843347191825996238);
      otherKronrodNodes_.add(0.4730027314457149605221821);
      otherKronrodNodes_.add(0.4178853821930377488518144);
      otherKronrodNodes_.add(0.3611723058093878377358217);
      otherKronrodNodes_.add(0.3030895389311078301674789);
      otherKronrodNodes_.add(0.2438668837209884320451904);
      otherKronrodNodes_.add(0.1837189394210488920159699);
      otherKronrodNodes_.add(0.1228646926107103963873598);
      otherKronrodNodes_.add(0.0615444830056850788865464);
      // Central Gauss/Kronrod weight);
      zeroKronrodWeight_ = 0.0615808180678329350787598;
      // Other Gauss/Kronrod weights
      otherKronrodWeights_.add(0.0019873838923303159265079);
      otherKronrodWeights_.add(0.0055619321353567137580402);
      otherKronrodWeights_.add(0.0094739733861741516072077);
      otherKronrodWeights_.add(0.0132362291955716748136564);
      otherKronrodWeights_.add(0.0168478177091282982315167);
      otherKronrodWeights_.add(0.0204353711458828354565683);
      otherKronrodWeights_.add(0.0240099456069532162200925);
      otherKronrodWeights_.add(0.0274753175878517378029485);
      otherKronrodWeights_.add(0.0307923001673874888911090);
      otherKronrodWeights_.add(0.0340021302743293378367488);
      otherKronrodWeights_.add(0.0371162714834155435603306);
      otherKronrodWeights_.add(0.0400838255040323820748393);
      otherKronrodWeights_.add(0.0428728450201700494768958);
      otherKronrodWeights_.add(0.0455029130499217889098706);
      otherKronrodWeights_.add(0.0479825371388367139063923);
      otherKronrodWeights_.add(0.0502776790807156719633253);
      otherKronrodWeights_.add(0.0523628858064074758643667);
      otherKronrodWeights_.add(0.0542511298885454901445434);
      otherKronrodWeights_.add(0.0559508112204123173082407);
      otherKronrodWeights_.add(0.0574371163615678328535827);
      otherKronrodWeights_.add(0.0586896800223942079619742);
      otherKronrodWeights_.add(0.0597203403241740599790993);
      otherKronrodWeights_.add(0.0605394553760458629453603);
      otherKronrodWeights_.add(0.0611285097170530483058590);
      otherKronrodWeights_.add(0.0614711898714253166615441);
      break;

    default:
      throw InvalidArgumentException(HERE) << "Invalid pair argument";

  } /* end switch */
}

/* Virtual constructor */
GaussKronrodRule * GaussKronrodRule::clone() const
{
  return new GaussKronrodRule(*this);
}

/* Pair accessor */
GaussKronrodRule::GaussKronrodPair GaussKronrodRule::getPair() const
{
  return pair_;
}

/* Order accessor */
UnsignedInteger GaussKronrodRule::getOrder() const
{
  return order_;
}

/* Zero Gauss weight accessor */
Scalar GaussKronrodRule::getZeroGaussWeight() const
{
  return zeroGaussWeight_;
}

/* Other Gauss weight accessor */
Point GaussKronrodRule::getOtherGaussWeights() const
{
  return otherGaussWeights_;
}

/* Other Kronrod nodes accessor */
Point GaussKronrodRule::getOtherKronrodNodes() const
{
  return otherKronrodNodes_;
}

/* Zero Kronrod weight accessor */
Scalar GaussKronrodRule::getZeroKronrodWeight() const
{
  return zeroKronrodWeight_;
}

/* Other Kronrod weight accessor */
Point GaussKronrodRule::getOtherKronrodWeights() const
{
  return otherKronrodWeights_;
}

/* String converter */
String GaussKronrodRule::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GaussKronrodRule::GetClassName()
      << ", pair=" << "G" << order_ << "K" << 2 * order_ + 1 << ")";
  return oss;
}

/* String converter */
String GaussKronrodRule::__str__(const String & ) const
{
  OSS oss(false);
  oss << GaussKronrodRule::GetClassName()
      << "(pair=G" << order_ << "K" << 2 * order_ + 1 << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS
