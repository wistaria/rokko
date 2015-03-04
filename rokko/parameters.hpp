/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2015 Rokko Developers https://github.com/t-sakashita/rokko
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_PARAMETERS_HPP
#define ROKKO_PARAMETERS_HPP

#include <list>
#include <map>
#include <string>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace rokko {

class parameters {
private:
  typedef std::map<std::string, std::string> map_type;
  typedef map_type::value_type value_type;
  typedef map_type::mapped_type mapped_type;
public:
  typedef map_type::key_type key_type;
  void clear() { map_.clear(); }
  void clear(key_type const& key) { map_.erase(map_.find(key)); }
  // set "value" for "key"
  template<typename T>
  void set(key_type const& key, T const& value) {
    map_[key] = boost::lexical_cast<mapped_type>(value);
  }
  // returns if parameter with "key" is defined or not
  bool defined(key_type const& key) const {return (map_.find(key) != map_.end()); }
  // returns list of keys
  std::list<key_type> keys() const {
    std::list<key_type> keys;
    BOOST_FOREACH(value_type const& p, map_) { keys.push_back(p.first); }
    return keys;
  }
  // returns value of parameter in type T
  template<typename T>
  T get(key_type const& key) const { return boost::lexical_cast<T>(map_.find(key)->second); }
private:
  std::map<std::string, std::string> map_;
};

} // namespace rokko

#endif // ROKKO_PARAMETERS_HPP