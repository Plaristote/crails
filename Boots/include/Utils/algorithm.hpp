#ifndef  BOOTS_ALGORITHM_HPP
# define BOOTS_ALGORITHM_HPP

namespace Algorithm
{
  template<typename ARRAY, typename COMP_TYPE>
  int sorted_find(ARRAY& array, COMP_TYPE value, int index_first = 0, int index_last = -1)
  {
    int middle;
  
    if (index_last == -1)
      index_last = array.size();
    middle = index_first + ((index_last - index_first) / 2);
    if (index_first == index_last && !(array[index_first] == value))
      return (-1);

    if      (array[middle] == value)
      return (middle);
    else if (array[middle] > value)
      return (sorted_find(array, value, index_first, middle));
    else
      return (sorted_find(array, value, middle, array.size()));
    return (-1);
  }
}

#endif
