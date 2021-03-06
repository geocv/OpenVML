/* * Copyright (c) 2014, 2015                                          Zhang Xianyi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "vml_test.h"
#include <stdio.h>
#include <string.h>
#include <openvml_reference.h>

static char* funcname[4]={"vsTanh", "vdTanh", NULL,NULL};
static double flop_per_elem[4]={0.0, 0.0, 0.0, 0.0};

static a_y_func_t ref_vtanh[] = {
  (a_y_func_t)OpenVML_FUNCNAME_REF(vsTanh),
  (a_y_func_t)OpenVML_FUNCNAME_REF(vdTanh),
  NULL,
  NULL,
};

static a_y_func_t test_vtanh[] = {
  (a_y_func_t)OpenVML_FUNCNAME(vsTanh),
  (a_y_func_t)OpenVML_FUNCNAME(vdTanh),
  NULL,
  NULL,
};


CTEST2(check_result_s, tanh){
  run_test_a_y(data->parameter, funcname, test_vtanh, ref_vtanh, flop_per_elem);
}

CTEST2(check_result_d, tanh){
  run_test_a_y(data->parameter, funcname, test_vtanh, ref_vtanh, flop_per_elem);
}
