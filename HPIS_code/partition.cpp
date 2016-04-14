#include "partition.h"

#include <limits>

bool cmp_ad(tbl a, tbl b) { return a.ad < b.ad; } // cost
bool cmp_gap(tbl a, tbl b) { return a.gap < b.gap; } // surplus

tbl create_tbl (int id, int mem, int m) { 
  tbl ret;
  ret.id = id;
  ret.sz = m;
  ret.mem = mem;
  ret.p = ceil(m*1.0/mem);
  // num of extra mem needed to reduce p
  ret.ad = ceil(m%mem*1.0 / (ret.p-1));
  // num of mem the table can lose with the same p  
  ret.gap = ceil((mem - m%mem)*1.0/ret.p) - 1; // not exceed
  return ret;
}

void update_tbl(tbl &t, int mem) {
  t.mem = mem;
  t.p = ceil(t.sz*1.0/mem);
  t.ad = ceil(t.sz%mem*1.0 / (t.p-1));
  t.gap = ceil((mem - t.sz%mem)*1.0/t.p) - 1;
}

bool reduce(vector<tbl> &v, int ad) {
  for (int i = 0; i < v.size() && ad > 0; ++i) {
    ad -= min(ad, v[i].gap);
  }
  return ad == 0;
}

int greedy(int m, vector<int>& mems, vector<int>& mem_alloc) {
  // divide the whole mem evenly
  int mean = ceil(m*1.0/mems.size());
  sort(mems.begin(), mems.end());
  // tables that are smaller than the mean are given smaller mem
  // i.e. they are not divided into multiple chunks
  while (!mems.empty() && *mems.begin() < mean) {
    int i;
    for (i = 0; i < mems.size() && mems[i] < mean; ++i) {
      m -= mems[i];
    }
    mems.erase(mems.begin(), mems.begin() + i);
    // mean is updated after subtracting the small tables
    mean = ceil(m*1.0/(m-i));
  }
  // now deal with tables that are larger than mean
  vector<tbl> v;
  vector<tbl> com_v;
  for (int i = 0; i < mems.size(); ++i) {
    assert(mems[i] > mean);
    tbl newtbl = create_tbl(i, mean, mems[i]);
    v.push_back(newtbl);
  }
  // while v.size() > 1, reduce until return
  while (v.size() > 1) {
    // pick the one with the smallest ad (least cost to reduce p)
    sort(v.begin(), v.end(), cmp_ad);
    com_v.push_back(v[0]);
    v.erase(v.begin());
    // borrow mem from tables with smallest gap
    sort(v.begin(), v.end(), cmp_gap);
    int ad = com_v.back().ad;
    // cannot reduce anymore
    // i.e. To reduce the # of partition of the current table will result in
    // increase in other tables'.
    if (reduce(v, ad)) {
      for (int i = 0; i < v.size() && ad > 0; ++i) {
        // change the amt of mem of tables and their corresponding ad and gap
        update_tbl(v[i], v[i].mem - min(ad, v[i].gap));
      }
    } else {
      break;
    }
  }
  // v.size() == 1 
  int result = 1;
  //for (auto i : v)      {cout << i.mem << ","; }
  //for (auto i : com_v)  {cout << i.mem << ","; }
//cout << "\n";
//for (auto i : v)      {cout << i.sz << ","; }
  //for (auto i : com_v)  {cout << i.sz << ","; }
//cout << "\n";
  int j = 0;
  mem_alloc.resize(mems.size());
  for (auto i : v)      {result *= i.p; mem_alloc[j++] = i.mem; }
  for (auto i : com_v)  {result *= i.p; mem_alloc[j++] = i.mem; }
  return result;
  
}

static void loopOptions(size_t available_mem, vector<int>& mems, vector<int>& min_mem_assignment, size_t loopingIdx,
 vector<int>& cur_mem_assignment, size_t parent_num_partition, size_t& min_num_partition) {
	if (loopingIdx < mems.size() - 1) {
		for (size_t i = 1;i < available_mem - (mems.size() - loopingIdx); ++i) {
			cur_mem_assignment[loopingIdx] = i;
			loopOptions(available_mem - i, mems, min_mem_assignment, loopingIdx + 1, cur_mem_assignment, 
			parent_num_partition * ceil((double)mems[loopingIdx] / cur_mem_assignment[loopingIdx]), min_num_partition);
		}
	}
	else {
		size_t num_partition = parent_num_partition * ceil((double)mems[loopingIdx] / available_mem);
		if (num_partition < min_num_partition) {
			min_num_partition = num_partition;
			min_mem_assignment = cur_mem_assignment;
			min_mem_assignment[loopingIdx] = available_mem;
		}
	}
}

int optimal(int m, vector<int>& mems, vector<int>& mem_alloc) {
	size_t ret = std::numeric_limits<size_t>::max();
	vector<int> tmp_assignment(mems.size());
	loopOptions(m, mems, mem_alloc, 0, tmp_assignment, 1, ret);
	//for (size_t i = 0;i < mem_alloc.size(); ++i) {
	//	cout << mem_alloc[i] << ",";
	//}
	return ret;
}
int proportional(int m, vector<int>& mems, vector<int>& mem_alloc) {
  int sum = 0;
  int unassigned_mem = m;
  vector<int>& assigned_mem = mem_alloc;
  assigned_mem = mems;
  std::for_each(assigned_mem.begin(), assigned_mem.end(), [&sum](int i) { sum += i; });
  std::for_each(assigned_mem.begin(), assigned_mem.end(), [&m, &sum, &unassigned_mem](int& i) { i = (double)i / sum * m; unassigned_mem -= i; });
  for (int i = 0; unassigned_mem > 0; ++i, --unassigned_mem) {
    assigned_mem[i % assigned_mem.size()] += 1;
  }
  //for (size_t i = 0;i < assigned_mem.size(); ++i) std::cout << assigned_mem[i] << ",";
  size_t num_partition = 1;
  for (size_t i = 0;i < mems.size(); ++i) {
    num_partition *= ceil((double)mems[i] / assigned_mem[i]);
  }
  return num_partition;
}
