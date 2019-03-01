
# coding: utf-8

# # Implémentation Python de la méthode de Gu
# 
# L'implémentation est limitée aux noyaux de Matérn et au prior de référence. Le Jointly Robust Prior n'est pas implémenté.

# In[1]:


import openturns as ot
import numpy as np
import matplotlib.pyplot as plt


# ## Création d'un plan d'expériences et d'une réalisation d'un processus gaussien

# In[2]:


NB_PTS_PLAN_EXPERIENCE = 20
DIMENSION_SPATIALE = 3

TREND = ot.SymbolicFunction(['x1','x2','x3'],['5'])
#TREND = ot.SymbolicFunction(['x'], ['0'])
NU = 1.5

AMPLITUDE = [2.0]
SCALE = [0.1, 0.1, 0.1]
#SCALE = [.3]

loi_ech = ot.ComposedDistribution([ot.Uniform(0,1)] * DIMENSION_SPATIALE)
points = loi_ech.getSample(NB_PTS_PLAN_EXPERIENCE)

modele_covariance = ot.MaternModel(SCALE, AMPLITUDE, NU)
sorties= ot.GaussianProcess(ot.TrendTransform(TREND ,ot.Mesh(points)), modele_covariance,ot.Mesh(points)).getRealization()


# ## Création manuelle du noyau

# ### Noyau de Matérn avec paramétrisation standard

# In[3]:


def matern(x, nu, theta):
    """Implémentation directe du noyau de Matérn avec paramétrisation standard.
    
    Arguments:
        nu : paramètre de régularité
        x : np.array représentant l'écart algébrique entre deux points
        theta : np.array de même taille que x contenant les longueurs de corrélation
        
    """
    if len(x)!=len(theta):
        raise ValueError("La longueur de x est {!r} alors que la longueur de theta est {!r}.".format(len(x), len(theta)))
    x = np.array(x)
    theta = np.array(theta)
    if all(x)==0:
        return 1.0
    expression = np.sqrt(2 * nu) * np.linalg.norm(x / theta)
    return np.exp((1. - nu) * np.log(2.)) / ot.SpecFunc_Gamma(nu) *            np.exp(nu * np.log(expression)) * ot.SpecFunc_BesselK(nu, expression)

def derivee_matern(x, indice_derive, nu, theta):
    """Implémentation de la dérivée du noyau de Matérn avec paramétrisation standard.
    Il s'agit de la dérivée partielle par rapport au indice_derive-ième élément du paramètre theta.
    
    Arguments:
        indice_derive : numéro de la longueur de corrélation par rapport à laquelle est faite la dérivation
        nu : paramètre de régularité
        x : np.array représentant l'écart algébrique entre deux points
        theta : np.array de même taille que x contenant les longueurs de corrélation
        
    """
    if len(x)!=len(theta):
        raise ValueError("La longueur de x est {!r} alors que la longueur de theta est {!r}.".format(len(x), len(theta)))
    if indice_derive<0:
        raise ValueError("indice_derive vaut {!r} : ce devrait être un entier positif ou nul.".format(indice_derive))
    if indice_derive>=len(theta):
        raise ValueError("indice_derive vaut {!r} or theta n'a que {!r} composantes !".format(indice_derive, len(theta)))
    x = np.array(x)
    theta = np.array(theta)
    if all(x)==0:
        return 0.
    expression = np.sqrt(2 * nu) * np.linalg.norm(x / theta)
    return nu / ot.SpecFunc_Gamma(nu) / np.power(2., nu-2.) * x[indice_derive]**2 *            theta[indice_derive]**(-3) *            np.power(expression, nu-1.) * ot.SpecFunc_BesselK(nu - 1., expression)


# In[4]:


print('matern(nu=NU, x=[2, 2], theta=[3, 4])=', matern(nu=NU, x=[2, 2], theta=[3, 4]))


# In[5]:


print('derivee_matern=', [ derivee_matern(indice_derive=i, nu=NU, x=[2, 2], theta=[3., 4]) for i in range(2)])




# In[104]:


ot_matern = ot.MaternModel([3, 4], AMPLITUDE, NU )
print('ot_matern([2,2])=',ot_matern([2,2]))
print('ot_matern.parameterGradient=', ot_matern.parameterGradient([2,2],[0,0]))

# In[105]:


h=1.e-8
(matern(nu=NU, x=[2,2], theta=[3+h,4]) - matern(nu=NU, x=[2,2], theta=[3,4]))/h


# ### Noyau de Matérn avec paramétrisation inverse

# In[8]:


def matern_param_inverse(x, nu, mu):
    """Implémentation directe du noyau de Matérn avec paramétrisation inverse.
    
    Arguments:
        nu : paramètre de régularité
        x : np.array représentant l'écart algébrique entre deux points
        mu : np.array de même taille que x contenant les longueurs de corrélation inverses
        
    """ 
    if len(x)!=len(mu):
        raise ValueError("La longueur de x est {!r} alors que la longueur de mu est {!r}.".format(len(x), len(mu)))
    x = np.array(x)
    mu = np.array(mu)
    return matern(nu=nu, x=x, theta=1. / mu)

def derivee_matern_param_inverse(x, indice_derive, nu, mu):
    """Implémentation de la dérivée du noyau de Matérn avec paramétrisation inverse.
    Il s'agit de la dérivée partielle par rapport au indice_derive-ième élément du paramètre mu.
    
    Arguments:
        indice_derive : numéro de la longueur de corrélation inverse par rapport à laquelle est faite la dérivation
        nu : paramètre de régularité
        x : np.array représentant l'écart algébrique entre deux points
        mu : np.array de même taille que x contenant les longueurs de corrélation inverses
        
    """
    if len(x)!=len(mu):
        raise ValueError("La longueur de x est {!r} alors que la longueur de mu est {!r}.".format(len(x), len(mu)))
    if indice_derive<0:
        raise ValueError("indice_derive vaut {!r} : ce devrait être un entier positif ou nul.".format(indice_derive))
    if indice_derive>=len(mu):
        raise ValueError("indice_derive vaut {!r} or mu n'a que {!r} composantes !".format(indice_derive, len(mu)))
    x = np.array(x)
    mu = np.array(mu)
    return derivee_matern(indice_derive=indice_derive, nu=nu, x=x, theta=1. / mu) / -mu[indice_derive]**2


