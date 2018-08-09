// numeric.h
//

#pragma once

#include "lograd.h"
#include <vector>

using namespace std;

float rand_range(float range_lo, float range_hi);
double rand_normal(double mean, double stddev);

float soft_sign(float x);
float pd_soft_sign(float x);

float soft_mux(float s, float x0, float x1);
float pd_mux_s(float x0, float x1);
float pd_mux_x0(float s);
float pd_mux_x1(float s);

float logistic(float x);
float pd_logistic(float x);

float loss(float target, float actual);
float pd_loss(float target, float actual);

float thresher(float x);

float mean_sq_error(vector<float> *target, vector<float> *actual);
float mean_sq_error(vector<float> *diffs);

// Clayton Bauman 2018

