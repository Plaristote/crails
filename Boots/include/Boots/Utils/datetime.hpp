#ifndef  DATETIME_HPP
# define DATETIME_HPP

class DateTime
{
public:
  enum TimeUnit
  {
    second = 1,
    minute = 60 * second,
    hour   = 60 * minute,
    day    = 24 * hour,
    month,
    year
  };
  
  DateTime(unsigned int timestamp) : timestamp(timestamp)
  {}
  
  operator unsigned int() const { return (timestamp); }
  
  bool     operator!=(DateTime dt) const { return (dt.timestamp != timestamp); }
  bool     operator==(DateTime dt) const { return (dt.timestamp == timestamp); }
  bool     operator<(DateTime dt)  const { return (dt.timestamp > timestamp);  }
  bool     operator>(DateTime dt)  const { return (dt.timestamp < timestamp);  }
  DateTime operator+(DateTime dt)  const { return (DateTime(dt.timestamp + timestamp)); }
  DateTime operator-(DateTime dt)  const { return (DateTime(dt.timestamp - timestamp)); }
  
  static DateTime Now(void);
  
  unsigned short Year(void) const
  {
    return (1970 + (timestamp / (86400 * 365.2425)));
  }

  unsigned short Month(void) const
  {
    unsigned short month, day;

    GetMonthAndDay(month, day);
    return (month);
  }
  
  unsigned short DayOfYear(void) const
  {
    return ((timestamp - ((Year() - 1970) * 86400 * 365.2425)) / 86400);
  }

  unsigned short Day(void) const
  {
    unsigned short month, day;

    GetMonthAndDay(month, day);
    return (day);
  }

  unsigned short Hour(void) const
  {
    return ((timestamp % 86400) / 3600);
  }

  unsigned short Minute(void) const
  {
    return ((timestamp % 3600) / 60);
  }

  unsigned short Second(void) const
  {
    return (timestamp % 60);
  }
  
  unsigned short DaysThisMonth(void) const;
  
  void Add(int amount, TimeUnit unit);

private:
  void GetMonthAndDay(unsigned short& month, unsigned short& day) const;

  unsigned int timestamp;
};

#endif