# In[9]:



print('matern_param_inverse=', matern_param_inverse(nu=NU, x=[2, 2], mu=[1/3, 1/4]))
ot_matern.setScaleParametrization(ot.CovarianceModelImplementation.INVERSE)
print(ot_matern.getParameter(), ot_matern.getParameterDescription())
print('ot_matern([2,2])=',ot_matern([2,2]))
print('ot_matern.parameterGradient=', ot_matern.parameterGradient([2,2],[0,0]))
# In[10]:



print('derivee_matern_param_inverse=', [derivee_matern_param_inverse(indice_derive=i, nu=NU, x=[2, 2], mu=[1/3., 1/4]) for i in range(2)])

# ### Noyau de Matérn avec paramétrisation log-inverse

# In[11]:


def matern_param_log_inverse(x, nu, xi):
    """Implémentation directe du noyau de Matérn avec paramétrisation log-inverse.
    
    Arguments:
        nu : paramètre de régularité
        x : np.array représentant l'écart algébrique entre deux points
        xi : np.array de même taille que x contenant l'opposé des logartihmes des longueurs de corrélation
        
    """
    if len(x)!=len(xi):
        raise ValueError("La longueur de x est {!r} alors que la longueur de xi est {!r}.".format(len(x), len(xi)))
    x = np.array(x)
    xi = np.array(xi)
    return matern(nu=nu, x=x, theta=np.exp(- xi))

def derivee_matern_param_log_inverse(x, indice_derive, nu, xi):
    """Implémentation de la dérivée du noyau de Matérn avec paramétrisation log-inverse.
    Il s'agit de la dérivée partielle par rapport au indice_derive-ième élément du paramètre xi.
    
    Arguments:
        indice_derive : numéro de la longueur de corrélation log-inverse par rapport à laquelle est faite la dérivation
        nu : paramètre de régularité
        x : np.array représentant l'écart algébrique entre deux points
        xi : np.array de même taille que x contenant l'opposé des logartihmes des longueurs de corrélation
        
    """
    if len(x)!=len(xi):
        raise ValueError("La longueur de x est {!r} alors que la longueur de xi est {!r}.".format(len(x), len(xi)))
    if indice_derive<0:
        raise ValueError("indice_derive vaut {!r} : ce devrait être un entier positif ou nul.".format(indice_derive))
    if indice_derive>=len(xi):
        raise ValueError("indice_derive vaut {!r} or xi n'a que {!r} composantes !".format(indice_derive, len(xi)))
    x = np.array(x)
    xi = np.array(xi)
    return derivee_matern(indice_derive=indice_derive, nu=nu, x=x, theta=np.exp(- xi)) * -np.exp(- xi[indice_derive])


# In[12]:


def matern_gen(x, nu, p, param):
    if param == 'standard':
        return matern(x, nu, p)
    elif param == 'inverse':
        return matern_param_inverse(x, nu, p)
    elif param == 'log_inverse':
        return matern_param_log_inverse(x, nu, p)
    raise ValueError(param)


def derivee_matern_gen(x, indice_derive, nu, p, param):
    if param == 'standard':
        return derivee_matern(x, indice_derive, nu)
    elif param == 'inverse':
        return derivee_matern_param_inverse(x, indice_derive, nu)
    elif param == 'log_inverse':
        return derivee_matern_param_log_inverse(x, indice_derive, nu, p)
    raise ValueError(param)


print('matern_param_log_inverse', matern_param_log_inverse(nu=NU, x=[2, 2], xi=[-np.log(3), -np.log(4)]))
ot_matern.setScaleParametrization(ot.CovarianceModelImplementation.LOGINVERSE)
print(ot_matern.getParameter(), ot_matern.getParameterDescription())
print('ot_matern([2,2])=',ot_matern([2,2]))
print('ot_matern.parameterGradient=', ot_matern.parameterGradient([2,2],[0,0]))

# In[13]:


print('derivee_matern_param_log_inverse', [derivee_matern_param_log_inverse(indice_derive=i, nu=NU, x=[2, 2], xi=[-np.log(3), -np.log(4)]) for i in range(2)])


# ## Création de la matrice de corrélation et de sa dérivée par rapport au paramètre

# ### Paramétrisation standard

# In[14]:


def matrice_correlation_matern(nu, plan_xp, theta):
    """Renvoie la matrice de corrélation correspondant à un noyau de Matérn utilisant la paramétrisation standard.
    
    Arguments :
        nu : paramètre de régularité
        plan_xp : ot.Sample contenant le plan d'expériences
        theta : np.array de même taille que x contenant les longueurs de corrélation
    
    """
    soustractions = np.subtract.outer(plan_xp, plan_xp)
    ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
    for indice in range(ecarts_algebriques.shape[2]):
        ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
    matrice_corr = np.apply_along_axis(matern, 2, ecarts_algebriques, nu=nu, theta=theta)
    return matrice_corr

def matrice_correlation_derivee_matern(indice_derive, nu, plan_xp, theta):
    """Renvoie la matrice de corrélation dérivée par rapport à la indice_derive-ième longeur de corrélation pour un noyau de Matérn.
    
    Arguments :
        indice_derive : numéro de la longueur de corrélation par rapport à laquelle est faite la dérivation
        nu : paramètre de régularité
        plan_xp : ot.Sample contenant le plan d'expériences
        theta : np.array de même taille que x contenant les longueurs de corrélation
    
    """
    soustractions = np.subtract.outer(plan_xp, plan_xp)
    ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
    for indice in range(ecarts_algebriques.shape[2]):
        ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
    matrice_corr_derivee = np.apply_along_axis(derivee_matern, 2, ecarts_algebriques, indice_derive=indice_derive, nu=nu, theta=theta)
    return matrice_corr_derivee  


# ### Paramétrisation inverse

# In[15]:


