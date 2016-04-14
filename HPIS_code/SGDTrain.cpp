/*
 * Author: Winston (Boqun) YAN
   SGDTrain including materialize and HPIS
*/

#include <iostream>
#include <cassert>
#include <fstream>
#include <cmath>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include "utils/numeric.h"
#include "utils/myhash.h"

using namespace std;

//#define dirR "/tables/r"
#define dirT "/t"

#define MAXDIM 1024

/*
   parsing the training datasets into XS[], XR[]
   */

// Schema of S is (SID Target FK XS) dimension = dS + 2
// Schema of R is (RID XR) dimension = dR + 1

bool debug = false;
string dirS;
int ns, ds;
double **global_T;

void gen_ridx(double **S, double ***R, const vector<int>& alloc_list, 
              int ns, int* nr, int *fkidx, vector<int>& idx_list);
void parse(string file, double **F, const int r, int *c);
void log_train(double **T, double a, int dim, double* weight, int nt);
void hp_shuffle (double **S, const vector<int>& ridx, int total);
double** join(
    double **S, double **R[], unordered_set<int>& fki, int *fki_arr, int num, int *nr, int *dr, int dt);
void print_table(double **T, int r, int c);
double lossfunc(double **T, int dim, double *weight);
void output_table(const char *out_dir, double **T, int r, int c);
inline void free_matrix(double **T, int r);

