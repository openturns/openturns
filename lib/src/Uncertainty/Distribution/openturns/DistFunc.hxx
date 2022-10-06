//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
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
#ifndef OPENTURNS_DISTFUNC_HXX
#define OPENTURNS_DISTFUNC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace DistFunc
{
const UnsignedInteger    NumberOfBandNormalZigurrat = 129;
const Scalar NormalZigguratTail = 3.44508288805539135654449538289;
const Scalar NormalZigguratAbscissa[NumberOfBandNormalZigurrat + 1] =
{
  0.0, 0.271599048510693754998163050702, 0.361898145612134821317157669025, 0.425392047157312675983415424660,
  0.476131717888617028166988513439, 0.519219013293501476350033678035, 0.557137093511580298809061311694, 0.591298984443313319787098187809,
  0.622592649542783642315521009315, 0.651616031063230052452116746597, 0.678792877347767760686380971868, 0.704435546364376654764501920124,
  0.728781608504968321114578996184, 0.752016422469955992826848889422, 0.774287710877425340184686032696, 0.795715332287609618751456615712,
  0.816398043479172731605919440849, 0.836418306930649053279379059005, 0.855845789056994122927551923075, 0.874739957924955634206040955649,
  0.893152046999867275098199151897, 0.911126563328297873672484801050, 0.928702462334730770821130245805, 0.945914074632515687029155431559,
  0.962791845646485921191452370469, 0.979362932051085821966553188665, 0.995651687354039595085786832964, 1.01168006070531516232965392900,
  1.02746792709172237058780318928, 1.04303336277130264040166296783, 1.05839287662899292124745211169, 1.07356160576994269038108408924,
  1.08855348188453823734559909469, 1.10338137356245140332933074704, 1.11805720869053844255517390583, 1.13259208026135637042889232625,
  1.14699633828758735801507046149, 1.16127967002039429336339539399, 1.17545117027533417287473048372, 1.18951940335453972086012397336,
  1.20349245780083403835096451764, 1.21737799501490321160206827336, 1.23118329260039645151126944958, 1.24491528316597898248595200283,
  1.25858058920182308169327058845, 1.27218555455602081689118701416, 1.28573627296019479845973032796, 1.29923861399021398514811910954,
  1.31269824679504233415845967597, 1.32612066188248490208347357933, 1.33951119121344617033356972221, 1.35287502682506865937549704692,
  1.36621723817679783092212387778, 1.37954278839122803847614338568, 1.39285654954287803582443862287, 1.40616331713229672637906532114,
  1.41946782386968059570797384556, 1.43277475288114273351645465423, 1.44608875044162355949759042785, 1.45941443833094318522780953546,
  1.47275642590347769759399918804, 1.48611932195724762500809972081, 1.49950774648472030334842211111, 1.51292634238526126953827654531,
  1.52637978721786115033208986070, 1.53987280507247501439918234146, 1.55341017863902384055596007811, 1.56699676155482666996780634172,
  1.58063749111398211301308134969, 1.59433740142604557618725800299, 1.60810163711632290631602733261, 1.62193546766631570500142829940,
  1.63584430250042915164619942820, 1.64983370693414137939082821110, 1.66390941910962125167951454089, 1.67807736805749698528091063361,
  1.69234369303839820145325283038, 1.70671476433535309787605152682, 1.72119720568852405198043944381, 1.73579791858759592141174442291,
  1.75052410866497944355179477982, 1.76538331446556845629758885967, 1.78038343890695608962355720514, 1.79553278378881937566665179110,
  1.81084008776290159625982126988, 1.82631456823722069305589567937, 1.84196596776173086857556181334, 1.85780460553003206309142077930,
  1.87384143473579289060235282188, 1.89008810664697886815311451817, 1.90655704241032628459974673316, 1.92326151377851307313333422169,
  1.94021573417040550238786161927, 1.95743496173982213662503222153, 1.97493561645224006631535036596, 1.99273541356693511100128687843,
  2.01085351641383291996867861457, 2.02931071196545767371544926885, 2.04812961346843841880165860053, 2.06733489536060750602383910558,
  2.08695356691825680949271767439, 2.10701529263367409420415792810, 2.12755276932450072216751946158, 2.14860217257314763105233505342,
  2.17020368849378343938267967922, 2.19240215131716394363476960255, 2.21524781328299046507842208032, 2.23879728143015418864904200351,
  2.26311466694490835351573544076, 2.28827300805157643267084011891, 2.31435604894699812266939486476, 2.34146048795978434043531292659,
  2.36969885260647551209738645471, 2.39920322494817055544917817450, 2.43013013978464913728162829644, 2.46266713119979698270547205291,
  2.49704164517949055368308433652, 2.53353343078795745430738206406, 2.57249218737695205289488067611, 2.61436340874381837128188016042,
  2.65972749227020134812347957976, 2.70936127691136762243924804229, 2.76433956507998809402134093896, 2.82621272644033894046564603318,
  2.89734153514829733018851271394, 2.98159419140365968437694926385, 3.08601280427975485670709771297, 3.22573136821851522103605577983,
  3.44508288805539135654449538289, 3.71537970891694553086615539964
};

const Scalar NormalZigguratRatio[NumberOfBandNormalZigurrat] =
{
  0.0, 0.750484775353838553484969850043, 0.850740271310955168539797476445, 0.893433541969631939167066002439,
  0.917015181837094471851335493153, 0.931941203234046233441298028733, 0.942225689827803086945282581056, 0.949736532992396229158449917333,
  0.955459380775071654971678008833, 0.959962976643589437257667346996, 0.963598275031758611303047864447, 0.966593473467949497522283939333,
  0.969103315737874152720557392852, 0.971236417555650662146408519772, 0.973071247290683962507094254337, 0.974665898140297597377304564176,
  0.976064293086860561017929573224, 0.977300253883645249170475123945, 0.978400244899316179173188425569, 0.979385269129977846895884043302,
  0.980272206900904494815390564396, 0.981074779362329241400634997295, 0.981804253938740083291033345358, 0.982469968882383589521668758583,
  0.983079728809119850562796554795, 0.983640106766412054356146072398, 0.984156677615944084392929857459, 0.984634200280007552311796735515,
  0.985076761458307086707253212313, 0.985487889991653471429663256532, 0.985870648634019490358385815715, 0.986227708271493343086583402375,
  0.986561408382272362985077361984, 0.986873806622761920341336293274, 0.987166719753625799389459567589, 0.987441757619090675912393772106,
  0.987700351516051174211224554768, 0.987943778003453440265050048019, 0.988173178983434816520165815368, 0.988389578716739479383699175238,
  0.988593898303624926534040096469, 0.988786968058724293246032191279, 0.988969538127397455898436508912, 0.989142287626960404173170844597,
  0.989305832545044324977737622638, 0.989460732586336934407985727862, 0.989607497125912170683173044980, 0.989746590400581327272272451975,
  0.989878436047901646747803160708, 0.990003421084648827869197604239, 0.990121899401909443020286447879, 0.990234194841858232813880454444,
  0.990340603911263982506239462637, 0.990441398178427316016322972125, 0.990536826393283914925721684389, 0.990627116364558512218324337623,
  0.990712476622928023181406791844, 0.990793097894984079572175093174, 0.990869154409244076145976128873, 0.990940805052437824691268287474,
  0.991008194391705548082954937064, 0.991071453576109215637707296064, 0.991130701128922528307152229819, 0.991186043640474608746669498017,
  0.991237576369836074033189634373, 0.991285383762317554082871105898, 0.991329539888568903748275039752, 0.991370108809995454035219189473,
  0.991407144874221943150024047055, 0.991440692943413945013043999353, 0.991470788557391074579718311726, 0.991497458032617503483410551001,
  0.991520718497315401540555107636, 0.991540577862097828628588285611, 0.991557034724640724341601679445, 0.991570078205989141038468359572,
  0.991579687715098995526722137843, 0.991585832637127947522049524329, 0.991588471939779518821684067585, 0.991587553690640507148992013935,
  0.991583014476894319513703858007, 0.991574778716995497249215923087, 0.991562757851797002828992213523, 0.991546849400162799407295751574,
  0.991526935861188886574791544186, 0.991502883441690831444489168144, 0.991474540583463335923580694623, 0.991441736259812196096950028863,
  0.991404278004792265983740672627, 0.991361949631191261607889802980, 0.991314508584238996353362687132, 0.991261682866858262508333044405,
  0.991203167458441781253777050367, 0.991138620131902842410876661610, 0.991067656552139074390092217899, 0.990989844511792319927328343055,
  0.990904697125582941945109300521, 0.990811664760263111407350456154, 0.990710125420284608160345942834, 0.990599373235399951429113359060,
  0.990478604599807611123088277090, 0.990346901385037190355276297326, 0.990203210479193398963460161789, 0.990046318677290513694722835952,
  0.989874821637971854574784332163, 0.989687085197041986702889368351, 0.989481196737866229216740827939, 0.989254903487686985183950871399,
  0.989005533422740516025904697311, 0.988729892746066823168948592841, 0.988424131369219266125861236354, 0.988083564029399232263953634600,
  0.987702428858508882825722518646, 0.987273556123533683467427764224, 0.986787905274353495741507623455, 0.986233904410023291038018271182,
  0.985596485459788251551070665927, 0.984855636576794042676975880029, 0.983984161793717454104724720040, 0.982944085941803527491004884902,
  0.981680632603656288469381808499, 0.980111601026471711171610577703, 0.978107394117398550145114447318, 0.975450319596402797690182574081,
  0.971742413337712360174408714492, 0.966163908091604482763906551568, 0.956686237014236193571274296719, 0.936329102386070277297433094023,
  0.927249206800360320095099544455
};

// For Beta distribution. WARNING: here we adopt the classical definition
// If tail=false, compute P(X<=x), else P(X>x)
OT_API Scalar pBeta(const Scalar p1,
                    const Scalar p2,
                    const Scalar x,
                    const Bool tail = false);
// If tail=false, compute x such that P(X<=x)=q, else x such that P(X>x)=q
OT_API Scalar qBeta(const Scalar p1,
                    const Scalar p2,
                    const Scalar p,
                    const Bool tail = false);
OT_API Scalar rBeta(const Scalar p1,
                    const Scalar p2);
OT_API Point rBeta(const Scalar p1,
                   const Scalar p2,
                   const UnsignedInteger size);

OT_API Scalar logdBinomial(const UnsignedInteger n,
                           const Scalar p,
                           const UnsignedInteger k);
OT_API Scalar dBinomial(const UnsignedInteger n,
                        const Scalar p,
                        const UnsignedInteger k);
OT_API UnsignedInteger rBinomial(const UnsignedInteger n,
                                 const Scalar p);
OT_API Indices rBinomial(const UnsignedInteger n,
                         const Scalar p,
                         const UnsignedInteger size);
// For discrete distribution
#ifndef SWIG
OT_API UnsignedInteger rDiscrete(const Point & probabilities,
                                 Point & base,
                                 Indices & alias);
OT_API Indices rDiscrete(const Point & probabilities,
                         Point & base,
                         Indices & alias,
                         const UnsignedInteger size);
OT_API UnsignedInteger rDiscrete(const Point & base,
                                 const Indices & alias);
OT_API Indices rDiscrete(const Point & base,
                         const Indices & alias,
                         const UnsignedInteger size);
#endif
OT_API UnsignedInteger rDiscrete(const Point & probabilities);
OT_API Indices rDiscrete(const Point & probabilities,
                         const UnsignedInteger size);
// For Gamma distribution
OT_API Scalar pGamma(const Scalar k,
                     const Scalar x,
                     const Bool tail = false);
OT_API Scalar qGamma(const Scalar k,
                     const Scalar p,
                     const Bool tail = false);
OT_API Scalar rGamma(const Scalar k);
OT_API Point rGamma(const Scalar k,
                    const UnsignedInteger size);

// For Hypergeometric distribution
OT_API Scalar dHypergeometric(const UnsignedInteger n,
                              const UnsignedInteger k,
                              const UnsignedInteger m,
                              const UnsignedInteger x);
OT_API Scalar logdHypergeometric(const UnsignedInteger n,
                                 const UnsignedInteger k,
                                 const UnsignedInteger m,
                                 const UnsignedInteger x);
OT_API Scalar pHypergeometric(const UnsignedInteger n,
                              const UnsignedInteger k,
                              const UnsignedInteger m,
                              const UnsignedInteger x,
                              const Bool tail = false);
OT_API UnsignedInteger rHypergeometric(const UnsignedInteger n,
                                       const UnsignedInteger k,
                                       const UnsignedInteger m);
OT_API Indices rHypergeometric(const UnsignedInteger n,
                               const UnsignedInteger k,
                               const UnsignedInteger m,
                               const UnsignedInteger size);
// For Kolmogorov distribution
OT_API Scalar pKolmogorov(const UnsignedInteger n,
                          const Scalar x,
                          const Bool tail = false);

// For NonCentralChiSquare distribution
OT_API Scalar dNonCentralChiSquare(const Scalar nu,
                                   const Scalar lambda,
                                   const Scalar x,
                                   const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                   const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));