def matrice_correlation_matern_param_inverse(nu, plan_xp, mu):
    """Renvoie la matrice de corrélation correspondant à un noyau de Matérn utilisant la paramétrisation standard.
    
    Arguments :
        nu : paramètre de régularité
        plan_xp : ot.Sample contenant le plan d'expériences
        mu : np.array de même taille que x contenant les longueurs de corrélation inverses
    
    """
    soustractions = np.subtract.outer(plan_xp, plan_xp)
    ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
    for indice in range(ecarts_algebriques.shape[2]):
        ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
    matrice_corr = np.apply_along_axis(matern_param_inverse, 2, ecarts_algebriques, nu=nu, mu=mu)
    return matrice_corr

def matrice_correlation_derivee_matern_param_inverse(indice_derive, nu, plan_xp, mu):
    """Renvoie la matrice de corrélation dérivée par rapport à la indice_derive-ième longeur de corrélation pour un noyau de Matérn.
    
    Arguments :
        indice_derive : numéro de la longueur de corrélation par rapport à laquelle est faite la dérivation
        nu : paramètre de régularité
        plan_xp : ot.Sample contenant le plan d'expériences
        mu : np.array de même taille que x contenant les longueurs de corrélation inverses
    
    """
    soustractions = np.subtract.outer(plan_xp, plan_xp)
    ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
    for indice in range(ecarts_algebriques.shape[2]):
        ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
    matrice_corr_derivee = np.apply_along_axis(derivee_matern_param_inverse, 2, ecarts_algebriques, indice_derive=indice_derive, nu=nu, mu=mu)
    return matrice_corr_derivee  


# ### Paramétrisation log-inverse

# In[16]:


def matrice_correlation_matern_param_log_inverse(nu, plan_xp, xi):
    """Renvoie la matrice de corrélation correspondant à un noyau de Matérn utilisant la paramétrisation standard.
    
    Arguments :
        nu : paramètre de régularité
        plan_xp : ot.Sample contenant le plan d'expériences
        xi : np.array de même taille que x contenant les longueurs de corrélation log-inverses
    
    """
    soustractions = np.subtract.outer(plan_xp, plan_xp)
    ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
    for indice in range(ecarts_algebriques.shape[2]):
        ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
    matrice_corr = np.apply_along_axis(matern_param_log_inverse, 2, ecarts_algebriques, nu=nu, xi=xi)
    return matrice_corr

def matrice_correlation_derivee_matern_param_log_inverse(indice_derive, nu, plan_xp, xi):
    """Renvoie la matrice de corrélation dérivée par rapport à la indice_derive-ième longeur de corrélation pour un noyau de Matérn.
    
    Arguments :
        indice_derive : numéro de la longueur de corrélation par rapport à laquelle est faite la dérivation
        nu : paramètre de régularité
        plan_xp : ot.Sample contenant le plan d'expériences
        xi : np.array de même taille que x contenant les longueurs de corrélation log-inverses
    
    """
    soustractions = np.subtract.outer(plan_xp, plan_xp)
    ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
    for indice in range(ecarts_algebriques.shape[2]):
        ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
    matrice_corr_derivee = np.apply_along_axis(derivee_matern_param_log_inverse, 2, ecarts_algebriques, indice_derive=indice_derive, nu=nu, xi=xi)
    return matrice_corr_derivee  



def matrice_correlation_matern_gen(nu, plan_xp, p, param):
    if param == 'standard':
        return matrice_correlation_matern(nu, plan_xp, p)
    elif param == 'inverse':
        return matrice_correlation_matern_param_inverse(nu, plan_xp, p)
    elif param == 'log_inverse':
        return matrice_correlation_matern_param_log_inverse(nu, plan_xp, p)
    raise ValueError(param)


def matrice_correlation_derivee_matern_gen(indice_derive, nu, plan_xp, p, param):
    if param == 'standard':
        return matrice_correlation_derivee_matern(indice_derive, nu, plan_xp, p)
    elif param == 'inverse':
        return matrice_correlation_derivee_matern_param_inverse(indice_derive, nu, plan_xp, p)
    elif param == 'log_inverse':
        return matrice_correlation_derivee_matern_param_log_inverse(indice_derive, nu, plan_xp, p)
    raise ValueError(param)

# ### Calcul des matrices de corrélation correspondant aux différentes parmétrisations

# In[17]:


print('matrice_correlation_matern=', matrice_correlation_matern(nu=NU, plan_xp=points, theta=SCALE))
print('MaternModel.discretize=', ot.MaternModel(SCALE, [1.0], NU).discretize(points))

#exit(1)



# In[18]:


matrice_correlation_matern_param_inverse(nu=NU, plan_xp=points, mu=1./np.array(SCALE))


# In[19]:


matrice_correlation_matern_param_log_inverse(nu=NU, plan_xp=points, xi=-np.log(np.array(SCALE)))


# In[20]:


matrice_correlation_derivee_matern(indice_derive=0, nu=NU, plan_xp=points, theta=SCALE)


# In[21]:


matrice_correlation_derivee_matern_param_inverse(indice_derive=0, nu=NU, plan_xp=points, mu=1./np.array(SCALE))


# In[22]:


matrice_correlation_derivee_matern_param_log_inverse(indice_derive=0, nu=NU, plan_xp=points, xi=-np.log(np.array(SCALE)))


# ## Matrices intervenant dans les formules calculées une fois pour toutes

# In[23]:


