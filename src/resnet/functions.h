#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void conv2d_ref(float* output, float* input, float* weight, int F, int E, int W, int H, int C, int S, int R, int M, int STRIDE, int PADDING);

void maxpool2d(float* output, float* input, int F, int E, int W, int H, int C, int S, int R, int STRIDE, int PADDING);

void batchnorm2d(float* inout, float* weight, int W, int H, int C);

void relu(float* inout, int W, int H, int C);

void fc_layer(float* temp, float* inout, float* weight, int W, int H, int C, int M);


#endif /* FUNCTIONS_H */
