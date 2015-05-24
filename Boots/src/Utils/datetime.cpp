#include <Boots/Utils/datetime.hpp>
#include <ctime>

using namespace std;

DateTime DateTime::Now(void)
{
  return (DateTime(std::time(0)));
}

unsigned short DateTime::DaysThisMonth(void) const
{
  unsigned short  days_per_month_not_bi[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  unsigned short  days_per_month_bi[]     = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  unsigned short  month = Month() - 1;

  return (Year() % 4 == 0 ? days_per_month_bi[month] : days_per_month_not_bi[month]);
}

void DateTime::Add(int amount, TimeUnit unit)
{
  bool do_sub = amount < 0;

  switch (unit)
  {
    case second:
    case minute:
    case hour:
    case day:
      timestamp += amount * unit;
      break ;
    case month:
      amount = do_sub ? -amount : amount;
      while (amount--)
        timestamp += (do_sub ? -1 : 1) * DaysThisMonth() * day;
      break ;
    case year:
      Add(amount * 12, month);
      break ;
  }
}

void DateTime::GetMonthAndDay(unsigned short& month, unsigned short& day) const
{
  unsigned short  days_per_month_not_bi[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  unsigned short  days_per_month_bi[]     = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  unsigned short* days_per_month;
  
  days_per_month = Year() % 4 == 0 ? days_per_month_bi : days_per_month_not_bi;
  month = day = 0;
  day   = DayOfYear();
  while (day > days_per_month[month])
  {
    day -= days_per_month[month];
    ++month;
  }
  month++;
}
