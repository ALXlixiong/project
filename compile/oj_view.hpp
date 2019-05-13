#include "oj_model.hpp"
#include <ctemplate/template.h>
class OJview{
    public:
        //把所有的数据转化为html题目列表页
        static void GetAllQuestionHtml(std::vector<Question>& aq,std::string &html)
        {
           // html += "<html>";
           // html += "<body>";
           // html += "<div>";
           // html += aq[0].id += aq[0].name....
           // html += "</div>";
           // html += "</body>";
           // html += "</html>";
           // 通过网页模板构造字符串html
            ctemplate::TemplateDictionary dict("aq");
            for(const auto& e:aq){
                ctemplate::TemplateDictionary *table_dict;
                table_dict = dict.AddSectionDictionary("question");
                table_dict->SetValue("id",e.id);
                table_dict->SetValue("name",e.name);
                table_dict->SetValue("star",e.star);
            }
            ctemplate::Template* tpl;
            tpl = ctemplate::Template::GetTemplate(
                    "./temlpate/all_question.html", ctemplate::DO_NOT_STRIP);
            tpl->Expand(&html, &dict);
        }

        static void GetAllQuestionHtml(Question &q,std::string &html)
        {
            ctemplate::TemplateDictionary dict("q");
            dict.SetValue("id",q.id);
            dict.SetValue("name",q.name);
            dict.SetValue("star",q.star);
            dict.SetValue("describe",q.describe);
            dict.SetValue("structure",q.code_structure);
            ctemplate::Template* tpl;
            tpl = ctemplate::Template::GetTemplate(
                    "./temlpate/question.html", ctemplate::DO_NOT_STRIP);
            tpl->Expand(&html, &dict);
        }

        static void GetResultHtml(const std::string &reason, const std::string &_stdout,
                std::string &html)
        {
            ctemplate::TemplateDictionary dict("result");
            dict.SetValue("reason",reason);
            dict.SetValue("_stdout",_stdout);
            ctemplate::Template* tpl;
            tpl = ctemplate::Template::GetTemplate(
                    "./temlpate/result.html", ctemplate::DO_NOT_STRIP);
            tpl->Expand(&html, &dict);

        }
};