def matrices_issues_correlation(matrice_correlation, plan_xp, liste_fonctions_tendance, pepite):
    """Renvoie un dictionnaire contenant :
        correlation : la matrice de corrélation passée en argument
        correlation_conception : matrice homogène à une matrice de corrélation restreinte à l'espace de tendance
        correlation_inverse_transformee : matrice sigma-tilde du cahier des charges
        
        Arguments :
            matrice_correlation : np.array contenant les correlations entre points du plan_xp
            plan_xp : ot.Sample qui recense les points d'observation
            liste_fonctions_tendance
            pepite
    
    """
    matrice_correlation_inverse = np.linalg.inv(matrice_correlation)
    matrice_conception = tendance(liste_fonctions_tendance=liste_fonctions_tendance, plan_xp=plan_xp)
    matrice_correlation_inverse_conception = np.matmul(matrice_correlation_inverse, matrice_conception)
    matrice_correlation_conception = np.linalg.inv(np.matmul(np.transpose(matrice_conception),
                                                             matrice_correlation_inverse_conception))
    matrice_correlation_inverse_transformee = matrice_correlation_inverse -                                              np.matmul(np.matmul(matrice_correlation_inverse_conception,
                                                                  matrice_correlation_conception),
                                                        np.transpose(matrice_correlation_inverse_conception))
    return {'correlation' : matrice_correlation,
            'conception' : matrice_conception, #NÉCESSAIRE UNIQUEMENT SI ON VEUT ANNULER L'EFFET DU CHOIX DES FONCTIONS DE BASE liste_fonctions_tendance
            'correlation_conception' : matrice_correlation_conception,
            'correlation_inverse_transformee' : matrice_correlation_inverse_transformee}


# ## Matrice de tendance $\boldsymbol{F}$

# In[24]:


def tendance(liste_fonctions_tendance, plan_xp):
    """Renvoie la matrice de tendance. Sa taille est plan_sp.getSize() x len(liste_fonctions_tendance).
    
    Arguments :
        liste_fonctions_tendance : liste de fonctions python acceptant plan_xp en entree
        plan_xp : ot.Sample contenant le plan d'expériences

    """
    matrice_tendance = np.zeros((plan_xp.getSize(), len(liste_fonctions_tendance)))
    for indice, fonction in enumerate(liste_fonctions_tendance):
        matrice_tendance[:, indice] = fonction(plan_xp)
    #print('tendance=', matrice_tendance)

    return matrice_tendance


# ### Définition de la fonction constante...
# ... et des fonctions coordonnées, ces dernières n'étant pas utilisées pour l'instant.

# In[25]:


def un(matrice): return np.ones(len(matrice))
def coord1(matrice) : return np.squeeze(matrice[:, 0])
def coord2(matrice) : return np.squeeze(matrice[:, 1])
def coord3(matrice) : return np.squeeze(matrice[:, 2])
LISTE_TENDANCE = [un]

CORR_MATERN = matrice_correlation_matern(nu=NU, plan_xp=points, theta=SCALE)
#DIC_CORR = matrices_issues_correlation(liste_fonctions_tendance=LISTE_TENDANCE, matrice_correlation=CORR_MATERN, pepite=0., plan_xp=points)


# In[26]:


un(points)


# In[27]:


#tend = tendance(LISTE_TENDANCE, points)
#tend


# ## Vraisemblance intégrée

# ### Fonction générique prenant en entrée les matrices apparaissant dans les formules

# In[29]:


def log_vraisemblance_integree(dic_correlation, sorties):
    """Renvoie la vraisemblance intégrée."""
    #Le terme bonus sert à éliminer l'impact du choix de matrice de conception.
    #Il est constant et donc inutile pour l'optimisation.
    #terme_bonus = 0.5 * np.log(np.linalg.det(np.matmul(np.transpose(dic_correlation['conception']),
    #                                                   dic_correlation['conception'])))
    
    #print('log_vraisemblance_integree C=', dic_correlation['correlation'])
    terme0 = -0.5 * np.log(np.linalg.det(dic_correlation['correlation']))
    terme1 = 0.5 * np.log(np.linalg.det(dic_correlation['correlation_conception']))
    terme2 = -0.5 * (dic_correlation['correlation'].shape[0] - 
                     dic_correlation['correlation_conception'].shape[0]) *\
             np.log(np.matmul(np.transpose(sorties),
                              np.matmul(dic_correlation['correlation_inverse_transformee'],
                                        sorties)))
    print('log_vraisemblance_integree terme0=', -2*terme0, 'terme1=',-2*terme1,'terme2=', -2*float(terme2))
    return float(terme0 + terme1 + terme2) # + terme_bonus)


# In[30]:


#log_vraisemblance_integree(DIC_CORR, sorties)


# ### Fonction de plus haut niveau prenant en entrée le paramètre.
# Cette fonction n'est pas utilisée dans la suite.

# In[70]:


def log_vraisemblance_integree_matern(theta, plan_xp, sorties, nu, liste_fonctions_tendance=None, pepite=0.):
    """Fonction à optimiser."""
    matrice_correlation = matrice_correlation_matern(nu=nu, plan_xp=plan_xp, theta=theta)
    dic_correlation = matrices_issues_correlation(matrice_correlation, plan_xp, liste_fonctions_tendance, pepite)

 
    return log_vraisemblance_integree(dic_correlation, sorties)


# In[71]:


#log_vraisemblance_integree_matern(theta=SCALE, plan_xp=points, sorties=sorties, nu=NU, liste_fonctions_tendance=LISTE_TENDANCE, pepite=0.)


# ## Prior de Jeffreys

# ### Fonction renseignant un élément de la matrice d'information de Fisher
# La fonction existe en 3 versions : une pour chaque paramétrisation.

# In[31]:


def element_info_fisher_matern(dic_correlation, ligne, colonne, theta, nu, plan_xp):
    """Renvoie un élément de la matrice d'information de Fisher."""
    #print('element_info_fisher_matern correlation=', dic_correlation['correlation'].shape, dic_correlation['correlation'])
    #print('element_info_fisher_matern correlation_inverse_transformee=', dic_correlation['correlation_inverse_transformee'].shape, dic_correlation['correlation_inverse_transformee'])
    if ligne==len(theta) or colonne==len(theta):
        if ligne==len(theta) and colonne==len(theta):
            return dic_correlation['correlation'].shape[0] - dic_correlation['correlation_conception'].shape[0]
        derivee = matrice_correlation_derivee_matern(indice_derive=min(ligne,colonne),
                                                     nu=nu, plan_xp=plan_xp, theta=theta)
        return np.trace(np.matmul(dic_correlation['correlation_inverse_transformee'],
                                  derivee))
    derivee_lig = matrice_correlation_derivee_matern(indice_derive=ligne, nu=nu,
                                                     plan_xp=plan_xp, theta=theta)
    facteur_lig = np.matmul(dic_correlation['correlation_inverse_transformee'],
                            derivee_lig)
    derivee_col = matrice_correlation_derivee_matern(indice_derive=colonne, nu=nu,
                                                     plan_xp=plan_xp, theta=theta)
    facteur_col = np.matmul(dic_correlation['correlation_inverse_transformee'],
                            derivee_col)
    return np.trace(np.matmul(facteur_lig, facteur_col)) 


