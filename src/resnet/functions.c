#include "functions.h"
#include <string.h>
#include <math.h>


void conv2d_ref (
	float* output,
	float* input,
	float* weight,
	/*output features*/
	int F, int E,
	/*input	 features*/
	int W, int H, int C,
	/*weight features*/
	int S, int R, int M,
	/*Conv2d features*/
	int STRIDE, int PADDING
) {
    memset(output, 0, F * E * M * sizeof(float));
	int e = 0, f = 0,
		h = 0, w = 0,
		r = 0, s = 0,
		c = 0, m = 0;
	for (m = 0; m < M; m++) {
		for (e = 0; e < E; e++) {
			for (f = 0; f < F; f++) {
				for (c = 0; c < C; c++) {
					for (r = 0; r < R; r++) {
						for (s = 0; s < S; s++) {
							w = f * STRIDE - PADDING + s;
							h = e * STRIDE - PADDING + r;
							if (w < 0 || w >= W || h < 0 || h >= H) {
								continue;
							}
							output[((m * E + e) * F) + f] += input[((c * H + h) * W) + w] * weight[(((m * C + c) * R + r) * S) + s];
						}
					}
				}
			}
		}
	}
}


void maxpool2d (
	float* output,
	float* input,
	/*output features*/
	int F, int E,
	/*input	 features*/
	int W, int H, int C,
	/*kernel features*/
	int S, int R,
	/*MaxPool2d features*/
	int STRIDE, int PADDING
) {
	int e = 0, f = 0,
		h = 0, w = 0,
		r = 0, s = 0,
		c = 0;
	for (c = 0; c < C; c++) {
		for (e = 0; e < E; e++) {
			for (f = 0; f < F; f++) {
				float max_val = -3.402823466e+38F; //-FLT_MAX
				for (r = 0; r < R; r++) {
					for (s = 0; s < S; s++) {
						w = f * STRIDE - PADDING + s;
						h = e * STRIDE - PADDING + r;
						if (w < 0 || w >= W || h < 0 || h >= H) {
							continue;
						}
						max_val = max_val < input[(c * H + h) * W + w] ? input[(c * H + h) * W + w] : max_val;
					}
				}
				output[(c * E + e) * F + f] = max_val;
			}
		}
	}
	
}


/*
* weight[0][C] : running_mean
* weight[1][C] : running_var
* weight[2][C] : weight
* weight[3][C] : bias
*/
void batchnorm2d (
	float* inout,
	float* weight,
	/*inout	 features*/
	int W, int H, int C
) {
	float eps = 1e-5F;
	int h = 0, w = 0, c = 0;
	for (c = 0; c < C; c++) {
		for (h = 0; h < H; h++) {
			for (w = 0; w < W; w++) {
				inout[(c * H + h) * W + w] = (inout[(c * H + h) * W + w] - weight[0 * C + c]) / sqrtf(weight[1 * C + c] + eps) * weight[2 * C + c] + weight[3 * C + c];
			}
		}
	}
}


void relu (
	float* inout,
	/*inout	 features*/
	int W, int H, int C
) {
	int h = 0, w = 0, c = 0;
	for (c = 0; c < C; c++) {
		for (h = 0; h < H; h++) {
			for (w = 0; w < W; w++) {
				inout[(c * H + h) * W + w] = inout[(c * H + h) * W + w] > 0 ? inout[(c * H + h) * W + w] : 0;
			}
		}
	}
}


/*
* fc_layer = nn.Sequential(
*     nn.AvgPool2d(kernel_size=1),
*     nn.Linear(in_features=C, out_features=M)
* )
*/
void fc_layer (
	float* temp,
	float* inout,
	float* weight,
	/*input	 features*/
	int W, int H, int C,
	/*output features*/
	int M
) {
	int h = 0, w = 0, c = 0, m = 0;
	float* biases = weight + C * M;

	// AvgPool2d(kernel_size=1)
	for (c = 0; c < C; c++) {
		float sum = 0;
		for (h = 0; h < H; h++) {
			for (w = 0; w < W; w++) {
				sum += inout[(c * H + h) * W + w];
			}
		}
		temp[c] = sum / (W * H);
	}

	// Linear(in_features=C, out_features=M)
	for (m = 0; m < M; m++) {
		float weight_tmp = 0;
		for (c = 0; c < C; c++) {
			weight_tmp += temp[c] * weight[m * C + c];
		}
		inout[m] = weight_tmp + biases[m];
	}

}
