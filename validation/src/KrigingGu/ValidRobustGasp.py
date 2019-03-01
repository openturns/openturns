import numpy as np
import openturns as ot
from openturns.viewer import View

ot.Log.Show(ot.Log.Flags() ^ ot.Log.WARN)

ot.ResourceMap.SetAsScalar("GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound", 1e-4);
ot.ResourceMap.SetAsScalar("GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound", 1e4);

import subprocess

def evalueErreur(noyau_covariance, realisation, base=ot.Basis(0), prior=0, parametrization=0):
    """
    Évalue la distance entre
    1. le vrai paramètre d'échelle (fourni par la commande noyau_covariance.getScale()) et
    2. le paramètre d'échelle estimé sur la base d'une realisation du vecteur gaussien observé.   

    Arguments :
    noyau_covariance -- ot.CovarianceModel représentant le vrai modèle
    realisation -- ot.Realization sur laquelle est faite l'estimation
    base -- ot.Basis : doit être spécifié pour le krigeage universel
    """
    noyau_covariance.setScaleParametrization(parametrization)
    processus = ot.GaussianProcess(noyau_covariance, realisation.getMesh())
    
    sigma = np.array(noyau_covariance.getAmplitude()) # vraie amplitude
    matrice_covariance = noyau_covariance.discretize(realisation.getMesh()) # vraie matrice de covariance
    matrice_correlation = np.multiply(sigma**(-2), matrice_covariance) # vraie matrice de corrélation
    matrice_fisher = np.arctanh(matrice_correlation - np.identity(realisation.getValues().getSize())) # matrice de corrélation transformée

    #La ligne suivante définit l'algorithme de krigeage. C'est à ce niveau qu'il faut permettre d'estimer les paramètres par les méthodes de Gu.
    krigeage = ot.KrigingAlgorithm(realisation.getMesh().getVertices(),realisation,noyau_covariance,base,False)
    krigeage.setScalePrior(prior)
    krigeage.run()

    resultat = krigeage.getResult()
    sigma_estime = np.array(resultat.getCovarianceModel().getAmplitude()) # amplitude estimée
    matrice_covariance_estimee = resultat.getCovarianceModel().discretize(realisation.getMesh()) # matrice de covariance estimée
    matrice_correlation_estimee = np.multiply(sigma_estime**(-2), matrice_covariance_estimee) # matrice de corrélation estimée
    matrice_fisher_estimee = np.arctanh(matrice_correlation_estimee - np.identity(realisation.getValues().getSize())) # matrice de corrélation transformée
    
    return np.linalg.norm(matrice_fisher_estimee - matrice_fisher,'fro') # erreur d'estimation

NB_ITERATIONS = 100

def generePoints(nb_pts, dim) :
    points = ot.RandomGenerator.Generate(nb_pts * dim)
    points = ot.Matrix(nb_pts, dim, points)
    return np.array(points)   

NB_PTS_PLAN_EXPERIENCE = 30
DIMENSION_SPATIALE = 3
AMPLITUDE = [2.0]
SCALE = [0.4, 0.8, 0.2]
TREND = ot.SymbolicFunction(['x1','x2','x3'],['5 + 20*x1 + 3*x2 - 12 * x3'])


