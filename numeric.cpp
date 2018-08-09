// numeric.cpp

#include "numeric.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                              UTILITIES                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

// uniform distribution random variable
//
float rand_range(float range_lo, float range_hi){

    float rand01 = (1.0f * (float)rand()) / (float)RAND_MAX;
    float range_mag = range_hi-range_lo;
    return ((float)rand01 * range_mag + range_lo);

}


// normal distribution random variable
//
double rand_normal(double mean, double stddev){ //Box muller method

    static double n2 = 0.0;
    static int n2_cached = 0;

    if (!n2_cached){

        double x, y, r;
        do{

            x = 2.0*rand()/RAND_MAX - 1;
            y = 2.0*rand()/RAND_MAX - 1;

            r = x*x + y*y;

        }
        while (r == 0.0 || r > 1.0);{

            double d = sqrt(-2.0*log(r)/r);
            double n1 = x*d;
            n2 = y*d;
            double result = n1*stddev + mean;
            n2_cached = 1;
            return result;

        }
    }
    else{

        n2_cached = 0;
        return n2*stddev + mean;

    }

}


//
//
float soft_sign(float x){

    return x / (1 + fabs(x));

}


//
//
float pd_soft_sign(float x){

    float denom = (1 + fabs(x));
    return 1 / (denom*denom);

}


//
//
float soft_mux(float s, float x0, float x1){

    return (1.0f-s)*x0 + s*x1;

}


//
//
float pd_mux_s(float x0, float x1){

    return x1-x0;

}


//
//
float pd_mux_x0(float s){

    return 1-s;

}


//
//
float pd_mux_x1(float s){

    return s;

}


//
//
float logistic(float x){

    return 1.0f / (1.0f + expf(-1*x));

}


// closed-form derivative of logistic function
//
float pd_logistic(float x){

//    return x * (1.0f - x);
    float root_denom = (1.0f + exp(x));
    return exp(x) / (root_denom * root_denom);

}


//
//
float loss(float target, float actual){

    float   root_loss = (target - actual);
    return (root_loss*root_loss)/2;

}


//
//
float pd_loss(float target, float actual){

    return (actual-target);

}


//
//
float thresher(float x){

    return (x >= 0.5) ? 1 : 0;

}


//
//
float mean_sq_error(vector<float> *target, vector<float> *actual){

    int num_terms = target->size();
    float accumulator = 0;
    float diff;

    for(int i=0; i<num_terms; i++){
        diff = (*target)[i]-(*actual)[i];
        accumulator += (diff*diff);
    }

    return (accumulator / (float)num_terms);

}


//
//
float mean_sq_error(vector<float> *diffs){

    int num_terms = diffs->size();
    float accumulator = 0;

    for(int i=0; i<num_terms; i++){
        accumulator += ((*diffs)[i] * (*diffs)[i]);
    }

    return (accumulator / (float)num_terms);

}


// Clayton Bauman 2018

