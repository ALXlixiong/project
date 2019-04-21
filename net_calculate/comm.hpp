#pragma once 
#include <iostream>
//日期类功能函数：
//                           1：计算一个日期加上多少天数后的日期       
//                           2：把该日期改为加上指定数目的天数后的日期
//                           3：一个日期减上多少天数后的日期 
//                           4：把该日期改为减去指定数目的天数后的日期
//                           5：该日期加1（前置++）（后置++）
//                           6：该日期减1（前置--）（后置--）
//                           7：计算某日期到未来某日期间隔的天数
class Date 
{
  friend std::ostream& operator<<(std::ostream& _cout,const Date& d);
  friend std::istream& operator>>(std::istream& _cin,Date& d);
  private:
  int _year;
  int _month;
  int _day;
  public:
  Date(int year = 1990,int month = 1,int day = 1)
    :_year(year)
     ,_month(month)
     ,_day(day)
  {}
  Date(const Date &d)
  {
    _year = d._year;
    _month = d._month;
    _day = d._day;
  }
  Date& operator=(const Date &d)
  {
    if(this != &d)
    {
      _year = d._year;
      _month = d._month;
      _day = d._day;
    }
    return *this;
  }
  bool operator==(const Date &d)
  {
    if(_year == d._year && _month == d._month && _day == d._day)
      return true;
    return false;
  }
  Date operator++(int)
  {
    Date tmp(*this);
    *this = *this+1;
    return tmp;
  }
  Date operator++()
  {
    *this = *this+1;
    return *this;
  }
  Date operator--(int)
  {
    Date tmp(*this);
    *this = *this-1;
    return tmp;
  }
  Date operator--()
  {
    *this = *this-1;
    return *this;
  }
  int InvalidDay(const Date& tmp)
  {
    int arr[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if((tmp._year % 4 == 0 && tmp._year % 100 != 0)||tmp._year % 400 == 0)
      arr[2] += 1;
    return arr[tmp._month];
  }
  Date operator-(int days)
  {
    Date tmp(*this);
    tmp._day -= days;
    while(tmp._day<=0)
    {
      tmp._month -= 1;
      if(tmp._month<1)
      {
        tmp._month = 12;
        tmp._year -= 1;
      }
      tmp._day += InvalidDay(tmp);
    }
    return tmp;
  }
  Date operator+(int days)
  {
    Date tmp(*this);
    tmp._day += days;
    while(tmp._day > InvalidDay(tmp))
    {
      tmp._month += 1;
      if(tmp._month>12)
      {
        tmp._year += 1;
        tmp._month = 1;
      }
      tmp._day -= InvalidDay(tmp);
    }
    return tmp;
  }
  Date& operator-=(int days)
  {
    *this = *this-days;
    return *this;
  }
  Date& operator+=(int days)
  {
    *this = *this+days;
    return *this;
  }
  bool operator>(const Date& d)
  {
    if(_year>d._year || (_year == d._year && _month>d._month)
        || (_year == d._year && _month == d._month && _day>d._day))
      return true;
    return false;
  }
  bool operator<(const Date& d)
  {
    if(*this>d)
      return false;
    return true;
  }
  int operator-(const Date& d)
  {
    int count = 0;
    Date tmp(d);
    if(tmp>*this)
    {
      while(!(tmp == *this))
      {
        --tmp;
        ++count;
      }
    }
    else if(tmp<*this)
    {
      while(!(tmp == *this))
      {
        ++tmp;
        ++count;
      }
    }
    else 
      return 0;
    return count;
  }
};

std::ostream& operator<<(std::ostream& _cout,const Date& d)
{
  _cout<<d._year<<"-"<<d._month<<"-"<<d._day;
  return _cout;

}

std::istream& operator>>(std::istream& _cin, Date& d)
{
  _cin>>d._year;
  _cin>>d._month;
  _cin>>d._day;
  return _cin;
}
struct Request_t{
  double cal_kind;//calculate kind (1)date(2)number(3)conversion of number systems
  double x;
  double y;
  Date d1;
  Date d2;
  char op;
};

struct Response_t{
  int status;
  double result;
  Date result_date;
  int res[32];
};