def echantillonneErreurs (noyau_covariance, prior, parametrization) :
    """
    Produit un dictionnaire contenant :
    1. "simple" : ot.Sample des erreurs d'estimation de krigeage simple pour NB_ITERATIONS plans d'expériences et réalisations de processus gaussiens.
    2. "universel" : ot.Sample des erreurs d'estimation de krigeage universel (base affine) pour NB_ITERATIONS plans d'expériences et réalisations de processus gaussiens.
    3. "simple_distribution" : ot.Distribution obtenue par KernelSmoothing du ot.Sample "simple"
    4. "universel_distribution" : ot.Distribution obtenue par KernelSmoothing du ot.Sample "universel"

    Argument :
    noyau_covariance -- ot.CovarianceModel représentant le vrai modèle
    """
    
    ech_erreur_correlation = np.empty(NB_ITERATIONS)
    ech_erreur_correlation_univ = np.empty(NB_ITERATIONS)


    for iter in range(NB_ITERATIONS):
        print(iter, NB_ITERATIONS)
        points = generePoints(NB_PTS_PLAN_EXPERIENCE, DIMENSION_SPATIALE)
    
        realisation = ot.GaussianProcess(noyau_covariance,ot.Mesh(points)).getRealization() #réalisation avec moyenne nulle
        realisation_univ = ot.Field(realisation.getMesh(),realisation.getValues() + TREND(points)) #réalisation + ajout du vecteur moyenne
        
        ech_erreur_correlation[iter] = np.array(evalueErreur(noyau_covariance, realisation, ot.Basis(0), prior, parametrization)) #erreur d'estimation en krigeage simple
    
        ech_erreur_correlation_univ[iter] = np.array(evalueErreur(noyau_covariance, realisation_univ, 
                                                            ot.LinearBasisFactory(DIMENSION_SPATIALE).build(), prior, parametrization)) #erreur d'estimation en krigeage universel

    ech_erreur_correlation = ot.Sample(ech_erreur_correlation.reshape((NB_ITERATIONS,1))) #reshape nécessaire car Sample demande un 2d-array
    ech_erreur_correlation_univ = ot.Sample(ech_erreur_correlation_univ.reshape((NB_ITERATIONS,1)))
    
    ech_erreur = {}
    ech_erreur["simple"] = ech_erreur_correlation
    ech_erreur["universel"] = ech_erreur_correlation_univ
    ech_erreur["simple_distribution"] = ot.KernelSmoothing().build(ech_erreur_correlation)
    ech_erreur["universel_distribution"] = ot.KernelSmoothing().build(ech_erreur_correlation_univ)
    
    return ech_erreur

MATERN_CINQ_DEMIS = ot.MaternModel(SCALE,AMPLITUDE,2.5)
MATERN_TROIS_DEMIS = ot.MaternModel(SCALE,AMPLITUDE,1.5)
EXPONENTIEL = ot.ExponentialModel(SCALE,AMPLITUDE)
EXPONENTIEL_ABSOLU = ot.AbsoluteExponential(SCALE,AMPLITUDE)
COSINUS_EXPONENTIEL = ot.ExponentiallyDampedCosineModel(SCALE,AMPLITUDE,1.0)
EXPONENTIEL_CARRE = ot.SquaredExponential(SCALE,AMPLITUDE)
EXPONENTIEL_GENERALISE = ot.GeneralizedExponential(SCALE,AMPLITUDE,1.9)
SPHERIQUE = ot.SphericalModel(SCALE,AMPLITUDE,1.5)
PRODUIT_MATERN_CINQ_DEMIS = ot.ProductCovarianceModel([ot.MaternModel([SCALE[i]], AMPLITUDE, 2.5) for i in range(3)])
PRODUIT_MATERN_TROIS_DEMIS = ot.ProductCovarianceModel([ot.MaternModel([SCALE[i]], AMPLITUDE, 1.5) for i in range(3)])
PRODUIT_EXPONENTIEL = ot.ProductCovarianceModel([ot.ExponentialModel([SCALE[i]], AMPLITUDE) for i in range(3)])
PRODUIT_EXPONENTIEL_GENERALISE = ot.ProductCovarianceModel([ot.GeneralizedExponential([SCALE[i]], AMPLITUDE, 1.9) for i in range(3)])


def getCovModelLabel(model):
    covname = covarianceModel.getClassName()
    covname += str(int(10*covarianceModel.getFullParameter()[-1])) if covarianceModel.getClassName() == 'MaternModel' else ''
    if covname == 'ProductCovarianceModel':
        submodel = covarianceModel.getCollection()[0].getImplementation()
        subname = submodel.getClassName()
        subname += str(int(10*submodel.getFullParameter()[-1])) if subname == 'MaternModel' else ''
        covname += subname
    return covname

prior_string = {ot.GeneralLinearModelAlgorithm.NONE: 'none',
                ot.GeneralLinearModelAlgorithm.JOINTLYROBUST: 'jointlyrobust',
                ot.GeneralLinearModelAlgorithm.REFERENCE: 'reference'}
parametrization_string = {ot.CovarianceModelImplementation.STANDARD: 'standard',
                          ot.CovarianceModelImplementation.INVERSE: 'inverse',
                          ot.CovarianceModelImplementation.LOGINVERSE: 'loginverse'}

