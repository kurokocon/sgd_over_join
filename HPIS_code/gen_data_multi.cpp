/*
 *   Copyright 2015 Arun Kumar
 * 
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using std::vector;

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
inline void printUsage() {
  std::cout << "gen_data_multi <nS> <dS> <Variance> <num_table> <Sfile> <Tfile> <nR list> <dR list> <Rfile list> [1 for R alone | 2 for S alone | 0 otherwise ]" << std::endl;
}

int main(int argc, char* argv[]) {
  // At least one R table required
  if(argc < 11) {
    printUsage();
    return -1;
  }
  int nS = atoi(argv[1]);
  
  int dS = atoi(argv[2]); //includes target
  int dim = dS - 1;
  double var = atof(argv[3]);
  
  vector<vector<double> > w_vec;
  //double XR[dR];
  const int num_table = atoi(argv[4]);
  int onlyflag = atoi(argv[argc - 1]);
  switch (onlyflag) {
    case 0:
      if (argc != (8 + num_table * 3)) {
        printUsage();
        return -1;
      }
      break;
    case 1:
      if (argc != (7 + num_table * 3)) {
        printUsage();
        return -1;
      }
      break;
    case 2:
      if (argc != (8 + num_table * 2)) {
        printUsage();
        return -1;
      }
      break;
    default:
      printUsage();
      return -1;
  }
  
  int dR_vec[num_table];
  int nR_vec[num_table];
  char *RNames[num_table];
  vector<int> weight_start_idx(num_table);
  for (int i = 0;i < num_table; ++i) {
    nR_vec[i] = atoi(argv[7 + i]);
    dR_vec[i] = atoi(argv[7 + i + num_table]);
    weight_start_idx[i] = dim;
    dim += dR_vec[i];
    RNames[i] = argv[7 + i + 2 * num_table];
  }
  std::cerr << nS << "," << dS << "," << var << "," << num_table << "," << argv[5] << ",";
  for (int i = 0;i < num_table; ++i) {
    std::cerr << nR_vec[i] << ",";
  }
  for (int i = 0;i < num_table; ++i) {
    std::cerr << dR_vec[i] << ",";
  }
  for (int i = 0;i < num_table; ++i) {
    std::cerr << RNames[i] << ",";
  }
  std::cerr << onlyflag << "\n";
  vector<vector<double> > XR_vec;
  // Reserve space for the R tables
  XR_vec.reserve(num_table);
  for (int i = 0;i < num_table; ++i) {
    XR_vec.push_back(vector<double>());
    XR_vec.back().reserve(dR_vec[i]);
  }
  
  double normsq = 0.0;
  srand48(0xDEADBEEF);
  vector<double> w(dim);
  
  // generate the model
   for(int i = dim - 1; i >= 0; i--) {w[i] = drand48() - 0.5; normsq += w[i]*w[i];}
  double norm = sqrt(normsq);
  for(int i = dim - 1; i >= 0; i--) {w[i] /= norm;}
  
  // Sample for R
  vector<vector<vector<double>>> Rtable (num_table);
  vector<vector<double>> Rweights(num_table);
  for (int j = 0;j < num_table; ++j) {
    int nR = nR_vec[j];
    int dR = dR_vec[j];
    if (onlyflag == 0) {
      Rtable[j].resize(nR);
      Rweights[j].resize(nR);
    }
    vector<double>& XR = XR_vec[j];
    std::ofstream Rout(RNames[j], std::ios::out);
    Rout << "-d " << dR + 1 << "\n";
    for(int k=1; k <= nR; k++)  {
      for(int i = dR - 1; i >= 0; i-=2) {
        if(i >= 1) {
          getgaussrv(XR[i], XR[i-1], var);
         
        } else {
          double y;
          getgaussrv(XR[i], y, var);
          
        }      
      }
      for (int i = 0;i < dR; ++i) {
        Rweights[j][k - 1] += XR[i] * w[weight_start_idx[j] + i];
      }
      if(onlyflag != 2) {
        //output to R (RID XR)
        Rout << k << " ";
        if (onlyflag == 0) {
          Rtable[j][k - 1].reserve(dR);
        }
        for(int i = 0; i < dR; i++) { 
          Rout << XR[i]; 
          if (onlyflag == 0) {
            Rtable[j][k - 1].push_back(XR[i]);
          }
          if(i < dR - 1) { 
            Rout << " "; 
          } 
        }
        Rout << "\n";
      }
    }
    Rout.close();
  }
  
  // Open S file and T file
  std::ofstream Sout, Tout;
  if(onlyflag != 1) {
    Sout.open(argv[5], std::ios::out);
    Sout << "-d " << dS + num_table + 1 << "\n";
  }
  if (onlyflag == 0) {
    Tout.open(argv[6], std::ios::out);
  }
  
    // Sample for S
    double XS[dS - 1];
  for(size_t k=1; k <= nS; k++)  {
    
    // generate a random vector for XS
    for(int i = dS - 2; i >= 0; i-=2) {
      if(i >= 1) {
        getgaussrv(XS[i], XS[i-1], var);
       
      } else {
        double y;
        getgaussrv(XS[i], y, var);
      }      
    }
    // Randomly select Ri entries for S
    int idx_entry[num_table];
    for (int i = 0;i < num_table; ++i) {
      idx_entry[i] = (int)(drand48() * nR_vec[i]);
    }
    // Calculate the label for S
    double s = 0;
    for (int i = 0;i < dS - 1; ++i) {
      s += XS[i] * w[i];
    }
    for (int i = 0; i < num_table; ++i) {
      s += Rweights[i].at(idx_entry[i]);
    }
    // Now apply match rate (Mismatch rate 10%)
    s *= drand48() - 0.1;
    s = s > 0 ? 1.0 : -1.0;
    
    if(onlyflag != 1) {
      //output to S (SID Target ForeignKey XS)
      Sout << k << " " << s << " ";
      if (onlyflag == 0) {
        Tout << k << " " << s << " ";
      }
      for (int j = 0; j < num_table; ++j) {
        Sout << idx_entry[j] + 1 << " ";
        if (onlyflag == 0) {
          size_t idx = idx_entry[j];
          for (size_t i = 0; i < Rtable[j][idx].size(); ++i) {
            Tout << Rtable[j][idx][i] << " ";
          }
        }
      }
      for(int i = 0; i < dS - 1; i++) { 
        Sout << XS[i]; 
        if (onlyflag == 0) {
          Tout << XS[i];
        }
        if(i < dS - 2) { 
          Sout << " "; 
          if (onlyflag == 0) {
            Tout << " ";
          }
        } 
      }
      Sout << "\n";
      if (onlyflag == 0) {
        Tout << "\n";
      }
    }
    // status
    if(k % 1000000 == 0) {std::cerr << "."; }    
  }
  Sout.close();

  std::cerr << "Finished" << std::endl;
}
