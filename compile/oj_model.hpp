#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include "util.hpp"

class Question{
  public:
    std::string id;
    std::string name;
    std::string star;//难易
    std::string route;//题目路径
    std::string describe;//题目描述
    std::string code_structure;//代码框架
    //std::string code_test;//最终代码
    std::string test_case;//测试用例
};

//将题目加载到内存，通过哈希组织
//
class OJmodel{
    private:
        std::map<std::string,Question> _model;
    public:
        bool Load(){
            //加载题目到内存
            //1. 打开oj_config.cfg文件
            std::ifstream file("./oj_data/oj_config.cfg");
            if(!file.is_open()){
                return false;
            }
            //2. 按行获取，并且解析
            std::string line = "";
            while(std::getline(file,line)){
                std::vector<std::string> token;
                StringUtil::Split(token,line,"\t");
                if(token.size() != 4){
                    LOG(ERROR)<<"config read error\n";
                    continue;
                }
                //3. 将解析结果组织成Question结构体
                Question q;
                q.id = token[0];
                q.name = token[1];
                q.star = token[2];
                q.route = token[3];
                FileUtil::Read(q.route+"/describe.txt",&q.describe);
                FileUtil::Read(q.route+"/code_structure.cc",&q.code_structure);
                FileUtil::Read(q.route+"/test_case.cc",&q.test_case);
                //4. 将Question填入哈希表
                _model[q.id] = q;
            }
            LOG(INFO)<<"Load "<<_model.size()<<" questions\n";
            return true;
        }
        //输出型参数，获取所有的题目
        bool GetAllQuestion(std::vector<Question> &questions)const{
            questions.clear();
            for(const auto &e :_model){
                questions.push_back(e.second);
            }
            return true;
        }
        bool GetQuestion(std::string id,Question &q)const{
            auto pos = _model.find(id);
            if(pos != _model.end()){
                q = pos->second;
            }
            else
                return false;
            return true;
        }
};
