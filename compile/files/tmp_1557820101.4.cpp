#include <iostream>
using namespace std;
#include <vector>
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        
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