# In[32]:


def element_info_fisher_matern_param_inverse(dic_correlation, ligne, colonne, mu, nu, plan_xp):
    """Renvoie un élément de la matrice d'information de Fisher."""
    if ligne==len(mu) or colonne==len(mu):
        if ligne==len(mu) and colonne==len(mu):
            return dic_correlation['correlation'].shape[0] - dic_correlation['correlation_conception'].shape[0]
        derivee = matrice_correlation_derivee_matern_param_inverse(indice_derive=min(ligne,colonne),
                                                     nu=nu, plan_xp=plan_xp, mu=mu)
        return np.trace(np.matmul(dic_correlation['correlation_inverse_transformee'],
                                  derivee))
    derivee_lig = matrice_correlation_derivee_matern_param_inverse(indice_derive=ligne, nu=nu,
                                                     plan_xp=plan_xp, mu=mu)
    facteur_lig = np.matmul(dic_correlation['correlation_inverse_transformee'],
                            derivee_lig)
    derivee_col = matrice_correlation_derivee_matern_param_inverse(indice_derive=colonne, nu=nu,
                                                     plan_xp=plan_xp, mu=mu)
    facteur_col = np.matmul(dic_correlation['correlation_inverse_transformee'],
                            derivee_col)
    return np.trace(np.matmul(facteur_lig, facteur_col)) 


# In[40]:


def element_info_fisher_matern_param_log_inverse(dic_correlation, ligne, colonne, xi, nu, plan_xp):
    """Renvoie un élément de la matrice d'information de Fisher."""
    if ligne==len(xi) or colonne==len(xi):
        if ligne==len(xi) and colonne==len(xi):
            return dic_correlation['correlation'].shape[0] - dic_correlation['correlation_conception'].shape[0]
        derivee = matrice_correlation_derivee_matern_param_log_inverse(indice_derive=min(ligne,colonne),
                                                     nu=nu, plan_xp=plan_xp, xi=xi)
        return np.trace(np.matmul(dic_correlation['correlation_inverse_transformee'],
                                  derivee))
    derivee_lig = matrice_correlation_derivee_matern_param_log_inverse(indice_derive=ligne, nu=nu,
                                                     plan_xp=plan_xp, xi=xi)
    facteur_lig = np.matmul(dic_correlation['correlation_inverse_transformee'],
                            derivee_lig)
    derivee_col = matrice_correlation_derivee_matern_param_log_inverse(indice_derive=colonne, nu=nu,
                                                     plan_xp=plan_xp, xi=xi)
    facteur_col = np.matmul(dic_correlation['correlation_inverse_transformee'],
                            derivee_col)
    return np.trace(np.matmul(facteur_lig, facteur_col)) 


# ### Fonction retournant la matrice d'information de Fisher complète
# La fonction existe en 3 versions : une pour chaque paramétrisation.

# In[34]:


def element_info_fisher_matern_gen(dic_correlation, ligne, colonne, p, nu, plan_xp, param):
    if param == 'standard':
        return element_info_fisher_matern(dic_correlation, ligne, colonne, p, nu, plan_xp)
    elif param == 'inverse':
        return element_info_fisher_matern_param_inverse(dic_correlation, ligne, colonne, p, nu, plan_xp)
    elif param == 'log_inverse':
        return element_info_fisher_matern_param_log_inverse(dic_correlation, ligne, colonne, p, nu, plan_xp)
    raise ValueError(param)


def info_fisher_matern(dic_correlation, theta, nu, plan_xp):
    """Renvoie la matrice d'information de Fisher"""
    nb_lignes_colonnes = plan_xp.getDimension()+1
    matrice_info_fisher = np.zeros((nb_lignes_colonnes, nb_lignes_colonnes))
    for ligne in range(nb_lignes_colonnes):
        for colonne in range(nb_lignes_colonnes):
            if colonne>=ligne:
                matrice_info_fisher[ligne, colonne] = element_info_fisher_matern(dic_correlation,
                                                                                 ligne,
                                                                                 colonne,
                                                                                 theta,
                                                                                 nu,
                                                                                 plan_xp)
            else:
                matrice_info_fisher[ligne, colonne] = matrice_info_fisher[colonne, ligne]
    #print('info_fisher_matern=', matrice_info_fisher)
    return matrice_info_fisher


# In[35]:


def info_fisher_matern_param_inverse(dic_correlation, mu, nu, plan_xp):
    """Renvoie la matrice d'information de Fisher"""
    nb_lignes_colonnes = plan_xp.getDimension()+1
    matrice_info_fisher = np.zeros((nb_lignes_colonnes, nb_lignes_colonnes))
    for ligne in range(nb_lignes_colonnes):
        for colonne in range(nb_lignes_colonnes):
            if colonne>=ligne:
                matrice_info_fisher[ligne, colonne] = element_info_fisher_matern_param_inverse(dic_correlation,
                                                                                               ligne,
                                                                                               colonne,
                                                                                               mu,
                                                                                               nu,
                                                                                               plan_xp)
            else:
                matrice_info_fisher[ligne, colonne] = matrice_info_fisher[colonne, ligne]
    return matrice_info_fisher


# In[36]:


def info_fisher_matern_param_log_inverse(dic_correlation, xi, nu, plan_xp):
    """Renvoie la matrice d'information de Fisher"""
    nb_lignes_colonnes = plan_xp.getDimension()+1
    matrice_info_fisher = np.zeros((nb_lignes_colonnes, nb_lignes_colonnes))
    for ligne in range(nb_lignes_colonnes):
        for colonne in range(nb_lignes_colonnes):
            if colonne>=ligne:
                matrice_info_fisher[ligne, colonne] = element_info_fisher_matern_param_log_inverse(dic_correlation,
                                                                                                   ligne,
                                                                                                   colonne,
                                                                                                   xi,
                                                                                                   nu,
                                                                                                   plan_xp)
            else:
                matrice_info_fisher[ligne, colonne] = matrice_info_fisher[colonne, ligne]
    return matrice_info_fisher