int main(int argc, char* argv[]) {
  // mode:
  // 1 for Single table shuffle every time
  // 2 for shuffle once
  // 3 for HPIS
  // 4 for HPIS once
  int mode = -1;
  int dt, dw;
  dt = dw = 0;
  int *dr = NULL;
  int *nr = NULL;
  //int *fkidx = NULL;
  unordered_set<int> fkidx;
  int *fkidx_arr;
  int num_r, ep; // epochs, partitions
  // only used for HP (mode 3 or 4), 0 when not used
  num_r = 0; // num of partitions
  double alpha, decay, old_alpha;
  //char *input_root, *output_root;
  string input_root, output_root;
  string *rtables = NULL;
  if (argc < 5) {
    cout << "Input format:" << argv[0] 
	 << " <ns> <S name> -n <num of Rs> -i <input dir_root> <R names>\n"
            "-f <foreign key index> -r <num of examples in Rs>\n"
	    "-o <output dir_root> -e <epochs> -p <# of partitions>\n"
	    "-a <stepsize> -d <stepsize decay> -m <mode> -v <verbose on>\n";
    exit(1);
  }

  // Init from args
  ns = atoi(argv[1]);
  //nr = atoi(argv[2]);
  //ds = atoi(argv[2]) + 2; // plus SID and FK 
  dirS = argv[2];
  //dr = atoi(argv[4]) + 1; // plus RID (FK)
  //dt = ds + dr - 1;
  //dw = 
  // List of memory allocations for the R tables
	vector<int> RAllocList;
  for (int i = 3; i < argc; ++i) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 'n':
          num_r = atoi(argv[++i]);
          rtables = new string[num_r];
          dr = new int[num_r];
          fkidx_arr = new int[num_r];
          break;
	case 'i':
          assert(num_r != 0);
          assert(rtables != NULL);
	  input_root = argv[++i];
          for (int j = 0; j < num_r; ++j) {
            rtables[j] = argv[++i];
          }
	  break;
        case 'f':
          assert(num_r != 0);
          for (int j = 0; j < num_r; ++j) {
            int val_test = atoi(argv[++i]);
            assert(val_test != 0);
            fkidx.insert(val_test);
            fkidx_arr[j] = val_test;
          }
          assert(num_r == fkidx.size());
          break;
        case 'r':
          assert(num_r != 0);
          nr = new int[num_r];
          for (int j = 0; j < num_r; ++j) {
            nr[j] = atoi(argv[++i]);
            assert(nr[j] != 0);
          }
          break;
	case 'o':
	  output_root = argv[++i];
	  break;
	case 'e':
	  ep = atoi(argv[++i]);
	  break;
	case 'p':
	  //num_p = atoi(argv[++i]);
	  RAllocList.reserve(num_r);
	  for (int j = 0;j < num_r; ++j) {
		 RAllocList.push_back(atoi(argv[++i]));
	  }
	  break;
	case 'a':
	  alpha = atof(argv[++i]);
	  break;
	case 'd':
	  decay = atof(argv[++i]);
	  break;
	case 'v':
	  debug = true;
	  break;
	case 'm':
	  mode = atoi(argv[++i]);
	  break;
	default:
	  cout << "unexpected flag: " << argv[i][1] << endl;
	  exit(1);
      }
    }
  }
  if (debug) {
    printf("ns: %d\n", ns);
    cout << "nr: ";
    for (int i = 0; i < num_r; ++i)
      cout << nr[i] << " ";
    cout << "\n";
    /*cout << "dr: ";
    for (int i = 0; i < num_r; ++i)
      cout << dr[i] << " ";
    cout << endl;*/
    printf("epochs: %d, alpha: %f, decay: %f, mode: %d\n",
	ep, alpha, decay, mode);
  }

  string input_S = input_root + "/" + dirS;
  string output_T = output_root + "/" + dirT;
  //ifstream inS(input_S);

  if (debug) {
    cout << "Input dir for S: " << input_S << endl;
    cout << "Output dir: " << output_T << endl;
  }

  vector<int> ridx, old_ridx; // row index array only for HP.

  

  // Tables...
  if (debug) cout << "Handle S." << endl;
  double **S = new double*[ns];
  parse(input_S, S, ns, &ds);

  //for(int i = 0; i < ns; i++)
    //S[i] = new double[ds];
  assert(num_r != 0);
  double ***R = new double**[num_r];
  if (debug) cout << "Handle Rs." << endl;
  for(int i = 0; i < num_r; i++) {
    R[i] = new double*[nr[i]];
    string cur_dir = input_root + "/" + rtables[i];
    parse(cur_dir, R[i], nr[i], &dr[i]);
  }
  if (debug)  cout << "R tables parsed." << endl;
  
  if (mode == 3 || mode == 4) {
    // Convert # page to # tuples
    const int PAGE_SIZE = 8000;
    for (int i = 0; i < RAllocList.size(); ++i) {
      cout << "Converting " << i << ": " << RAllocList[i] << "," << dr[i] << "\n";
      RAllocList[i] = floor((PAGE_SIZE * RAllocList[i]) / 8.0 / 
      dr[i]);
    }
    gen_ridx(S, R, RAllocList, ns, nr, fkidx_arr, ridx);
    cout << "Partition size: " << ridx.size() << "\n";
    old_ridx = ridx;
    cout << "Print ridx: ";
    for (auto i : ridx) cout << i << ",";
    cout << "\n";
  }

  //print_table(R[0], nr[0], dr[0]);
  dt = ds;
  for (int i = 0; i < num_r; ++i) {
    dt += dr[i] - 2;
  }
  cout << "dt: " << dt << endl;
  double **T;


  if (debug) {
    cout << "output S table..." << endl;;
    cout << num_r << "," << dr[0] << dr[1] << dt << endl;
    T = join(S, R, fkidx, fkidx_arr, num_r, nr, dr, dt);
    //output_table(output_T.c_str(), T, ns, dt);
    print_table(S, ns, ds);
    cout << "finished output" << endl;
  }
  old_alpha = alpha;

  dw = dt - 2;
  double *W = new double[dw];
  double loss_value = 0;
  memset(W, 0, sizeof(double)*dw);
