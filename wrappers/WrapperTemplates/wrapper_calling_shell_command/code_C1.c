/*
 * file: code_C1.c
 *
 * This is code that computes a small analytical formula.
 * Its data are read from a regular file named 'code_C1.data'
 * and its result is a single regular file named 'code_C1.result'.
 *
 * The 'code_C1.data' file is structured as follows:
 * # a comment
 * I1 = <double>
 * I2 = <double>
 * I3 = <doouble>
 *
 * The 'code_C1.result' file is structured as follows:
 * # a comment
 * O1 = <double> O2 = <double>
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>
#include <string.h>
#include <math.h>


#if WIN32
// not defined in mingw
size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
                return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
                size = size + 128;
                bufptr = realloc(bufptr, size);
                if (bufptr == NULL) {
                        return -1;
                }
        }
        *p++ = c;
        if (c == '\n') {
                break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif




struct input {
  double i1, i2, i3;
};

struct output {
  double o1, o2;
};

/*
 * This function opens and reads the input file.
 * It returns the input values in a structure.
 * The returned code is non-zero if case of a failure.
 */
int readInputFile ( const char * filename, struct input * in )
{
  int rc = 0;
  FILE * f = 0;
  char * line = 0;
  size_t n;
  regex_t re_comment;
  regex_t re_blankline;
  regex_t re_variable;
  size_t nmatch = 0;
  regmatch_t * pmatch = 0;

  rc = ( !filename ) | ( !in );
  if ( rc ) goto ERR;

  /* We create some regular expressions to help parsing */
  rc = regcomp ( &re_comment,   "^[ \t]*#", REG_EXTENDED | REG_NOSUB );
  if ( rc ) goto ERR;
  rc = regcomp ( &re_blankline, "^[ \t]*$", REG_EXTENDED | REG_NOSUB );
  if ( rc ) goto ERR;
  rc = regcomp ( &re_variable,  "^[ \t]*([a-zA-Z_][a-zA-Z0-9_]*)[ \t]*=[ \t]*([+-]?(([0-9]+([.][0-9]*)?)|([.][0-9]+))([eE][+-]?[0-9]+)?)[ \t]*$", REG_EXTENDED );
  if ( rc ) goto ERR;

  /* We allocate the structure receiving the parsed sub-expressions */
  nmatch = 7 + 1;
  pmatch = ( regmatch_t * ) calloc ( nmatch, sizeof ( regmatch_t ) );

  /* We open the file */
  f = fopen ( filename, "r" );
  if ( !f ) {
    fprintf ( stderr, " ==> error in opening file '%s': %s\n" , filename, strerror(errno) );
    rc = 1;
    goto ERR;
  }

  n = 256;
  line = ( char * ) calloc ( n, sizeof ( char ) );
  
  /* We read each line in turn */
  while ( getline ( &line, &n, f ) != -1 ) {

    /* We remove CR from the end of the line */
    char * pos = strchr ( line, '\n' );
    if ( pos ) *pos = 0;

    /* We skip blank lines and comments */
    int rc = 0;
    rc = regexec ( &re_comment, line, 0, 0, 0 );
    if ( !rc ) {
#ifdef DEBUG
      fprintf ( stderr, " ==> seen comment ( line = '%s' )\n", line );
#endif
      continue;
    }
    rc = regexec ( &re_blankline, line, 0, 0, 0 );
    if ( !rc ) {
#ifdef DEBUG
      fprintf ( stderr, " ==> seen blank line ( line = '%s' )\n", line );
#endif
      continue;
    }
    
    /* Here we have a 'valid' line */
    rc = regexec ( &re_variable, line, nmatch, pmatch, 0 );
    if ( rc ) {
#ifdef DEBUG
      char * buf = 0;
      size_t len = 0;
      fprintf ( stderr, " ==> seen unknown line ( line = '%s' )\n", line );
      len = regerror ( rc, &re_variable, buf, len );
      buf = ( char * ) calloc ( len, sizeof ( char ) );
      len = regerror ( rc, &re_variable, buf, len );
      fprintf ( stderr, " ==> reason: %s\n", buf );
      free ( buf );
#endif
      continue;
    }
    {
      size_t var_len = 0;
      char * var = 0;
      size_t val_len = 0;
      char * val = 0;

      var_len = pmatch[1].rm_eo - pmatch[1].rm_so;
      var = ( char * ) calloc ( var_len + 1, sizeof ( char ) );
      strncpy ( var, line + pmatch[1].rm_so, var_len );
      var[var_len] = 0;

      val_len = pmatch[2].rm_eo - pmatch[2].rm_so;
      val = ( char * ) calloc ( val_len + 1, sizeof ( char ) );
      strncpy ( val, line + pmatch[2].rm_so, val_len );
      val[val_len] = 0;

#ifdef DEBUG
      fprintf ( stderr, " ==> seen valid line ( line = '%s' ) : VAR is '%s', VALUE is '%s'\n", line, var, val );
#endif

      if ( ! strcmp ( var, "I1" ) ) in->i1 = strtod ( val, 0 );
      if ( ! strcmp ( var, "I2" ) ) in->i2 = strtod ( val, 0 );
      if ( ! strcmp ( var, "I3" ) ) in->i3 = strtod ( val, 0 );
    }
  }

ERR:
  free ( line );
  fclose ( f );

  regfree( &re_comment );
  regfree( &re_blankline );
  regfree( &re_variable );

  /* OK */
  return rc;
}



