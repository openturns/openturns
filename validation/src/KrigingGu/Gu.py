#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct 30 08:49:05 2019

@author: Joseph MURÉ
"""
from operator import mul
from functools import reduce
import numpy as np
import openturns as ot
from scipy.optimize import minimize

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
        return 1.
    expression = np.sqrt(2 * nu) * np.linalg.norm(x / theta)
    return np.exp((1. - nu) * np.log(2.)) / ot.SpecFunc_Gamma(nu) * \
           np.exp(nu * np.log(expression)) * ot.SpecFunc_BesselK(nu, expression)

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
    return nu / ot.SpecFunc_Gamma(nu) / np.power(2., nu-2.) * x[indice_derive]**2 * \
           theta[indice_derive]**(-3) * \
           np.power(expression, nu-1.) * ot.SpecFunc_BesselK(nu - 1., expression)
           

class Gu:
    def __init__(self, input_Sample, output_Sample, trend_Basis, initial_CovarianceModel, prior='reference', parametrization='standard'):
        if type(input_Sample)!=ot.typ.Sample:
            raise TypeError("input_Sample should be an OpenTURNS Sample, not be of {}.".format(type(input_Sample)))
        if type(output_Sample)!=ot.typ.Sample:
            raise TypeError("output_Sample should be an OpenTURNS Sample, not be of {}.".format(type(input_Sample)))
        if output_Sample.getDimension()!=1:
            raise ValueError("output_Sample should be of dimension 1.")
        if output_Sample.getSize()!=input_Sample.getSize():
            message = "input_Sample has size {} and ".format(input_Sample.getSize())
            message += "output_Sample has size {}, ".format(output_Sample.getSize())
            message += "but they should have the same size."
            raise ValueError(message)
        if type(trend_Basis)!=ot.func.Basis:
            raise TypeError("trend_Basis should be an OpenTURNS Basis, not be of {}.".format(type(trend_Basis)))
            
        if type(initial_CovarianceModel)==ot.statistics.ProductCovarianceModel:
            collection = initial_CovarianceModel.getCollection()

            for model in collection:
                if model.getImplementation().getClassName()!='MaternModel':
                    raise TypeError("In a ProductCovarianceModel, all models should be Matern, not be of type {}.".format(model.getImplementation().getClassName()))
                if model.getFullParameter()[-1]!=collection[0].getFullParameter()[-1]:
                    raise TypeError("In a ProductCovarianceModel, all models should have the same nu.")
            
            self._nu = collection[0].getFullParameter()[-1]
        elif type(initial_CovarianceModel)!=ot.statistics.MaternModel:
            raise TypeError("initial_CovarianceModel should be an OpenTURNS MaternModel, not be of {}.".format(type(initial_CovarianceModel)))
        else:
            self._nu = initial_CovarianceModel.getNu()
        
        if initial_CovarianceModel.getScale().getSize()!=input_Sample.getDimension():
            initial_scale = str(list(np.array(initial_CovarianceModel.getScale())))
            message = "The scale parameter of initial_CovarianceModel is {}, ".format(initial_scale)
            message += "but input_Sample is of dimension {}. ".format(input_Sample.getDimension())
            message += "The dimensions do not match."
            raise ValueError(message)
        
        if parametrization not in ['standard', 'inverse', 'log-inverse']:
            raise ValueError("parametrization should be 'standard', 'inverse' or 'log-inverse'.")
        
        if prior not in ['reference', 'jointly-robust']:
            raise ValueError("prior should be either 'reference' or 'jointly-robust'.")
        
        self._parametrization = parametrization
        self._prior = prior

        self._input_Sample = input_Sample # Should it be a deepcopy?
        self._algebraic_gaps = self.compute_algebraic_gaps()
        self._output_Sample = output_Sample
        self._trend_Basis = trend_Basis
        self._current_CovarianceModel = initial_CovarianceModel
        
        self._W, self._H = self.compute_W()
        if self._trend_Basis.getSize()!=0:
            self._additional_loglikelihood_term = float(-0.5 * np.log(np.linalg.det(np.transpose(self._H) @ self._H)))
        else:
            self._additional_loglikelihood_term = 0.0
        
        self._restricted_output = np.transpose(self._W) @ output_Sample
        
        if type(initial_CovarianceModel)==ot.statistics.ProductCovarianceModel:
            self._list_dim1_MaternModels = []
            placeholder_output_Sample = ot.Sample(np.ones((input_Sample.getSize(),1)))
            for index, length_scale in enumerate(initial_CovarianceModel.getScale()):
                dim1_input_Sample = input_Sample.getMarginal(index)
                dim1_CovarianceModel = ot.MaternModel([length_scale], [1.0], self._nu)
                self._list_dim1_MaternModels.append(Gu(dim1_input_Sample, placeholder_output_Sample, ot.Basis(0), dim1_CovarianceModel))
                
        if self._prior=='jointly-robust':
            self._b0 = 1.0
            self._b1 = 0.2
            self._b = self._b0 * np.exp(-1./self._input_Sample.getDimension() * np.log(self._input_Sample.getSize())) * (self._input_Sample.getDimension() + self._b1)
            #non_zero_gaps = self._input_Sample.getSize() * (self._input_Sample.getSize() - 1)
            #self._Ca = np.apply_over_axes(a=np.abs(self.compute_algebraic_gaps()), axes=(0,1), func=np.sum)[0, 0, :] / non_zero_gaps
            self._Ca = np.apply_over_axes(a=self.compute_algebraic_gaps(), axes=(0,1), func=np.max)[0, 0, :] / self._input_Sample.getSize()**(1./self._input_Sample.getDimension())
        
        self.update_current_correlation()
        #self.update_current_log_likelihood()

        
    
    def compute_W(self):
        if self._trend_Basis.getSize()==0:
            return np.identity(self._input_Sample.getSize()), 0
        else:
            if self._trend_Basis.getSize()>=self._input_Sample.getSize():
                message = "Size of trend_Basis {} >= ".format(self._trend_Basis.getSize())
                message += "size of input/output_Sample {}.".format(self._input_Sample.getSize())
                raise ValueError(message)
                
            matrice_H = self._trend_Basis.build(0)(self._input_Sample)
            for dim in range(1, self._trend_Basis.getSize()):
                matrice_H = np.concatenate((matrice_H, self._trend_Basis.build(dim)(self._input_Sample)), axis=1)
            
            u, s, vh = np.linalg.svd(matrice_H)
            return u[:, self._trend_Basis.getSize():], matrice_H
    
    def compute_algebraic_gaps(self):
        soustractions = np.subtract.outer(self._input_Sample, self._input_Sample)
        ecarts_algebriques = np.zeros((soustractions.shape[0], soustractions.shape[2], soustractions.shape[1]))
        for indice in range(ecarts_algebriques.shape[2]):
            ecarts_algebriques[:, :, indice] = soustractions[:, indice, :, indice]
        return ecarts_algebriques
        
    def compute_matern_correlation_matrix(self):
        """Renvoie la matrice de corrélation correspondant à un noyau de Matérn utilisant la paramétrisation standard.
        
        Arguments :
            nu : paramètre de régularité
            plan_xp : ot.Sample contenant le plan d'expériences
            theta : np.array de même taille que x contenant les longueurs de corrélation
        
        """
        if type(self._current_CovarianceModel)==ot.statistics.MaternModel:
            matrice_corr = np.apply_along_axis(matern, 
                                               2, 
                                               self._algebraic_gaps, 
                                               nu=self._nu, 
                                               theta=self._current_CovarianceModel.getScale())
        else:
            matrices_corr_1d = [gu_instance._current_correlation_matrix for gu_instance in self._list_dim1_MaternModels]
            matrice_corr = reduce(mul, matrices_corr_1d, 1)
        return matrice_corr

    def compute_matern_derivative_matrix(self, diff_index):
        """Renvoie la matrice de corrélation dérivée par rapport à la indice_derive-ième longeur de corrélation pour un noyau de Matérn.
        
        Arguments :
            indice_derive : numéro de la longueur de corrélation par rapport à laquelle est faite la dérivation
            nu : paramètre de régularité
            plan_xp : ot.Sample contenant le plan d'expériences
            theta : np.array de même taille que x contenant les longueurs de corrélation
        
        """
        if type(self._current_CovarianceModel)==ot.statistics.MaternModel:
            matrice_corr_derivee = np.apply_along_axis(derivee_matern, 
                                                       2, 
                                                       self._algebraic_gaps, 
                                                       indice_derive=diff_index, 
                                                       nu=self._nu, 
                                                       theta=self._current_CovarianceModel.getScale())
        else:
            matrices_corr_1d_without_diff_index = [gu_instance._current_correlation_matrix for 
                                                   gu_instance in self._list_dim1_MaternModels[0:diff_index] \
                                                   + self._list_dim1_MaternModels[diff_index+1:]]
            matrice_corr_derivee = reduce(mul, matrices_corr_1d_without_diff_index, 1)
            #print(matrice_corr_derivee)
            matrice_corr_derivee *= self._list_dim1_MaternModels[diff_index].compute_matern_derivative_matrix(0)
        return matrice_corr_derivee
    
    def element_info_fisher_matern(self, ligne, colonne):
        """Renvoie un élément de la matrice d'information de Fisher."""
        if ligne==self._input_Sample.getDimension() or colonne==self._input_Sample.getDimension():
            if ligne==self._input_Sample.getDimension() and colonne==self._input_Sample.getDimension():
                return self._input_Sample.getSize() - self._trend_Basis.getSize()
            derivee = np.transpose(self._W) @ self.compute_matern_derivative_matrix(min(ligne, colonne)) @ self._W
            return np.trace(self._current_correlation_inverse @ derivee)
        derivee_lig = np.transpose(self._W) @ self.compute_matern_derivative_matrix(ligne) @ self._W 
        facteur_lig = self._current_correlation_inverse @ derivee_lig
        derivee_col = np.transpose(self._W) @ self.compute_matern_derivative_matrix(colonne) @ self._W 
        facteur_col = self._current_correlation_inverse @ derivee_col
        return np.trace(np.matmul(facteur_lig, facteur_col)) 
    
    def info_fisher_matern(self):
        """Renvoie la matrice d'information de Fisher"""
        nb_lignes_colonnes = self._input_Sample.getDimension()+1
        matrice_info_fisher = np.zeros((nb_lignes_colonnes, nb_lignes_colonnes))
        for ligne in range(nb_lignes_colonnes):
            for colonne in range(nb_lignes_colonnes):
                if colonne>=ligne:
                    matrice_info_fisher[ligne, colonne] = self.element_info_fisher_matern(ligne, colonne)
                else:
                    matrice_info_fisher[ligne, colonne] = matrice_info_fisher[colonne, ligne]
        return matrice_info_fisher
    
    def update_current_correlation(self):
        nugget = self._current_CovarianceModel.getNuggetFactor()
        
        if type(self._current_CovarianceModel)==ot.statistics.ProductCovarianceModel:
            for index, gu_instance in enumerate(self._list_dim1_MaternModels):
                gu_instance._current_CovarianceModel.setScale([self._current_CovarianceModel.getScale()[index]])
                gu_instance.update_current_correlation()
        
        correlation_matrix = self.compute_matern_correlation_matrix()
        correlation_matrix += nugget * np.identity(self._input_Sample.getSize())
        
        self._current_correlation_matrix = np.transpose(self._W) @ correlation_matrix @ self._W
        #self._current_correlation_inverse = np.linalg.inv(self._current_correlation_matrix)
        
    def update_current_correlation_inverse(self):
        self._current_correlation_inverse = np.linalg.inv(self._current_correlation_matrix)
        
    
    def update_current_log_likelihood(self):        
        mahalonobis = np.transpose(self._restricted_output) @ self._current_correlation_inverse @ self._restricted_output

        log_likelihood = -0.5 * np.log(np.linalg.det(self._current_correlation_matrix))
        log_likelihood -= 0.5 * self._restricted_output.size * np.log(mahalonobis)
        
        # To be consistent with Julien's implementation
        log_likelihood += self._additional_loglikelihood_term
        
        self._current_log_likelihood = float(log_likelihood)
        
    def update_current_log_prior(self): 
        if self._prior=='reference':
            self._current_log_prior = float(0.5 * np.log(np.linalg.det(self.info_fisher_matern())))
        else:
            main_term = (self._Ca  / np.array(self._current_CovarianceModel.getScale())).sum() + self._current_CovarianceModel.getNuggetFactor()
            self._main_term = main_term
            self._current_log_prior = self._b1 * np.log(main_term) - self._b * main_term - 2 * np.log(self._current_CovarianceModel.getScale()).sum()
        
        if self._parametrization=='inverse':
            self._current_log_prior += 2 * np.log(self._current_CovarianceModel.getScale()).sum()
        
        if self._parametrization=='log-inverse':
            self._current_log_prior += np.log(self._current_CovarianceModel.getScale()).sum()
    
    def set_scale(self, scale_vector):
        if (np.array(scale_vector)<1e-12).sum()>0:
            return np.inf
        self._current_CovarianceModel.setScale(scale_vector)
        self.update_current_correlation()
        self.update_current_correlation_inverse()
        self.update_current_log_likelihood()
        self.update_current_log_prior()
        return - self._current_log_prior - self._current_log_likelihood  #the optimizer minimizes
    
    def optimize_scale(self):
        #bounds = [(1e-12,10)] * self._input_Sample.getDimension()
        return minimize(self.set_scale, self._current_CovarianceModel.getScale())#, bounds=bounds)
        
    