# In[37]:

def info_fisher_matern_gen(dic_correlation, p, nu, plan_xp, param):
    if param == 'standard':
        return info_fisher_matern(dic_correlation, p, nu, plan_xp)
    elif param == 'inverse':
        return info_fisher_matern_param_inverse(dic_correlation, p, nu, plan_xp)
    elif param == 'log_inverse':
        return info_fisher_matern_param_log_inverse(dic_correlation, p, nu, plan_xp)
    raise ValueError(param)
  
  
#info_fisher_matern(dic_correlation=DIC_CORR, theta=SCALE, nu=NU, plan_xp=points)


# In[38]:


#info_fisher_matern_param_inverse(dic_correlation=DIC_CORR, mu=1./np.array(SCALE), nu=NU, plan_xp=points)


# In[41]:


#info_fisher_matern_param_log_inverse(dic_correlation=DIC_CORR, xi=-np.log(np.array(SCALE)), nu=NU, plan_xp=points)


# ## Logarithme du prior de référence (= prior de Jeffreys)
# 3 versions corresponant à chacune des 3 paramétrisations.

# In[52]:


def log_prior_jeffreys_matern(theta, nu, plan_xp, liste_fonctions_tendance, pepite):
    """Densité du prior de Jeffreys au point theta."""
    matrice_correlation = matrice_correlation_matern(nu=nu, plan_xp=plan_xp, theta=theta)
    dic_correlation = matrices_issues_correlation(matrice_correlation=matrice_correlation,
                                                  plan_xp=plan_xp,
                                                  liste_fonctions_tendance=liste_fonctions_tendance,
                                                  pepite=pepite)
    return 0.5 * np.log(np.linalg.det(info_fisher_matern(dic_correlation, theta, nu, plan_xp)))


# In[53]:


def log_prior_jeffreys_matern_param_inverse(mu, nu, plan_xp, liste_fonctions_tendance, pepite):
    """Densité du prior de Jeffreys au point mu."""
    matrice_correlation = matrice_correlation_matern_param_inverse(nu=nu, plan_xp=plan_xp, mu=mu)
    dic_correlation = matrices_issues_correlation(matrice_correlation=matrice_correlation,
                                                  plan_xp=plan_xp,
                                                  liste_fonctions_tendance=liste_fonctions_tendance,
                                                  pepite=pepite)
    return 0.5 * np.log(np.linalg.det(info_fisher_matern_param_inverse(dic_correlation, mu, nu, plan_xp)))


# In[55]:


def log_prior_jeffreys_matern_param_log_inverse(xi, nu, plan_xp, liste_fonctions_tendance, pepite):
    """Densité du prior de Jeffreys au point xi."""
    matrice_correlation = matrice_correlation_matern_param_log_inverse(nu=nu, plan_xp=plan_xp, xi=xi)
    dic_correlation = matrices_issues_correlation(matrice_correlation=matrice_correlation,
                                                  plan_xp=plan_xp,
                                                  liste_fonctions_tendance=liste_fonctions_tendance,
                                                  pepite=pepite)
    return 0.5 * np.log(np.linalg.det(info_fisher_matern_param_log_inverse(dic_correlation, xi, nu, plan_xp)))


# In[43]:

def log_prior_jeffreys_matern_gen(p, nu, plan_xp, liste_fonctions_tendance, pepite, param):
    if param == 'standard':
        return log_prior_jeffreys_matern(p, nu, plan_xp, liste_fonctions_tendance, pepite)
    elif param == 'inverse':
        return log_prior_jeffreys_matern_param_inverse(p, nu, plan_xp, liste_fonctions_tendance, pepite)
    elif param == 'log_inverse':
        return log_prior_jeffreys_matern_param_log_inverse(p, nu, plan_xp, liste_fonctions_tendance, pepite)
    raise ValueError(param)

#log_prior_jeffreys_matern(SCALE, NU, points, LISTE_TENDANCE, pepite=0.)


# In[54]:


#log_prior_jeffreys_matern_param_inverse(1./np.array(SCALE), NU, points, LISTE_TENDANCE, pepite=0.)


# In[56]:


#log_prior_jeffreys_matern_param_log_inverse(-np.log(SCALE), NU, points, LISTE_TENDANCE, pepite=0.)


# ## Logarithme de la vraisemblance pénalisée par le prior de référence
# 3 versions corresponant à chacune des 3 paramétrisations.

# In[44]:


def log_vraisemblance_penalisee_matern(theta, plan_xp, sorties, nu, liste_fonctions_tendance=None, pepite=0.):
    """Fonction à optimiser."""
    matrice_correlation = matrice_correlation_matern(nu=nu, plan_xp=plan_xp, theta=theta)
    dic_correlation = matrices_issues_correlation(matrice_correlation, plan_xp, liste_fonctions_tendance, pepite)

    penalization = 0.5 * np.log(np.linalg.det(info_fisher_matern(dic_correlation, theta, nu, plan_xp)))
    #print('penalization=', 2*penalization)

    total = log_vraisemblance_integree(dic_correlation, sorties) +   penalization
    print('penalization=', -2*penalization, 'total=', -2*total)
    return total


# In[76]:


def log_vraisemblance_penalisee_matern_param_inverse(mu, plan_xp, sorties, nu, liste_fonctions_tendance=None, pepite=0.):
    """Fonction à optimiser."""
    matrice_correlation = matrice_correlation_matern_param_inverse(nu=nu, plan_xp=plan_xp, mu=mu)
    dic_correlation = matrices_issues_correlation(matrice_correlation, plan_xp, liste_fonctions_tendance, pepite)

 
    return log_vraisemblance_integree(dic_correlation, sorties) +    0.5 * np.log(np.linalg.det(info_fisher_matern_param_inverse(dic_correlation, mu, nu, plan_xp)))


# In[77]:


