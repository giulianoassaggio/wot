#pragma once

#include <interfaces/db.hpp>

namespace wot {

template<class K, class V>
class ReadonlyDb : public Db<K,V>{
private:
  const std::string name;

public:
  ReadonlyDb(const std::string & db_name) : name{db_name} {}
  ReadonlyDb() : name{""} {}
  ~ReadonlyDb() {}

  bool add(const K &, const V &) override {
    throw(std::logic_error("Database is readonly"));
  }

  bool rm(const K &) override {
    throw(std::logic_error("Database is readonly"));
  }

  virtual bool get(const K &, V &) const = 0;

  virtual void keys(std::set<K> &) const = 0;
};

} // namespace wot
