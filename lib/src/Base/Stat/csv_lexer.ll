/*                                               -*- C++ -*- */
/*
 * @brief The lexer definition in order to read CSV files
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

%{

#include <sstream>
#include <locale>
#include <stdlib.h>
#include "csv_parser.hh"

#define YY_DECL      int csvlex(YYSTYPE *yylval_param, yyscan_t yyscanner, FILE * theFile, const char * theSeparator)
#define YY_USER_INIT {yyin = theFile;}
%}

%option bison-bridge
%option yylineno
%option reentrant
%option prefix="csv"
%option noyywrap
%option nounput

digit     [0-9]
sign      [+-]
integer   {digit}+
exponent  [eE]{sign}?{integer}
real      {sign}?({integer}("."({integer})?)?|"."{integer}){exponent}?

string    [[:alnum:]!#\$%&'()\*\+\-\./\:\<=\>\?@\[\\\]\^_`\{\|\}~]*

space     [ \t]

%%

{real} { yylval_param->real = atof(yytext); return(REAL); }

{string}    { yylval_param->st = yytext; return(STRING); }

"\n"        { return(CRLF); }

"\r\n"      { return(CRLF); }

"\""        { return(DOUBLEQUOTE); }

{space}     { return(SPACE); }

.           { yylval_param->st = yytext; if (strspn(yytext,theSeparator)) return(COMMA); else return(CHARACTER); }
