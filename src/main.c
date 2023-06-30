///
/******************************************************************************
Copyright (c) 2017 SoC Design Laboratory, Konkuk University, South Korea
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met: redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer;
redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution;
neither the name of the copyright holders nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Authors: Jooho Wang (joohowang@konkuk.ac.kr)

Revision History
2017.11.14: Started by Jooho Wang
2019.05.27: Extended by Sunwoo Kim (AlexNet)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "benchmarking.h"

#include "resnet/functions.h"
#include "resnet/resnet50.h"
#include "resnet/input_image.h"
#include "resnet/class_labels.h"

// This is your Conv2d function. See check list on "conv2d_opt.c".
extern void conv2d_opt();


// If you do not want to see your results, comment out the following 'define' statement below.
//#define DO_TEST

#define TEST_CASE   2
#define TEST_ROUNDS 1
#define DATASETS        10
#define CLASSES         1000

#define WEIGHTS_MEMPOS      0x0C900000
#define WORKLOAD            (112*112*64*sizeof(float))


void benchmark_init(unsigned int in_map) {
    memcpy((float*)in_map, in_img[0], 3 * 224 * 224 * sizeof(float));
}

void benchmarker_wrapper(unsigned int functions, unsigned int in_map, unsigned int weight_base) {
    resnet50(*((palette*)functions), (float*)in_map, (float*)weight_base);
}

#define REFERENCE 1
#define OPTIMIZED 0
void benchmark_valid(unsigned int in_map, unsigned int results, unsigned int is_ref) {
    float (*results_p)[CLASSES] = (float (*)[CLASSES])results;
    if (is_ref) {
    	memcpy(results_p[REFERENCE], (float*)in_map, CLASSES * sizeof(float));
	}
    else {
		memcpy(results_p[OPTIMIZED], (float*)in_map, CLASSES * sizeof(float));
	}
}



int main() {
    int i;
    float* in_map;

    palette ref_functions = {
        conv2d_ref,
        maxpool2d,
        batchnorm2d,
        relu,
        fc_layer
    };
    palette opt_functions = {
        conv2d_opt,
        maxpool2d,
        batchnorm2d,
        relu,
        fc_layer
    };

    in_map = (float*)malloc(WORKLOAD);
	if (in_map == NULL) {
        printf("\'in_map\' allocation failed.");
        goto INMAP_ALLOC_FATAL;
    }

#ifdef DO_TEST
	int data_set = 0;
    int label_idx;
    float value_max;
    /*****************************
          Check your results!
    *****************************/
    
    printf("Resnet50 w/ Conv2d_opt. \r\n");

    for (data_set = 0; data_set < DATASETS; data_set++) {

        // reading input image
        memcpy(in_map, in_img[data_set], 3 * 224 * 224 * sizeof(float));

        // run resnet50
        resnet50(opt_functions, in_map, (float*)WEIGHTS_MEMPOS);

        // result
        label_idx = 0;
        value_max = in_map[0];
        for (i = 1; i < CLASSES; i++) {
            if (in_map[i] > value_max) {
                label_idx = i;
                value_max = in_map[i];
            }
        }
        printf("Image %1d: %3d - %s \r\n", data_set, label_idx, label[label_idx]);
    }
#endif


    /*****************************
             Run benchmark
    *****************************/
   	BENCHMARK_CASE *pBenchmarkCase;
	BENCHMARK_STATISTICS    *pStat;
	double opt_time = 0;
	double ref_time = 0;
    float results[2][CLASSES] = {{0,},};
	BENCHMARK_CASE BenchmarkCases[TEST_CASE] = {
		{"Reference", TEST_ROUNDS, benchmark_init, benchmarker_wrapper,
			{   /*functions*/   (unsigned int)&ref_functions,
				/*input addr*/  (unsigned int)in_map,
				/*output addr*/ (unsigned int)results,
				/*weights addr*/(unsigned int)WEIGHTS_MEMPOS,
				/*is ref?*/     (unsigned int)REFERENCE,
			}, 0, benchmark_valid
		},
		{"Optimization", TEST_ROUNDS, benchmark_init, benchmarker_wrapper,
			{   /*functions*/   (unsigned int)&opt_functions,
                /*input addr*/  (unsigned int)in_map,
                /*output addr*/ (unsigned int)results,
                /*weights addr*/(unsigned int)WEIGHTS_MEMPOS,
                /*is ref?*/     (unsigned int)OPTIMIZED,
            }, 0, benchmark_valid
		}
	};

	xil_printf("\r\n");
	xil_printf("-----Benchmarking Start-----\r\n");
	for(i=0; i < TEST_CASE; i++) {
		pBenchmarkCase = &BenchmarkCases[i];
		pStat = &(pBenchmarkCase->stat);
		printf("Case %d: %s\r\n", i, pBenchmarkCase->pName);
		run_benchmark_single(pBenchmarkCase);
		statistics_print(pStat);
		if (i == 0) {
			ref_time = pStat->ullMax;
		}
		else {
			opt_time = pStat->ullMax;
		}
	}
	xil_printf("----Benchmarking Complete----\r\n");
	xil_printf("\r\n");
	printf("Optimized is x%.2f faster than Reference\r\n", (double)(ref_time / opt_time));

    /*****************************
           Measure accuracy
         (Quantization error)
    *****************************/

    float error  = 0;
    float signal = 0;
    float NSRdB  = 0;
    for (i = 0; i < CLASSES; i++) {
        error += pow((results[REFERENCE][i] - results[OPTIMIZED][i]), 2);
        signal+= pow((results[REFERENCE][i]), 2);
    }
    NSRdB = 10*log10(error/signal);
    printf("Measured Accuracy: NSR(dB) = %0.3f \r\n", NSRdB);

    free(in_map);
INMAP_ALLOC_FATAL:
	return 0;
}
