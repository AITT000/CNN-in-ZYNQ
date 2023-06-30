//This is your 2-Dimensional Convolution
//PLEASE CHECK THE FOLLOWING:
//1. The name of the function, "conv2d_opt".
//2. The order of the function parameters and their data types.

#include <string.h>

// If you try to use assembly, release comment the following 'define' statement below.
// You can change both conv2d_asm.s and conv2d_opt.c
// But never change parameters of conv2d_opt function

//#define USING_ASM

#ifdef USING_ASM
extern void conv2d_asm(
      float* out_ptr,
      float* in_ptr,
      float* weight_ptr
);
#endif

void conv2d_opt (
   float* output,
   float* input,
   float* weight,
   /*output features*/
   int F, int E,
   /*input    features*/
   int W, int H, int C,
   /*weight features*/
   int S, int R, int M,
   /*Conv2d features*/
   int STRIDE, int PADDING
) {
   ///////////////////////
   // Edit code below!! //



             memset(output, 0, F * E * M * sizeof(float));
            int e = 0, f = 0,
               h = 0, w = 0,
               r = 0, s = 0,
               mo = 0, mi = 0,
               co = 0, ci = 0,
			   c = 0,
               c_Tile_Size = 16,
               m_Tile_Size = 16;


            if (C==3){
                  for(mo = 0; mo < M; mo+=m_Tile_Size){
					   for (r = 0; r < R; r++) {
						 for(e = 0; e < E; e++){
							for (s = 0; s < S; s++){
								for(f = 0; f < F; f++){
							   w = f*STRIDE - PADDING + s;
							   h = e*STRIDE - PADDING + r;

							   if (w < 0 || w >= W || h < 0 || h >= H) {
								  continue;
								  }
							   for(mi = 0; mi < m_Tile_Size; mi++){
							   output[((((mo + mi) * E) + e) * F) + f] += input[((h) * W) + w] * weight[((((((mo + mi) * C)) * R) + r) * S) + s];
							   }
							   for(mi = 0; mi < m_Tile_Size; mi++){
							   output[((((mo + mi) * E) + e) * F) + f] += input[((((1) * H) + h) * W) + w] * weight[((((((mo + mi) * C) + 1) * R) + r) * S) + s];
							   }
							   for(mi = 0; mi < m_Tile_Size; mi++){
							   output[((((mo + mi) * E) + e) * F) + f] += input[((((2) * H) + h) * W) + w] * weight[((((((mo + mi) * C) + 2) * R) + r) * S) + s];
							   }

								   /*
            #ifdef USING_ASM

               conv2d_asm(&output[((((mo + mi) * E) + e) * F) + f], &input[((((c) * H) + h) * W) + w], &weight[((((((mo + mi) * C) + c) * R) + r) * S) + s]);
               conv2d_asm(&output[((((mo + mi) * E) + e) * F) + f], &input[((((c + 1) * H) + h) * W) + w], &weight[((((((mo + mi) * C) + c + 1) * R) + r) * S) + s]);
               conv2d_asm(&output[((((mo + mi) * E) + e) * F) + f], &input[((((c + 2) * H) + h) * W) + w], &weight[((((((mo + mi) * C) + c + 2) * R) + r) * S) + s]);

            #else
            */
              // output[((((mo + mi) * E) + e) * F) + f] += input[((h) * W) + w] * weight[((((((mo + mi) * C)) * R) + r) * S) + s];
               //output[((((mo + mi) * E) + e) * F) + f] += input[((((1) * H) + h) * W) + w] * weight[((((((mo + mi) * C) + 1) * R) + r) * S) + s];
               //output[((((mo + mi) * E) + e) * F) + f] += input[((((2) * H) + h) * W) + w] * weight[((((((mo + mi) * C) + 2) * R) + r) * S) + s];

             //  #endif

                                    			}
                                    			}
                                 }
                              }
                           }
                        }




            else
            {

               for (mo = 0; mo < M; mo+=m_Tile_Size){
                  for(co = 0; co < C; co+=c_Tile_Size){

                           for (r = 0; r < R; r++) {

                                 for(e = 0; e < E; e++){
                                    for (s = 0; s < S; s++){
                                 for(f = 0; f < F; f++){

                                       w = f*STRIDE - PADDING + s;
                                       h = e*STRIDE - PADDING + r;

                                       if (w < 0 || w >= W || h < 0 || h >= H) {
                                          continue;
                                          }
                                       for(mi = 0; mi < m_Tile_Size; mi++){
                                       for(ci = 0; ci < c_Tile_Size; ci++){

                                          output[((((mo + mi) * E) + e) * F) + f] += input[((((co + ci) * H) + h) * W) + w] * weight[((((((mo + mi) * C) + co + ci) * R) + r) * S) + s];

                                       }
                                    }
                                 }
                              }
                           }
                        }
            }
            }

            }
               // Edit code above!! //
               ///////////////////////
         }