OT_API Scalar pNonCentralChiSquare(const Scalar nu,
                                   const Scalar lambda,
                                   const Scalar x,
                                   const Bool tail = false,
                                   const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                   const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));
OT_API Scalar rNonCentralChiSquare(const Scalar nu,
                                   const Scalar lambda);
OT_API Point rNonCentralChiSquare(const Scalar nu,
                                  const Scalar lambda,
                                  const UnsignedInteger size);
// For NonCentralStudent distribution
OT_API Scalar dNonCentralStudent(const Scalar nu,
                                 const Scalar delta,
                                 const Scalar x);
OT_API Scalar dNonCentralStudentAlt0(const Scalar nu,
                                     const Scalar delta,
                                     const Scalar x,
                                     const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                     const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));
OT_API Scalar pNonCentralStudent(const Scalar nu,
                                 const Scalar delta,
                                 const Scalar x,
                                 const Bool tail = false);
OT_API Scalar rNonCentralStudent(const Scalar nu,
                                 const Scalar delta);
OT_API Point rNonCentralStudent(const Scalar nu,
                                const Scalar delta,
                                const UnsignedInteger size);
// For Normal distribution
OT_API Scalar pNormal(const Scalar x,
                      const Bool tail = false);
OT_API Point pNormal(const Point & x,
                     const Bool tail = false);