// Code below until switching mode only works in mode 3, as it is solely used 
// to produce a loss value table for HPIS accuracy optimization problem
  ofstream lossTblFile(string(output_root) + string("/lossvalue.tbl"));
  vector<int> idx_array(ridx.size());
  switch (mode) { 
    case 1: // shuffleAlways single table
      if (debug) cout << "ShuffleAlways Single Table:\n";
      for (int i = 0; i < ep; ++i) {
	//random_shuffle(S, &S[ns]);
        T = join(S, R, fkidx, fkidx_arr, num_r, nr, dr, dt);
	if (i == 0 && debug) {
	  cout << "1st shuffled T:\n";
	  print_table(T, ns, dt);
	}
	log_train(T, alpha, dw, W, ns);
	alpha *= decay;
	if (debug) {
	  loss_value = lossfunc(T, dw, W);
	  cout << "Loss value @ epoch " << i+1 << ": " << loss_value << "\n";
	}
	if (i == ep - 1) {
	  loss_value = lossfunc(T, dw, W);
	}
	free_matrix(T, ns);
      }
      break;
    case 2: // shuffleOnce single table
      if (debug) cout << "ShuffleOnce Single Table:\n";
      random_shuffle(S, &S[ns]);
      T = join(S, R, fkidx, fkidx_arr, num_r, nr, dr, dt);
      if (debug) print_table(T, ns, dt);

      for (int i = 0; i < ep; ++i) {
	log_train(T, alpha, dw, W, ns);
	alpha *= decay;
	if (debug) {
	  loss_value = lossfunc(T, dw, W);
	  cout << "Loss value @ epoch " << i+1 << ": " << loss_value << "\n";
	}
      }
      loss_value = lossfunc(T, dw, W);

      free_matrix(T, ns);
      break;
    case 3: // HPIS shuffleAlways
      if (debug) cout << "ShuffleAlways HPIS:" << endl;
      for (int i = 0; i < idx_array.size(); ++i) idx_array[i] = i;
      for (int i = 0; i < ep; ++i) {
	// shuffle within chunks always	
	ridx = old_ridx;
	hp_shuffle(S, ridx, ns);
	//T = join(S, R); // freed within loop always.
        T = join(S, R, fkidx, fkidx_arr, num_r, nr, dr, dt);
	// shuffle chunk order
	random_shuffle(idx_array.begin(), idx_array.end());

	if (debug) {
	  cout << "Row idx @ epoch " << i+1 << ": \n";
	  for (int j = 0; j < ridx.size(); ++j)
	    cout << ridx[j] << " ";
	  cout << "\n";
	}

	for (int j = 0; j < idx_array.size(); ++j) {
	  int rnum = (idx_array[j] != (ridx.size() - 1) ? ridx[idx_array[j] + 1] : ns) - ridx[idx_array[j]];

	  if (debug) {
	    cout << "training based on row " << ridx[j]
		 << " to row " << ridx[j]+rnum-1 << "\n";
                 
	  }
          global_T = T;
	  log_train(&T[ridx[idx_array[j]]], alpha, dw, W, rnum);
	  //alpha *= decay;
	}
	//log_train(T, alpha, dw, W, ns);
	alpha *= decay;
	if (debug) {
	  loss_value = lossfunc(T, dw, W);
	  cout << "Loss value @ epoch " << i+1 << ": " << loss_value << "\n";
	}
	if (i == ep-1)
	  loss_value = lossfunc(T, dw, W);
// For HIPS optimization problem, print the loss table
	lossTblFile << i + 1 << " " << lossfunc(T, dw, W) << "\n";
	free_matrix(T, ns);
      } 
      cout << "Printing predicted W: ";
      for (int i = 0; i < dw; ++i) cout << W[i] << ",";
      cout << "\n";
      cout << "Printing result" << endl;
      T = join(S, R, fkidx, fkidx_arr, num_r, nr, dr, dt);
      for (int i = 0;i < 200; ++i) {
	if (i == 0) cout << T[i][0] << "," << T[i][1] << "," << T[i][2] << "\n";
        cout << T[i][0] << " " << dot(W, &T[i][2], dw) << "\n";
      }
      free_matrix(T, ns);
      cout << "End printing result\n";
      break;
    case 4: // HP once
      if (debug) cout << "ShuffleOnce HPIS:\n";
      // shuffle within chunks just once
      hp_shuffle(S, ridx, ns);
      //T = join(S, R); // need to be freed.
      T = join(S, R, fkidx, fkidx_arr, num_r, nr, dr, dt);

      for (int i = 0; i < ep; ++i) {
	// shuffle chunk order always
	//ridx = old_ridx;
	//random_shuffle(&ridx[0], &ridx[num_p]);
	for (int j = 0; j < ridx.size(); ++j) {
	  int rnum = (j != (ridx.size() - 1) ? ridx[j + 1] : ns) - ridx[j];
	  if (debug) {
	    cout << "training based on row " << ridx[j]
		 << " to row " << ridx[j]+rnum-1 << "\n";
	  }
	  log_train(&T[ridx[j]], alpha, dw, W, rnum);
	  alpha *= decay;
	}
	if (debug) {
	  loss_value = lossfunc(T, dw, W);
	  cout << "Loss value @ epoch " << i+1 << ": " << loss_value << "\n";
	}
      }
      loss_value = lossfunc(T, dw, W);

      free_matrix(T, ns);
      break;
    default:
      cerr << "wrong mode: " << mode << endl;
      exit(1);
  }
  lossTblFile.close();
  // Output format:
  // <mode> <alpha>, <decay>, <epochs>, (<partitions>,) <loss value>
  cout << mode << ", " << old_alpha << ", " << decay << ", " << ep;
  cout << ", " << loss_value << endl;
  return 0;
}