/*
 * This function computes some trivial analytical formula.
 * The returned code is non-zero if case of a failure.
 */
int doComputation ( const struct input * in, struct output * out )
{
  int rc = 0;
  
  rc = ( !in ) | ( !out );
  if ( rc ) goto ERR;

  /* The real work is here */
  out->o1 = in->i1 * sqrt ( in->i2 * in->i3 );
  out->o2 = 3. * in->i1 * in->i1 + in->i2 * in->i3;

ERR:
  /* OK */
  return rc;
}




/*
 * This function opens and writes the input file.
 * It reads the output values in a structure.
 * The returned code is non-zero if case of a failure.
 */
int writeOutputFile ( const char * filename, struct output * out )
{
  int rc = 0;
  FILE * f = 0;

  rc = ( !filename ) | ( !out );
  if ( rc ) goto ERR;

  /* We open the file */
  f = fopen ( filename, "w" );
  if ( !f ) {
    fprintf ( stderr, " ==> error in opening file '%s': %s\n" , filename, strerror(errno) );
    rc = 1;
    goto ERR;
  }

  /* We write the values */
  fprintf ( f, "O1 = %g O2 = %g\n", out->o1, out->o2 );

ERR:
  fclose ( f );

  /* OK */
  return rc;
}



/*
 * The main function that is first called when the program runs.
 */
int main ( int argc, char * argv[] )
{
  int rc = 0;
  struct input in;
  struct output out;

#ifdef DEBUG
  fprintf ( stderr, "==> code_C1 is starting\n" );
#endif

  /* We parse the command line */
  if ( argc != 3 ) {
    fprintf ( stderr, "usage: code_C1 code_C1.data code_C1.result\n" );
    rc = 1;
    goto ERR;
  }

  /* We open the input file and read its content */
  rc = readInputFile ( argv[1], &in );  
  if ( rc ) goto ERR;

#ifdef DEBUG
  fprintf ( stderr, "==> code_C1 has read { i1 = %g, i2 = %g, i3 = %g }\n", in.i1, in.i2, in.i3 );
#endif

  /* We realize the computation */
  rc = doComputation ( &in, &out );
  if ( rc ) goto ERR;

#ifdef DEBUG
  fprintf ( stderr, "==> code_C1 has computed { o1 = %g, o2 = %g }\n", out.o1, out.o2 );
#endif

  /* We create the output file */
  rc = writeOutputFile ( argv[2], &out );
  if ( rc ) goto ERR;

#ifdef DEBUG
  fprintf ( stderr, "==> code_C1 has written result file\n" );
#endif

ERR:
#ifdef DEBUG
  fprintf ( stderr, "==> code_C1 is ending\n" ) ;
#endif
  return rc;
}