OT_API Scalar pNormal2D(const Scalar x1,
                        const Scalar x2,
                        const Scalar rho,
                        const Bool tail = false);
OT_API Scalar pNormal3D(const Scalar x1,
                        const Scalar x2,
                        const Scalar x3,
                        const Scalar rho12,
                        const Scalar rho13,
                        const Scalar rho23,
                        const Bool tail = false);
OT_API Scalar logpNormal(const Scalar x,
                         const Bool tail = false);
OT_API Scalar qNormal(const Scalar p,
                      const Bool tail = false);
OT_API Point qNormal(const Point & p,
                     const Bool tail = false);
OT_API Scalar rNormal();
OT_API Point rNormal(const UnsignedInteger size);

OT_API Scalar dNormal(const Scalar x);
OT_API Point dNormal(const Point &x);
OT_API Scalar logdNormal(const Scalar x);

// For Poisson distribution
OT_API Scalar logdPoisson(const Scalar lambda,
                          const UnsignedInteger k );
OT_API Scalar dPoisson(const Scalar lambda,
                       const UnsignedInteger k);
OT_API Scalar qPoisson(const Scalar lambda,
                       const Scalar p,
                       const Bool tail = false);
OT_API UnsignedInteger rPoisson(const Scalar lambda);
OT_API Indices rPoisson(const Scalar lambda,
                        const UnsignedInteger size);

