#include "CompileRun.hpp"
#include<jsoncpp/json/json.h>

int main()
{
  Json::Value req_json;
  Json::Value resp_json;
  req_json["code"] = "#include<stdio.h> \nint main() { printf(\"hello world\");return 0;}";
  req_json["stdin"] = "";
  Compiler::CompileAndRun(req_json,resp_json);
  Json::FastWriter writer;
  LOG(INFO)<<writer.write(resp_json)<<std::endl;
  return 0;
}
