#include <iostream>
using namespace std;
#include <vector>
#include <unordered_map>
class Solution {
public:
   vector<int> twoSum(vector<int>& nums, int target) {
    vector<int> res;
    if ( nums.empty() ) return res;
    std::unordered_map<int , int > diff_val;
    for ( int idx = 0; idx < nums.size(); idx++ ) {
        if ( diff_val.count( target - nums[idx] ) > 0 ) {
            res.push_back( diff_val[target - nums[idx]] );
            res.push_back( idx );
            return res;
        }
        else {
            diff_val[nums[idx]] = idx;
        }
    }
    
    return res;
}
};
//这个文件最终要和code_structure.cc文件合并
void Test1()
{
    Solution s;
    //vector<int> twoSum(vector<int>& nums, int target)
    vector<int> nums{2,7,11,15};
    int target = 9;
    vector<int> ret = s.twoSum(nums,target);
    vector<int> tmp{0,1};
    if(tmp.size() != ret.size()){
        //可以反应测试用例通过的百分比
        cout<<"test case 1 fail"<<endl;
        exit(1);
    }
    else{
        for(int i = 0;i<tmp.size();++i){
            if(tmp[i] != ret[i]){
                cout<<"test case 1 fail"<<endl;
                exit(1);
            }
        }
    }
    cout<<"This time, the test case passed !"<<endl;
}
int main()
{
    Test1();
    return 0;
}
