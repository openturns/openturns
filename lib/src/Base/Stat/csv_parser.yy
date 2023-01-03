/*                                               -*- C++ -*- */
/*
 * @brief The parser definition in order to read CSV files
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

/* This parser is based on RFC 4180 from www.ietf.org */

%define api.pure
%parse-param {OT::CSVParserState & theState}
%parse-param {yyscan_t yyscanner}
%parse-param {FILE * theFile}
%parse-param {OT::SampleImplementation &impl}
%parse-param {OT::UnsignedInteger & theDimension}
%parse-param {const char * theSeparator}
%lex-param   {yyscan_t yyscanner}
%lex-param   {FILE * theFile}
%lex-param   {const char * theSeparator}
%defines
%name-prefix "csv"

%union {
  double real;
  const char * st;
}

%token <real>      REAL
%token <st>        STRING
%token <st>        COMMA
%token             CRLF
%token <st>        SPACE
%token             DOUBLEQUOTE
%token             CHARACTER

%left              COMMA
%left              CRLF

%type <st>         escapedData
%type <st>         data
%type <st>         CRLF
%type <st>         DOUBLEQUOTE
%type <st>         error



%code requires {
#include <string>
#include "openturns/csv_parser_state.hxx"
#include "openturns/SampleImplementation.hxx"
typedef void*                 yyscan_t;
}

%code {
#include "openturns/Log.hxx"

int yylex                     (YYSTYPE *lvalp, yyscan_t yyscanner, FILE * theFile, const char * theSeparator);
int yyerror                   (OT::CSVParserState & theState, yyscan_t yyscanner, FILE * theFile, OT::SampleImplementation &impl, OT::UnsignedInteger & theDimension, const char * theSeparator, const char *s);
int csvget_lineno             (yyscan_t yyscanner);


static std::string ToString(double val)
{
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

static void ClearPoint(OT::CSVParserState & theState, yyscan_t /*yyscanner*/)
{
  theState.point = OT::Point();
}

static void PrintPoint(OT::CSVParserState & theState, yyscan_t yyscanner, OT::SampleImplementation &impl, OT::UnsignedInteger & theDimension)
{
  LOGDEBUG(OT::OSS() << "file " << theState.theFileName << " line " << csvget_lineno(yyscanner) << ": point=" << theState.point.__repr__());

  if (theDimension == 0) {
    theDimension = theState.point.getDimension();
    impl.add(theState.point);
  } else if (theDimension == theState.point.getDimension()) impl.add(theState.point);
}


static void ClearHeader(OT::CSVParserState & theState, yyscan_t /*yyscanner*/)
{
  theState.Header = OT::Description();
}

static void PrintHeader(OT::CSVParserState & theState, yyscan_t yyscanner, OT::SampleImplementation &impl, OT::UnsignedInteger & theDimension)
{
  LOGDEBUG(OT::OSS() << "file " << theState.theFileName << " line " << csvget_lineno(yyscanner) << ": Header=" << theState.Header.__repr__());

  if (theDimension == 0) {
    theDimension = theState.Header.getSize();
    impl.setDescription(theState.Header);
  }
}
}


%start CSVFile
%%

CSVFile:  endOfFile
	| recordSet endOfFile
	;

recordSet: Record 
	|  recordSet CRLF Record 
	;

Record: { ClearHeader(theState,yyscanner); ClearPoint(theState,yyscanner); theState.errors = false; }
	record
        { if (!theState.errors) { PrintHeader(theState,yyscanner,impl,theDimension); PrintPoint(theState,yyscanner,impl,theDimension); } } ;

record:   spacedField
	| record COMMA spacedField
	| record error
	;

spacedField: field
	| spaces field
        | field spaces
        | spaces field spaces
        ;

spaces: SPACE
	| spaces SPACE
	;

field:    { theState.St = ""; } escapedField    { theState.Header.add(theState.St); }
	  | { theState.St = ""; } nonEscapedField { if (theState.Type==OT::CSVParserState::RealField) theState.point.add(theState.Val); else theState.Header.add(theState.St); }
	;



escapedField: { theState.Type = OT::CSVParserState::StringField; } DOUBLEQUOTE escapedDataList DOUBLEQUOTE ;

escapedDataList: escapedData 
	|        escapedDataList escapedData
	;

escapedData: data
	|    SPACE                   { theState.St += ' '; }
	|    COMMA                   { theState.St += $1; }
	|    CRLF                    { theState.St += '\n'; }
	|    DOUBLEQUOTE DOUBLEQUOTE { theState.St += '"'; }
	;

nonEscapedField: { theState.Type = OT::CSVParserState::NotSet; } data 
	|        { theState.Type = OT::CSVParserState::NotSet; } error data
	;

data:     REAL   { if (theState.Type==OT::CSVParserState::NotSet) { theState.Val = $1; theState.Type = OT::CSVParserState::RealField; } else { theState.St += ToString($1); }  }
	| STRING { theState.St += $1; theState.Type = OT::CSVParserState::StringField; }
	;

endOfFile: /* Empty */
	| CRLF
	;

%%

int yyerror(OT::CSVParserState & theState, yyscan_t yyscanner, FILE * /*theFile*/, OT::SampleImplementation &/*impl*/, OT::UnsignedInteger & /*theDimension*/, const char * /*theSeparator*/, const char *s) {
  LOGINFO(OT::OSS() << "file " << theState.theFileName << " line " << csvget_lineno(yyscanner) << " is ignored: " << s);
  theState.errors = true;
  return 0;
}
