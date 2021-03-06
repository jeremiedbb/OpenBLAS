/***************************************************************************
 * Copyright (c) 2013, The OpenBLAS Project
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * 3. Neither the name of the OpenBLAS project nor the names of
 * its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * *****************************************************************************/

/**************************************************************************************
* 2014/05/22 Saar
* 	TEST double precision unblocked : OK
* 2014/05/23 Saar
* 	TEST double precision blocked: OK
* 	TEST single precision blocked: OK
**************************************************************************************/

#include <stdio.h>
#include "common.h"

// static FLOAT dp1 =  1.;
// static FLOAT dm1 = -1.;


#ifdef UNIT
#define TRTI2   TRTI2_LU
#define TRMM    TRMM_LNLU
#define TRSM    TRSM_RNLU
#else
#define TRTI2   TRTI2_LN
#define TRMM    TRMM_LNLN
#define TRSM    TRSM_RNLN
#endif


blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG j, n, lda;
  FLOAT *a;

  // BLASLONG info=0;
  BLASLONG jb;
  BLASLONG NB;
  BLASLONG start_j;

  FLOAT beta_plus[2]   = { ONE, ZERO};
  FLOAT beta_minus[2]  = {-ONE, ZERO};

  n      = args -> n;

  NB = GEMM_Q;

  if (n < NB) {
    TRTI2(args, NULL, range_n, sa, sb, 0);
    return 0;
  }


  lda	 = args -> lda;
  a	 = (FLOAT *) args -> a;
  args -> ldb = lda;
  args -> ldc = lda;
  args -> alpha = NULL;

  start_j = 0;
  while (start_j < n) start_j += NB;
  start_j -= NB;


  for (j = start_j ; j >=0 ; j-= NB)
  {
	jb = n - j;
	if ( jb > NB ) jb = NB;

	args -> n = jb;
	args -> m = n-j-jb;

	args -> a = &a[(j+jb+(j+jb)*lda) * COMPSIZE];
	args -> b = &a[(j+jb+j*lda) * COMPSIZE];
	args -> beta = beta_plus;

        TRMM(args, NULL, NULL, sa, sb, 0);

	args -> a = &a[(j+j*lda) * COMPSIZE];
	args -> beta = beta_minus;

        TRSM(args, NULL, NULL, sa, sb, 0);

	args -> a = &a[(j+j*lda) * COMPSIZE];

        TRTI2(args, NULL, range_n, sa, sb, 0);

  }
  return 0;
}
