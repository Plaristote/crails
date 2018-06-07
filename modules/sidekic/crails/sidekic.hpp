#ifndef  SIDEKIC_HPP
# define SIDEKIC_HPP

# include <crails/datatree.hpp>

struct Sidekic
{
  static const std::string tasks_path;
  static std::string async_task(const std::string& name, Data params);
  static std::string schedule_task(std::time_t timestamp, const std::string& name, Data params);
};

#endif