// -------------------------- end of main ---------------------------

struct r_hash_func {
    static int num_r;
    size_t operator()(const int *a) const {
      int hash = 0;
          for (int i = 0; i < num_r; ++i) {
                  hash += a[i];
          }
          return hash;
    }
  };
  
  struct r_equal_func {
    static int num_r;
    bool operator()(const int *a, const int *b) const {
      for (int i = 0; i < num_r; ++i) {
                  if (a[i] != b[i]) return false;
          }
          return true;
    }
  };
  int r_hash_func::num_r;
  int r_equal_func::num_r;


void gen_ridx(double **S, double ***R, const vector<int>& alloc_list, int ns, int* nr, 
              int *fkidx, vector<int>& idx_list) {
  int num_r = alloc_list.size();
  cout << "alloc: " << alloc_list[0] << "," << alloc_list[1] << endl;
  idx_list.reserve(num_r);
  int *partition_group = new int[ns * num_r];
  for (int i = 0; i < ns; ++i) {
	  for (int j = 0; j < num_r; ++j) {
		  partition_group[i * num_r + j] = (S[i][fkidx[j]] - 1) / alloc_list[j];
	  }
  }
  /*for (int i = 0;i < 200; ++i) {
    cout << "printing partition groups\n";
    for (int j = 0; j < num_r; ++j) {
      cout << partition_group[i * num_r + j] << ",";
    }
    cout << "\n";
  }*/
  r_hash_func::num_r = num_r;
  r_equal_func::num_r = num_r;
  
  unordered_multimap<int*, double*, r_hash_func, 
  r_equal_func> group_map;
  
  for (int i = 0; i < ns; ++i) {
	  group_map.insert(make_pair(partition_group + i * num_r, S[i]));
  }
  int j = 0;
  for (auto it = group_map.begin(); it != group_map.end(); ) {
	idx_list.push_back(j);
        auto end = group_map.equal_range(it->first).second;
    for (; it != end; ++it ) {
		S[j] = it->second;
		++j;
	}
  }
  assert(j == ns);
  
  delete[] partition_group;
  /*for (int i = 0;i < alloc_list.size(); ++i) {
	  int assigned_mem = alloc_list[i], total_mem = nr[i];
	  for (int j = 0; j < total_mem;)
  }*/
}

inline void free_matrix(double **T, int r) {
  for (int i = 0; i < r; ++i)
    delete [] T[i];
}

