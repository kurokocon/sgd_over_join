/*
   Copyright 2015 Arun Kumar

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <random>

//Computes dot (inner) product of vectors x and y 
double dot(int len, double *x, double *y) {
  double ret = 0.0;
  for(int i = len -1; i >= 0; i--) {
    ret += x[i]*y[i];
  }
  return ret;
}

//Gaussian random number generator
void getgaussrv(double &v1, double &v2, double var) {
  double a, b, w;
  do {
    a = 2.0 * drand48() - 1.0;
    b = 2.0 * drand48() - 1.0;
    w = a * a + b * b;
  } while (w >= 1.0);
  w = sqrt((-2.0 * log(w)) / w);
  v1 = a * w * var;
  v2 = b * w * var;
}

int getLabel(int nR, int ridx, int num_partition, double prob, int mode) {
  static std::default_random_engine generator;
  static std::uniform_real_distribution<double> distribution(0, 1);
  int chunk_sz = nR / num_partition;
  double p;
  int ret;
  switch (mode) {
    case 1: 
      p = (ridx / chunk_sz) % 2 == 0 ? prob : 1 - prob;
      ret = distribution(generator) > p ? -1 : 1;
      break;
    case 2:
      p = ridx > (nR / 2) ? 1 - prob : prob;
      ret = distribution(generator) > p ? -1 : 1;
      break;
    default:
      std::cout << "Error: invalid mode\n";
      exit(1);
  }
  return ret;
}

//Writes out the files for S and R in space-separated-values format
//S is the entity table (the outer table for the join)
//Schema of S is (SID Target ForeignKey XS)
//R is the attribute table (the inner table for the join)
//Schema of R is (RID XR)
//nS is number of tuples in S, nR is number of tuples in R
//dS is number of features in S (including target), dR is number of features in R
//Variance is a parameter for the Gaussian random number generator (1.0 is recommended)
//Sfile is the prefix of the name for S files
//Rfile is the name of the R file
//The last argument enables generation of only R file or only S files
int main(int argc, char* argv[]) {
  if(argc != 11) {
    std::cout << "gen_data <Ratio of nS:nR> <nR> <dS> <dR> <Variance> <Sfile> <Rfile> <Expected number of partitions> <Probability of positive> <skew_mode>" << std::endl;
    return -1;
  }
  int r = atoi(argv[1]);
  int nR = atoi(argv[2]);
  int dS = atoi(argv[3]); //includes target
  int dR = atoi(argv[4]);
  double var = atof(argv[5]);
  int partition_num = atoi(argv[8]);
  double prob = atof(argv[9]);
  int mode = atoi(argv[10]);
  std::cerr << "r " << r << " nR " << nR << " dS " << dS << " dR " << dR << " var " << var << std::endl;

  int dim = dS + dR - 1;
  double w[dim];
  double XS[dS - 1];
  double XR[dR];
  double normsq = 0.0;
  srand48(0xDEADBEEF);

 

  // generate the model
  for(int i = dim - 1; i >= 0; i--) {w[i] = drand48() - 0.5; normsq += w[i]*w[i];}
  double norm = sqrt(normsq);
  for(int i = dim - 1; i >= 0; i--) {w[i] /= norm;}
  //for(int i = dim - 1; i >= 0; i--) { std::cerr << " " << w[i]; if(i > 0) { std::cerr << " "; } }
  // std::cerr << std::endl;

  std::ofstream Sout;

    Sout.open(argv[6], std::ios::out);
  std::ofstream Rout;

    Rout.open(argv[7], std::ios::out);
Sout << "-d " << dS + 2 << "\n";
Rout << "-d " << dR + 1 << "\n";
  // Sample for S and R
  for(int k=1; k <= nR; k++)  {
    // generate a random vector for XR
    for(int i = dR - 1; i >= 0; i-=2) {
      if(i >= 1) {
        getgaussrv(XR[i], XR[i-1], var);
        //XR[i] += s;
        //XR[i-1] +=s;
      } else {
        double y;
        getgaussrv(XR[i], y, var);
        //XR[i] += s;
      }      
    }
    // transform it by w
    for(int i = 0; i < dR; i++) { XR[i] *= w[dS - 1 + i]; }

    for(int ri=1; ri <= r; ri++) {
      // assign this point to a cluster
      double s =  getLabel(nR, k, partition_num, prob, mode);    
      // generate a random vector for XS
      for(int i = dS - 2; i >= 0; i-=2) {
        if(i >= 1) {
          getgaussrv(XS[i], XS[i-1], var);
          XS[i] += s;
          XS[i-1] +=s;
        } else {
          double y;
          getgaussrv(XS[i], y, var);
          XS[i] += s;
        }      
      }
      // transform it by w
      for(int i = 0; i < dS - 1; i++) { XS[i] *= w[i]; }

     
        //output to S (SID Target ForeignKey XS)
        Sout << r * (k - 1) + ri << " " << s << " " << k << " ";
        for(int i = 0; i < dS - 1; i++) { 
          Sout << XS[i]; 
          if(i < dS - 2) { 
            Sout << " "; 
          } 
        }
        Sout << std::endl;
      }

 
      //output to R (RID XR)
      Rout << k << " ";
      for(int i = 0; i < dR; i++) { 
        Rout << XR[i]; 
        if(i < dR - 1) { 
          Rout << " "; 
        } 
      }
      Rout << std::endl;

    // status
    if(k % 1000000 == 0) {std::cerr << "."; }    
  }
  Rout.close();
  Sout.close();
}
