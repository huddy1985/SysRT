/***************************************************************************
Author Jun Xiao and Giuseppe Lipari
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __PLIST_HPP__
#define __PLIST_HPP__

#include <set>
#include <iostream>
#include <baseexc.hpp>
#include<algorithm>
/**
   Priority List built according to STL framework. 
*/
#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class T, 
	  class Compare = std::less<T>,
	  class Alloc = std::allocator<T> >
#else
template <class T, class Compare, class Alloc = alloc>
#endif
class priority_list : private std::set<T, Compare, Alloc> {
  typedef std::set<T, Compare, Alloc> Impl;
public:
  typedef typename Impl::iterator iterator;
  typedef typename Impl::const_iterator const_iterator;
  typedef typename Impl::pointer pointer;
  typedef typename Impl::const_pointer const_pointer;
  typedef typename Impl::reference reference;
  typedef typename Impl::const_reference const_reference;
  typedef typename Impl::reverse_iterator reverse_iterator;
  typedef typename Impl::const_reverse_iterator const_reverse_iterator;
  typedef typename Impl::size_type size_type;
  typedef typename Impl::difference_type difference_type;

  iterator begin() { return Impl::begin(); }
  iterator end() { return Impl::end(); }
  const_iterator begin() const { return Impl::begin(); }
  const_iterator end() const { return Impl::end(); }
  reverse_iterator rbegin() const { return Impl::rbegin(); }
  reverse_iterator rend() const { return Impl::rend(); }

  std::pair<iterator,bool> insert(const_reference x) 
  {
    return Impl::insert(x);
  };
  
  void erase(const_reference x) {
	  //pointer it = x;
	  iterator it_bug = std::find(begin(), end(), x);
	  Impl::erase(it_bug);
  }

  const_reference front() const { return *begin(); }
  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }
  bool empty() {return Impl::empty(); }
  void clear() { Impl::clear(); }
  size_type size() const { return Impl::size(); }
};

#endif