covModels = [MATERN_CINQ_DEMIS, MATERN_TROIS_DEMIS, EXPONENTIEL, EXPONENTIEL_ABSOLU, COSINUS_EXPONENTIEL, EXPONENTIEL_CARRE, EXPONENTIEL_GENERALISE, SPHERIQUE, PRODUIT_MATERN_CINQ_DEMIS, PRODUIT_MATERN_TROIS_DEMIS, PRODUIT_EXPONENTIEL, PRODUIT_EXPONENTIEL_GENERALISE]

# write report
with open('gu.tex', 'w') as tex:
    tex.write('\\documentclass[12pt,a4paper]{article}\n')
    tex.write('\\usepackage{graphicx}\n')
    tex.write('\\usepackage{geometry}\n')
    tex.write('\\usepackage{hyperref}\n')
    tex.write('\\geometry{margin=5mm}\n')
    tex.write('\\begin{document}\n')
    tex.write('\\tableofcontents\n')
    tex.write('\\newpage\n')
    for covarianceModel in covModels:
        covname = getCovModelLabel(covarianceModel)
        tex.write('\\section{'+covname+'}\n')
        tex.write('\\begin{tabular}{ccc}\n')
        tex.write('  standard & inverse & log-inverse \\\\\n')
        for prior in [ot.GeneralLinearModelAlgorithm.NONE, ot.GeneralLinearModelAlgorithm.JOINTLYROBUST, ot.GeneralLinearModelAlgorithm.REFERENCE]:
            for parametrization in [ot.CovarianceModelImplementation.STANDARD,
                                    ot.CovarianceModelImplementation.INVERSE,
                                    ot.CovarianceModelImplementation.LOGINVERSE]:
                eotab = '\\\\' if parametrization==ot.CovarianceModelImplementation.LOGINVERSE else '&'
                tex.write('  \\includegraphics[width=60mm]{graph_'+covname+'_'+prior_string[prior]+'_'+parametrization_string[parametrization]+'.png}'+eotab+'\n')
        tex.write('\\end{tabular}\n')
        tex.write('\\newpage\n')
    tex.write('\\end{document}\n')

# write pngs
for covarianceModel in covModels:
    covname = getCovModelLabel(covarianceModel)
    for prior in [ot.GeneralLinearModelAlgorithm.NONE, ot.GeneralLinearModelAlgorithm.JOINTLYROBUST, ot.GeneralLinearModelAlgorithm.REFERENCE]:
        for parametrization in [ot.CovarianceModelImplementation.STANDARD,
                                ot.CovarianceModelImplementation.INVERSE,
                                ot.CovarianceModelImplementation.LOGINVERSE]:
            if covname.startswith('ProductCovarianceModel') and parametrization != ot.CovarianceModelImplementation.STANDARD:
                subprocess.run('cp graph_'+covname+'_'+prior_string[prior]+'_standard.png graph_'+covname+'_'+prior_string[prior]+'_'+parametrization_string[parametrization]+'.png', shell=True)
                continue
            ot.RandomGenerator.SetSeed(0)
            print('-- covmodel=', covname, 'prior=', prior_string[prior], 'parametrization=', parametrization_string[parametrization])
            ECH_ERREURS = echantillonneErreurs(covarianceModel, prior, parametrization)
            graph = ECH_ERREURS["simple_distribution"].drawPDF()
            graph.add(ECH_ERREURS["universel_distribution"].drawPDF())
            graph.setLegends(['simple kriging','universal kriging'])
            graph.setColors(['blue','red'])
            graph.setXTitle('')
            graph.setBoundingBox(ot.Interval([0.0, 0.0], [20.0, 0.5]))
            graph.setTitle("Estimated error probability density\ncov="+covname[:6]+"... prior="+prior_string[prior]+" param="+parametrization_string[parametrization]+" pts="+str(NB_ITERATIONS))
            view = View(graph)
            view.save('graph_'+covname+'_'+prior_string[prior]+'_'+parametrization_string[parametrization]+'.png')
            view.close()

subprocess.run('pdflatex -halt-on-error gu.tex && pdflatex gu.tex', shell=True)
