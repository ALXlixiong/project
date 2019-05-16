#include <iostream>
#include <string>
class Question{
  public:
    std::string id;
    std::string name;
    std::string star;//难易
    std::string describe;//题目描述
    std::string code_structure;//代码框架
    //std::string code_test;//最终代码
    std::string test_case;//测试用例
    std::string time_limit;//运行时间限制
    std::string mem_limit;//运行内存限制
};
