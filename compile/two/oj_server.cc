#include "util.hpp"
#include "CompileRun.hpp"
#include "httplib.h"
#include "oj_view.hpp"
int main()
{
    //服务器启动加载一次数据即可
    OJmodel model;
    model.Load();
    using namespace httplib;
    Server server;
    server.Get("/all_question",[&model](const Request& req,Response &resp){
        (void)req;
            std::vector<Question> aq;
            model.GetAllQuestion(aq);
            //借助aq得到最终的html
            std::string html = "";
            OJview::GetAllQuestionHtml(aq,html);
            //把html设置到响应
            resp.set_content(html,"text/html");
            });
    //R"()":原始字符串，不需要转义
    //\d+:正则表达式
    server.Get(R"(/question/(\d+))",[&model](const Request& req,Response& resp){
            Question q;
            model.GetQuestion(req.matches[1].str(),q);
            std::string html = "";
            OJview::GetAllQuestionHtml(q,html);
            resp.set_content(html,"text/html");
            });
    server.Post(R"(/compile/(\d+))",[&model](const Request& req,Response& resp){
            std::unordered_map<std::string,std::string> kv;
            UrlUtil::EncodeBody(req.body,kv);
            std::string user_code = kv["code"];
            //用户代码和测试用例代码合并
            //根据id获取题目信息
            Question q;
            model.GetQuestion(req.matches[1].str(),q);
            Json::Value req_json;//从req中提取得到
            req_json["code"] = user_code + q.test_case;
            Json::Value resp_json;//放到响应当中
            //最终编译
            Compiler::CompileAndRun(req_json,resp_json,q.time_limit,q.mem_limit);
            //获取网页
            std::string html = "";
            OJview::GetResultHtml(resp_json["reason"].asString(),resp_json["stdout"].asString(),html);
            resp.set_content(html,"text/html");
            });
    server.set_base_dir("./temlpate");
    server.listen("0.0.0.0",8080);
    return 0;
}