// For Pearson test
OT_API Scalar pPearsonCorrelation(const UnsignedInteger size, const Scalar rho, const Bool tail = false);

// For Spearman test
OT_API Scalar pSpearmanCorrelation(const UnsignedInteger size, const Scalar rho, const Bool tail = false, const Bool ties = false);
// For Student distribution
OT_API Scalar pStudent(const Scalar nu,
                       const Scalar x,
                       const Bool tail = false);
OT_API Point pStudent(const Scalar nu,
                      const Point & x,
                      const Bool tail = false);
OT_API Scalar qStudent(const Scalar nu,
                       const Scalar p,
                       const Bool tail = false);
OT_API Point qStudent(const Scalar nu,
                      const Point & p,
                      const Bool tail = false);
OT_API Scalar rStudent(const Scalar nu);
OT_API Point rStudent(const Scalar nu,
                      const UnsignedInteger size);
// For uniform distribution over a triangle
OT_API Point rUniformTriangle(const Point & a,
                              const Point & b,
                              const Point & c);
OT_API Sample rUniformTriangle(const Point & a,
                               const Point & b,
                               const Point & c,
                               const UnsignedInteger size);
// For TruncatedNormal distribution
//      Scalar rTruncatedNormal(const Scalar a, const Scalar b);
// Compute the expectation of the min of n independent standard normal random variables
OT_API Scalar eZ1(const UnsignedInteger n);

// K factor for exact two-sided tolerance intervals of normal pooled populations
OT_API Scalar kFactorPooled(const UnsignedInteger n,
                            const UnsignedInteger m,
                            const Scalar p,
                            const Scalar alpha);

// K factor for exact two-sided tolerance intervals of a normal population
OT_API Scalar kFactor(const UnsignedInteger n,
                      const Scalar p,
                      const Scalar alpha);

// Asymptotic distribution of the DickeyFuller distribution
OT_API Scalar pDickeyFullerTrend(const Scalar x,
                                 const Bool tail = false);

OT_API Scalar pDickeyFullerConstant(const Scalar x,
                                    const Bool tail = false);

OT_API Scalar pDickeyFullerNoConstant(const Scalar x,
                                      const Bool tail = false);

OT_API Scalar qDickeyFullerTrend(const Scalar p,
                                 const Bool tail = false);

OT_API Scalar qDickeyFullerConstant(const Scalar p,
                                    const Bool tail = false);

OT_API Scalar qDickeyFullerNoConstant(const Scalar p,
                                      const Bool tail = false);

} /* DistFunc */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTFUNC_HXX */
