#include "httplib.h"
#include <iostream>
int main()
{
  using namespace httplib;
  Server ser;
  ser.Get("/",[](const Request& req,Response &resp){
      resp.set_content("<html>hello world</html>","text/html");
      });
  ser.listen("0.0.0.0",9000);
  return 0;
}
