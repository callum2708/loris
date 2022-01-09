#pragma once

#include <stdint.h>
#include <string.h>
#include "../kernel/kmalloc.h"

typedef void *type_t;
/**
  A predicate should return nonzero if the first argument is less than the second. Else
  it should return zero.
**/
typedef bool (*lessthan_predicate_t)(type_t, type_t);

template <typename T>
class OrderedList
{
private:
  T *m_elements;
  uint32_t m_size;
  uint32_t m_maxSize;
  lessthan_predicate_t m_lessThanPredicate;

public:
  OrderedList(uint32_t max_size, lessthan_predicate_t less_than)
  {
    m_elements = (T *)kmalloc(max_size * sizeof(T *), false, nullptr);
    memset(m_elements, 0, max_size * sizeof(T *));
    m_size = 0;
    m_maxSize = max_size;
    m_lessThanPredicate = less_than;
  }

  OrderedList(void *elementsMemAddress, uint32_t maxSize, lessthan_predicate_t lessThanPrediacate)
  {
    m_elements = (T *)elementsMemAddress;
    memset(m_elements, 0, maxSize * sizeof(T));
    m_size = 0;
    m_maxSize = maxSize;
    m_lessThanPredicate = lessThanPrediacate;
  }

  T GetItemAtIndex(uint32_t index)
  {
    return m_elements[index];
  }

  void Insert(T item)
  {
    uint32_t iterator = 0;
    while (iterator < m_size && m_lessThanPredicate(m_elements[iterator], item))
      iterator++;
    if (iterator == m_size) // just add at the end of the m_elements.
      m_elements[m_size++] = item;
    else
    {
      T tmp = m_elements[iterator];
      m_elements[iterator] = item;
      while (iterator < m_size)
      {
        iterator++;
        T tmp2 = m_elements[iterator];
        m_elements[iterator] = tmp;
        tmp = tmp2;
      }
      m_size++;
    }
  }

  void Remove(uint32_t index)
  {
    while (index < m_size)
    {
      m_elements[index] = m_elements[index + 1];
      index++;
    }
    m_size--;
  }

  const size_t Size() const { return m_size; }
};

bool standard_lessthan_predicate(type_t a, type_t b);