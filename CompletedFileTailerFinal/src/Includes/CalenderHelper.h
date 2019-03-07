#pragma once
#include <map>
#include <string>
namespace CalenderHelper 
{
  std::map<std::string, int> monthPatternShort;
  std::map<std::string, int> monthPatternLong;
  std::map<std::string, int> weekDayPatternShort;
  std::map<std::string, int> weekDayPatternLong;
  inline void setmonthPatternShort()
  {
    monthPatternShort["jan"] = 1;
    monthPatternShort["feb"] = 2;
    monthPatternShort["mar"] = 3;
    monthPatternShort["apr"] = 4;
    monthPatternShort["may"] = 5;
    monthPatternShort["jun"] = 6;
    monthPatternShort["jul"] = 7;
    monthPatternShort["aug"] = 8;
    monthPatternShort["sep"] = 9;
    monthPatternShort["oct"] = 10;
    monthPatternShort["nov"] = 11;
    monthPatternShort["dec"] = 12;
  }
  inline void setMOnthPatternLong()
  {
    monthPatternLong["january"] = 1;
    monthPatternLong["february"] = 2;
    monthPatternLong["march"] = 3;
    monthPatternLong["april"] = 4;
    monthPatternLong["may"] = 5;
    monthPatternLong["june"] = 6;
    monthPatternLong["july"] = 7;
    monthPatternLong["august"] = 8;
    monthPatternLong["september"] = 9;
    monthPatternLong["october"] = 10;
    monthPatternLong["november"] = 11;
    monthPatternLong["december"] = 12;
  }
  inline void setWeekDayPatternShort()
  {
    weekDayPatternShort["sun"] = 1;
    weekDayPatternShort["mon"] = 2;
    weekDayPatternShort["tue"] = 3;
    weekDayPatternShort["wed"] = 4;
    weekDayPatternShort["thu"] = 5;
    weekDayPatternShort["fri"] = 6;
    weekDayPatternShort["sat"] = 7;
  }
  inline void setWeekDayPatternLong()
  {
    weekDayPatternLong["sunday"] = 1;
    weekDayPatternLong["monday"] = 2;
    weekDayPatternLong["tuesday"] = 3;
    weekDayPatternLong["wednesday"] = 4;
    weekDayPatternLong["thursday"] = 5;
    weekDayPatternLong["friday"] = 6;
    weekDayPatternLong["saturday"] = 7;
  }
}