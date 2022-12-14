
#include <deque>

#include "isometric.hpp"

int main(int argc, char** argv)
{
  std::deque<std::string> args{argv, argv + argc};
  std::string exe_path = args[0];
  args.pop_front();

  px::IsometricMap map = px::IsometricMap();

  return map.exec();
}
