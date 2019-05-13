CREATE TABLE `question` (
  id int primary key  NOT NULL COMMENT '题目号',
  name varchar(255) NOT NULL COMMENT '题目名字',
  level enum('简单','中等','较难') NOT NULL COMMENT '题目难易程度',
  question_describe text NOT NULL COMMENT '题目描述',
  test_example text comment '测试例子',
  code_structure text COMMENT '代码框架'
)  CHARSET=utf8;

insert into question values(1,'两数之和','简单','给定一个整数数组 nums 和一个目标值 target，请你在该数组中找出和为目标值的那 两个 整数，并返回他们的数组下标。你可以假设每种输入只会对应一个答案。但是，你不能重复利用这个数组中同样的元素。','示例:
给定 nums = [2, 7, 11, 15], target = 9
因为 nums[0] + nums[1] = 2 + 7 = 9
所以返回 [0, 1]','class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        
    }
};');