def log_vraisemblance_penalisee_matern_param_log_inverse(xi, plan_xp, sorties, nu, liste_fonctions_tendance=None, pepite=0.):
    """Fonction à optimiser."""
    matrice_correlation = matrice_correlation_matern_param_log_inverse(nu=nu, plan_xp=plan_xp, xi=xi)
    dic_correlation = matrices_issues_correlation(matrice_correlation, plan_xp, liste_fonctions_tendance, pepite)
 
    return log_vraisemblance_integree(dic_correlation, sorties) +    0.5 * np.log(np.linalg.det(info_fisher_matern_param_log_inverse(dic_correlation, xi, nu, plan_xp)))


# In[45]:


def log_vraisemblance_penalisee_matern_gen(p, plan_xp, sorties, nu, liste_fonctions_tendance=None, pepite=0., param='standard'):
    if param == 'standard':
        return log_vraisemblance_penalisee_matern(p, plan_xp, sorties, nu, liste_fonctions_tendance, pepite)
    elif param == 'inverse':
        return log_vraisemblance_penalisee_matern_param_inverse(p, plan_xp, sorties, nu, liste_fonctions_tendance, pepite)
    elif param == 'log_inverse':
        return log_vraisemblance_penalisee_matern_param_log_inverse(p, plan_xp, sorties, nu, liste_fonctions_tendance, pepite)
    raise ValueError(param)
  

#log_vraisemblance_penalisee_matern(SCALE, plan_xp=points, sorties=sorties, nu=NU, liste_fonctions_tendance=LISTE_TENDANCE)


# In[60]:


#log_vraisemblance_penalisee_matern_param_inverse(1./np.array(SCALE), plan_xp=points, sorties=sorties, nu=NU, liste_fonctions_tendance=LISTE_TENDANCE)


# In[61]:


#log_vraisemblance_penalisee_matern_param_log_inverse(-np.log(SCALE), plan_xp=points, sorties=sorties, nu=NU, liste_fonctions_tendance=LISTE_TENDANCE)


# ## Courbes de vraisemblance
# 3 versions corresponant à chacune des 3 paramétrisations.
# 
# **Nota bene** : 
# 
# - dans tous les graphes, l'axe des abscisses représente le paramètre standard, i.e. $\boldsymbol{\theta}$, et ce même lorsque la paramétrisation utilisée pour pénaliser la vraisemblance est inverse ou log-inverse.
# - pour pouvoir représenter ces graphes, on ne considère que les valeurs du paramètre standard qui rendent le noyau isotrope. Par exemple, abscisse = 0.2 signifie que $\boldsymbol{\theta} = (0.2, 0.2, 0.2)$.

# In[115]:


def courbe_vraisemblance_penalisee_matern(seed, theta, plan_xp, tendance, nu, liste_fonctions_tendance=None, pepite=0.):
        ot.RandomGenerator.SetSeed(seed)
        modele_covariance = ot.MaternModel(theta, AMPLITUDE, nu)
        realisation = ot.GaussianProcess(ot.TrendTransform(tendance ,ot.Mesh(plan_xp)), modele_covariance,ot.Mesh(plan_xp)).getRealization()
        nb_abs = 50
        lin = np.linspace(1./nb_abs, 1, nb_abs).reshape(nb_abs, 1)
        mat = np.concatenate((lin,lin,lin), axis=1)
        posterior = np.apply_along_axis(log_vraisemblance_penalisee_matern, 1, mat, plan_xp=plan_xp, sorties=realisation.getValues(), nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite)
        prior = np.apply_along_axis(log_prior_jeffreys_matern, 1, mat, plan_xp=plan_xp, nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite)
        vraisemblance_integree = posterior - prior

        figure = plt.figure()
        ax = figure.add_subplot(111)
        line_posterior = ax.plot(lin, posterior, label='Vraisemblance pénalisée')
        line_vrais_integree = ax.plot(lin, vraisemblance_integree, label='Vraisemblance intégrée')
        titre = 'Paramétrisation standard'
        ax.set_title(titre)
        ax.set_xlabel('theta')
        ax.legend()
        figure.savefig(titre+'.pdf')


# In[113]:


def courbe_vraisemblance_penalisee_matern_param_inverse(seed, theta, plan_xp, tendance, nu, liste_fonctions_tendance=None, pepite=0.):
        ot.RandomGenerator.SetSeed(seed)
        modele_covariance = ot.MaternModel(theta, AMPLITUDE, nu)
        realisation = ot.GaussianProcess(ot.TrendTransform(tendance ,ot.Mesh(plan_xp)), modele_covariance,ot.Mesh(plan_xp)).getRealization()
        nb_abs = 50
        lin = np.linspace(1./nb_abs, 1, nb_abs).reshape(nb_abs, 1)
        mat = np.concatenate((lin,lin,lin), axis=1)
        posterior = np.apply_along_axis(log_vraisemblance_penalisee_matern_param_inverse, 1, 1./mat, plan_xp=plan_xp, sorties=realisation.getValues(), nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite)
        prior = np.apply_along_axis(log_prior_jeffreys_matern_param_inverse, 1, 1./mat, plan_xp=plan_xp, nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite)
        vraisemblance_integree = posterior - prior
        
        figure = plt.figure()
        ax = figure.add_subplot(111)
        line_posterior = ax.plot(lin, posterior, label='Vraisemblance pénalisée')
        line_vrais_integree = ax.plot(lin, vraisemblance_integree, label='Vraisemblance intégrée')
        titre = 'Paramétrisation inverse'
        ax.set_title(titre)
        ax.set_xlabel('theta')
        ax.legend()
        figure.savefig(titre+'.pdf')


# In[117]:


