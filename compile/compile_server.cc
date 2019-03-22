#include "httplib.h"
#include "CompileRun.hpp"
#include <jsoncpp/json/json.h>
int main()
{
  using namespace httplib;
  Server ser;
  ser.Post("/compile",[](const Request& req,Response &resp){
      //使用JSON格式序列化数据
      //1、如何从req中获取JSON请求？
      //2、JSON如何和HTTP结合？
      //浏览器请求包含我们所需要的数据一般可以存放的位置有三种
      //1）在url中-----一般GET方法把自定制数据存放在url中，以键值对的形式存放
      //键和值之间用=分隔，键值对之间用&符号分隔，用户数据中也可能包含&，=
      //特殊符号，所以传输会进行转码
      //2）在header
      //3）在body部分----一般POST方法把数据存放在body中，存储形式由头部content-type
      //决定，如果Content-Type：application/x-www-form-urlencoded，这种一般用户数据是以
      //键值对方式存储，键值对之间以&符号分隔，键和值用=分隔，为了防止键和值中有特殊符号如&，=
      //所以这种方式在传输过程中会进行转码，那么用户得到的请求数据就需要解码。
      //如果Content-Type：application/json;这种方式非常流行，服务端得到的请求就是json串，
      //不像之前转码之后的样子。另外这种格式用户数据必须要是字符串，html表单不支持，需要使用js
      //进行构造，为了项目的可拓展性，比如增加对mysql语句的处理使用application/x-www-form-urlencoded比较好。
      
      //这里我们规定把用户自定制数据放在正文中，所以使用Post方法
      //httplib中Post中Content-Type默认字段是application/x-www-form-urlencoded
      //所以，我们服务端需要做两件事情
      //（1）对浏览器响应数据组织成JSON格式
      //（2）解码
      //通过unordered_map组织键值对
      std::unordered_map<std::string,std::string> kv;
      UrlUtil::EncodeBody(req.body,kv);
      Json::Value req_json;//从req中提取得到
      for(auto &e:kv){
        req_json[e.first] =e.second;
      }
      //3、如何解析JSON格式和构造，使用第三方库jsoncpp？
      Json::Value resp_json;//放到响应当中
      Compiler::CompileAndRun(req_json,resp_json);
      //这里需要把Json::Value格式转化为字符串，才可以响应
      Json::FastWriter writer;
      resp.set_content(writer.write(resp_json),"text/plain");
      });
  //这条语句为了让浏览器访问到静态页面
  //静态页面：每个用户访问都一样
  //动态页面：随着用户访问要求，不断发生变化
  ser.set_base_dir("./compile_root");
  ser.listen("0.0.0.0",9000);
  return 0;
}
