/*****************************************************************************
*
* Rokko: Integrated Interface for libraries of eigenvalue decomposition
*
* Copyright (C) 2012-2014 by Synge Todo <wistaria@comp-phys.org>
*
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
*****************************************************************************/

#ifndef ROKKO_FACTORY_HPP
#define ROKKO_FACTORY_HPP

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>
#include <vector>

namespace rokko {

template<typename BASE>
class factory : private boost::noncopyable {
private:
  class abstract_creator {
  public:
    virtual ~abstract_creator() {}
    virtual boost::shared_ptr<BASE> create() const = 0;
  };
  template<typename PRODUCT>
  class creator : public abstract_creator {
  public:
    virtual ~creator() {}
    boost::shared_ptr<BASE> create() const {
      return boost::shared_ptr<BASE>(new PRODUCT());
    }
  };

public:
  typedef boost::shared_ptr<BASE> product_pointer_type;

private:
  typedef boost::shared_ptr<abstract_creator> creator_pointer_type;
  typedef std::map<std::string, creator_pointer_type> creator_map_type;
public:
  factory() : largest_priority_(0) {}
  static product_pointer_type make_product(std::string const& name = "") {
    factory* f = factory::instance();
    if (name == "") {
      if (f->default_product_ != "") {
        return f->make_creator(f->default_product_)->create();
      } else {
        std::cerr << "Error: default product is not defined\n";
        boost::throw_exception(std::runtime_error("factory::make_product()"));
      }
    } else {
      return f->make_creator(name)->create();
    }
  }
  template<typename PRODUCT>
  bool register_creator(std::string const& name, int priority = 0) {
    bool isnew = (creators_.find(name) == creators_.end());
    creators_[name] = creator_pointer_type(new creator<PRODUCT>());
    if (priority >= largest_priority_) {
      largest_priority_ = priority;
      default_product_ = name;
    }
    return isnew;
  }
  bool unregister_creator(std::string const& name); // to be implemented
  static std::vector<std::string> product_names() {
    factory* f = factory::instance();
    std::vector<std::string> retvec;
    for (typename creator_map_type::const_iterator it = f->creators_.begin();
         it != f->creators_.end(); ++it) {
      retvec.push_back(it->first);
    }
    return retvec;
  }

  static std::string default_product_name() {
    return instance()->default_product_;
  }
  static factory* instance() {
    if (!instance_) instance_ = new factory;
    return instance_;
  }
protected:
  creator_pointer_type make_creator(std::string const& name) const {
    typename creator_map_type::const_iterator itr = creators_.find(name);
    if (itr == creators_.end() || itr->second == 0) {
      std::cerr << "Error: unknown product: \"" << name << "\" (registered products: ";
      for (typename creator_map_type::const_iterator itr = creators_.begin();
           itr != creators_.end(); ++itr) {
        if (itr != creators_.begin()) std::cerr << ", ";
        std::cerr << "\"" << itr->first << "\"";
      }
      std::cerr << ")\n";
      boost::throw_exception(std::runtime_error("factory::make_creator()"));
    }
    return itr->second;
  }
      
private:
  static factory* instance_;
  creator_map_type creators_;
  int largest_priority_;
  std::string default_product_;
};

} // end namespace rokko

#define ROKKO_REGISTER_PRODUCT(base, product, name, priority) \
  namespace { namespace BOOST_JOIN(product_register, __LINE__) { struct register_caller { register_caller() { rokko::factory::instance()->register_creator<product>(name, priority); } } caller; } }

#endif // ROKKO_FACTORY_HPP
