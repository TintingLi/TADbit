#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include "tadbit.h"

// Declare and register R/C interface.
SEXP
tadbit_R_call(
  SEXP list,
  SEXP max_tad_size,
  SEXP n_threads,
  SEXP verbose
);

R_CallMethodDef callMethods[] = {
   {"tadbit_R_call", (DL_FUNC) &tadbit_R_call, 4},
   {NULL, NULL, 0}
};

void R_init_tadbit(DllInfo *info) {
   R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}


SEXP
tadbit_R_call(
  SEXP list,
  SEXP max_tad_size,
  SEXP n_threads,
  SEXP verbose
){

/*
   * This is a tadbit wrapper for R. The matrices have to passed
   * in a list (in R). Checks that the input consists of numeric
   * square matrices, with identical dimensions. The list is
   * is converted to pointer of pointers to doubles and passed
   * to 'tadbit'.
   * Assume that NAs can be passed from R and are ignored in the
   * computation.
*/

   R_len_t i, m = length(list);
   int first = 1, n, *dim;

   // Convert 'obs_list' to pointer of pointer to double.
   double **obs = (double **) malloc(m * sizeof(double **));
   for (i = 0 ; i < m ; i++) {
      // This fails is list element is not numeric.
      obs[i] = REAL(coerceVector(VECTOR_ELT(list, i), REALSXP));
      // Check that input is a matrix.
      if (!isMatrix(VECTOR_ELT(list, i))) {
         error("input must be square matrix");
      }
      // Check the dimension.
      dim = INTEGER(getAttrib(VECTOR_ELT(list, i), R_DimSymbol));
      if (dim[0] != dim[1]) {
         error("input must be square matrix");
      }
      if (first) {
         n = dim[0];
         first = 0;
      }
      else {
         if (n != dim[0]) {
            error("all matrices must have same dimensions");;
         }
      }
   }

   SEXP return_val_sexp;
   PROTECT(return_val_sexp = allocVector(INTSXP, n*n));
   int *return_val = INTEGER(return_val_sexp);
   
   // Call 'tadbit'.
   tadbit((const double **) obs, n, m, REAL(max_tad_size)[0],
         INTEGER(n_threads)[0], INTEGER(verbose)[0], return_val);

   UNPROTECT(1);
   return return_val_sexp;

}