def courbe_vraisemblance_penalisee_matern_param_log_inverse(seed, theta, plan_xp, tendance, nu, liste_fonctions_tendance=None, pepite=0.):
        ot.RandomGenerator.SetSeed(seed)
        modele_covariance = ot.MaternModel(theta, AMPLITUDE, nu)
        realisation = ot.GaussianProcess(ot.TrendTransform(tendance ,ot.Mesh(plan_xp)), modele_covariance,ot.Mesh(plan_xp)).getRealization()
        nb_abs = 50
        lin = np.linspace(1./nb_abs, 1, nb_abs).reshape(nb_abs, 1)
        mat = np.concatenate((lin,lin,lin), axis=1)
        posterior = np.apply_along_axis(log_vraisemblance_penalisee_matern_param_log_inverse, 1, -np.log(mat), plan_xp=plan_xp, sorties=realisation.getValues(), nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite)
        prior = np.apply_along_axis(log_prior_jeffreys_matern_param_log_inverse, 1, -np.log(mat), plan_xp=plan_xp, nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite)
        vraisemblance_integree = posterior - prior

        figure = plt.figure()
        ax = figure.add_subplot(111)
        line_posterior = ax.plot(lin, posterior, label='Vraisemblance pénalisée')
        line_vrais_integree = ax.plot(lin, vraisemblance_integree, label='Vraisemblance intégrée')
        titre = 'Paramétrisation log-inverse'
        ax.set_title(titre)
        ax.set_xlabel('theta')
        ax.legend()
        figure.savefig(titre+'.pdf')


# In[116]:


def courbe_vraisemblance_penalisee_matern_gen(seed, theta, plan_xp, tendance, nu, liste_fonctions_tendance=None, pepite=0., param='standard'):
    ot.RandomGenerator.SetSeed(seed)
    modele_covariance = ot.MaternModel(theta, AMPLITUDE, nu)
    realisation = ot.GaussianProcess(ot.TrendTransform(tendance, ot.Mesh(plan_xp)), modele_covariance,ot.Mesh(plan_xp)).getRealization()
    nb_abs = 50
    lin = np.linspace(1./nb_abs, 1, nb_abs).reshape(nb_abs, 1)
    mat = np.concatenate((lin,lin,lin), axis=1)
    if param == 'standard':
        pass
    elif param == 'inverse':
        mat = 1./mat
    elif param == 'log_inverse':
        mat = -np.log(mat)
    else:
        raise ValueError(param)
    posterior = np.apply_along_axis(log_vraisemblance_penalisee_matern_gen, 1, mat, plan_xp=plan_xp, sorties=realisation.getValues(), nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite, param=param)
    prior = np.apply_along_axis(log_prior_jeffreys_matern_gen, 1, mat, plan_xp=plan_xp, nu=nu, liste_fonctions_tendance=liste_fonctions_tendance, pepite=pepite, param=param)
    vraisemblance_integree = posterior - prior

    figure = plt.figure()
    ax = figure.add_subplot(111)
    line_posterior = ax.plot(lin, posterior, label='Penalized likelihood')
    line_vrais_integree = ax.plot(lin, vraisemblance_integree, label='Integrated likelihood')
    ax.set_title('py prior=reference param='+ param)
    ax.set_xlabel('p')
    ax.legend()
    figure.savefig('py_refprior_'+param+'.png')


courbe_vraisemblance_penalisee_matern(seed=0, theta = SCALE, plan_xp=points, tendance=TREND, nu=NU, liste_fonctions_tendance=[un, coord1, coord2, coord3])

LISTE_TENDANCE=[ot.SymbolicFunction(['x1','x2', 'x3'], ['1']),ot.SymbolicFunction(['x1','x2', 'x3'], ['x1']),ot.SymbolicFunction(['x1','x2', 'x3'], ['x2']),ot.SymbolicFunction(['x1','x2', 'x3'], ['x3'])]
seed = 0


def courbe_vraisemblance_penalisee_matern_gen_ot(param):
    modele_covariance = ot.MaternModel(SCALE, AMPLITUDE, NU)
    parametrization = {'standard': ot.CovarianceModelImplementation.STANDARD, 'inverse': ot.CovarianceModelImplementation.INVERSE, 'log_inverse': ot.CovarianceModelImplementation.LOGINVERSE}[param]
    modele_covariance.setScaleParametrization(parametrization)
    ot.RandomGenerator.SetSeed(seed)
    realisation = ot.GaussianProcess(ot.TrendTransform(TREND ,ot.Mesh(points)), modele_covariance,ot.Mesh(points)).getRealization()
    krigeage = ot.KrigingAlgorithm(points,realisation.getValues(),modele_covariance,ot.Basis(LISTE_TENDANCE),False)

    krigeage.setScalePrior(ot.GeneralLinearModelAlgorithm.REFERENCE)
    llf = krigeage.getReducedLogLikelihoodFunction()
    nb_abs = 50
    lin = np.linspace(1./nb_abs, 1, nb_abs).reshape(nb_abs, 1)
    mat = np.concatenate((lin,lin,lin), axis=1)
    if param == 'standard':
        pass
    elif param == 'inverse':
        mat = 1./mat
    elif param == 'log_inverse':
        mat = -np.log(mat)
    else:
        raise ValueError(param)
    figure = plt.figure()
    ax = figure.add_subplot(111)
    posterior = [llf(mat[i])[0] for i in range(nb_abs)]
    ax.plot(lin, posterior, label='Penalized likelihood')
    ax.set_title('ot prior=reference param='+ param)
    ax.set_xlabel('p')
    ax.legend()
    figure.savefig('ot_refprior_'+param+'.png')

for param in ['standard', 'inverse', 'log_inverse']:
    courbe_vraisemblance_penalisee_matern_gen(seed=0, theta = SCALE, plan_xp=points, tendance=TREND, nu=NU, liste_fonctions_tendance=[un, coord1, coord2, coord3], param=param)
    courbe_vraisemblance_penalisee_matern_gen_ot(param)




#log_vraisemblance_penalisee_matern

# In[114]:


#courbe_vraisemblance_penalisee_matern_param_inverse(seed=0, theta = SCALE, plan_xp=points, tendance=TREND, nu=NU, liste_fonctions_tendance=[un, coord1, coord2, coord3])


# In[118]:


#courbe_vraisemblance_penalisee_matern_param_log_inverse(seed=0, theta = SCALE, plan_xp=points, tendance=TREND, nu=NU, liste_fonctions_tendance=[un, coord1, coord2, coord3])