// TODO: read in -d for all; not for sparse version: -d includes ID
void parse(string file, double **F, const int r, int *c) {
  ifstream inF(file);
  int row = 0;
  int col = 0;
  if(inF.is_open()){
    char input[MAXDIM];
    char* temp;
    // 1st line: -d dimension
    inF.getline(input, MAXDIM);
    if (debug)  cout << "dim: " << input << endl;
    temp = strtok(input, " ");
    assert(temp[0] == '-');
    temp = strtok(NULL, " ");
    *c = atoi(temp);
    if (debug)  cout << "table dim: " << *c << endl;
    while (!inF.eof()) {
      F[row] = new double[*c];
      inF.getline(input, MAXDIM);
      temp = strtok(input, " ");
      while(temp != NULL){
        assert(col < *c && row < r);
	F[row][col++] = atof(temp);
	temp = strtok(NULL, " ");
      }
      col = 0;
      row++;
    }

  }else{
    cout << "-------FILE OPEN FAILURE------" << endl;
    exit(1);
  }
}

void log_train(double **T, double a, int dim, double* weight, int nt){
  for(int i = 0; i < nt; i++){
    double wx = dot(weight, &T[i][2], dim);
    double sig = sigma(-1 * wx * T[i][1]);
    double c = a*T[i][1]*sig;
    //double temp = T[i][1];
    add_and_scale(weight, dim, &T[i][2], c);
    //cout << "Trained on " << i << "th entry, now loss value: " << 
    //lossfunc(T, dim, weight) << "\n";
  }
}

void hp_shuffle (double **S, const vector<int>& ridx, int total) {
  for (int i = 0; i < ridx.size(); ++i) {
    if (i == ridx.size() - 1) {
      assert(ridx[i] < total);
      random_shuffle(&S[ridx[i]], &S[total]);
    } else {
      random_shuffle(&S[ridx[i]], &S[ridx[i+1]]);
    }
  }
}

// ds + dr - 1
double** join(
    double **S, double **R [], unordered_set<int> &fki, int *fki_arr,
    int num, int *nr, int *dr, int dt) {
  // ith R element is at matchR[i] pos
  // matchR[0] and empty are -1 TODO: really need this?
  // TODO: define new dimension var for all R tables.
  
  // current dimension of the table (so T[cur-1] is the last attr)
  int cur = 0;
  /*int **matchR = new int* [num];
  for(int i = 0; i < num; i++) {
    matchR[i] = new int[nr[i]]; 
    for(int j = 0; j < nr[i]; ++j) {
      matchR[i][j] = -1;
    }
  }*/

  double **T = new double* [ns];

  for(int i = 0; i < ns; i++){
    cur = 0;
    T[i] = new double[dt];
    for(int j = 0; j < ds; ++j, ++cur){
      if (fki.find(j) == fki.end()) {
        T[i][cur] = S[i][j];
      } else {
        --cur;
      }
    }
    // loop thru the r tables
    for(int j = 0; j < num; ++j) {
      int fk = (int)S[i][fki_arr[j]] - 1; // fk idx
      for (int p = 1; p < dr[j]; ++p, ++cur) {
         T[i][cur] = R[j][fk][p];
      }
    }
    //int idx = matchR[fk];
    /*if(idx == -1) cout<<"------RMATCHING EXCEPTION------"<<endl;
    for(int j = 0; j < (dr)-1; j++)
      T[i][ds+j] = R[idx][j+1];*/
  }
  //cout << "printing in join." << endl;
  //print_table(T, ns, dt);
  // delete[] matchR;
  return T;
}

double lossfunc(double **T, int dim, double *weight){
  double lossVal = 0;
  for (int i = 0; i < ns; i++) {
    lossVal += log(1 + exp(-1*T[i][1]*dot(weight, &T[i][2], dim)));
  }
  return lossVal;
}

void print_table(double **T, int r, int c) {
  cout << "Printing..." << endl;
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      cout << T[i][j] << " ";
    }
    cout << endl;
  }
}

void output_table(const char *out_dir, double **T, int r, int c) {
  ofstream out(out_dir);
  if (!out.is_open()) {
    cout << "output dir open failure.\n";
    exit(1);
  }
  cout << c << endl;
  for (int i = 0; i < c; ++i) {
    if (i == 1) out << "t" << " ";
    else out << "x" << i << " ";
  }
  out << endl;
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      if (j == 1 && T[i][j] == -1)
	out << 0 << " ";
      else
	out << T[i][j] << " ";
    }
    out << endl;
  }
}
