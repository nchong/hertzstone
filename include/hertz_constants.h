#pragma once
#ifndef HERTZ_CONSTANTS_H
#define HERTZ_CONSTANTS_H

#define sqrtFiveOverSix 0.91287092917527685576161630466800355658790782499663875

#ifdef __CUDACC__
#include "cuda_common.h"
#include "unpickle.h"

__constant__ double d_dt;
__constant__ double d_nktv2p;
__constant__ double d_yeff;
__constant__ double d_geff;
__constant__ double d_betaeff;
__constant__ double d_coeffFrict;

void setup_hertz_constants(struct params *p) {
  double dt = p->dt;
  double nktv2p = p->nktv2p;
  double yeff = p->yeff[3];
  double geff = p->geff[3];
  double betaeff = p->betaeff[3];
  double coeffFrict = p->coeffFrict[3];
  ASSERT_NO_CUDA_ERROR(
    cudaMemcpyToSymbol("d_dt", &dt, sizeof(double),
      0, cudaMemcpyHostToDevice));
  ASSERT_NO_CUDA_ERROR(
    cudaMemcpyToSymbol("d_nktv2p", &nktv2p, sizeof(double),
      0, cudaMemcpyHostToDevice));
  ASSERT_NO_CUDA_ERROR(
    cudaMemcpyToSymbol("d_yeff", &yeff, sizeof(double),
      0, cudaMemcpyHostToDevice));
  ASSERT_NO_CUDA_ERROR(
    cudaMemcpyToSymbol("d_geff", &geff, sizeof(double),
      0, cudaMemcpyHostToDevice));
  ASSERT_NO_CUDA_ERROR(
    cudaMemcpyToSymbol("d_betaeff", &betaeff, sizeof(double),
      0, cudaMemcpyHostToDevice));
  ASSERT_NO_CUDA_ERROR(
    cudaMemcpyToSymbol("d_coeffFrict", &coeffFrict, sizeof(double),
      0, cudaMemcpyHostToDevice));
}
#else

double d_dt;
double d_nktv2p;
double d_yeff;
double d_geff;
double d_betaeff;
double d_coeffFrict;

#endif

#endif
