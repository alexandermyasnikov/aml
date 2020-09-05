#pragma once

#include <memory>
#include <deque>
#include "utils.h"

namespace aml::env_n {
  namespace utils_n = aml::utils_n;



  struct var_info_t {
    size_t      id = {};
    std::string name;
    size_t      offset = {};

    std::string show() const;
  };

  using var_info_sptr_t = std::shared_ptr<var_info_t>;



  struct env_t : std::enable_shared_from_this<env_t> {
    using key_t = std::string;
    using val_t = var_info_sptr_t;
    using vars_t = std::deque<val_t>;

    std::shared_ptr<env_t> parent;
    static inline size_t   id = {};
    vars_t                 funcs;
    vars_t                 vars;

    env_t(std::shared_ptr<env_t> parent = nullptr) : parent(parent) { }

    val_t def(const auto& key, auto pvars) {
      auto it = std::find_if((this->*pvars).begin(), (this->*pvars).end(),
          [&key](auto var) { return key == var->name; });
      if (it != (this->*pvars).end())
        throw utils_n::fatal_error_t("env_t: '" + key + "' is exists");

      (this->*pvars).push_back(std::make_shared<var_info_t>());
      (this->*pvars).back()->id = id++;
      (this->*pvars).back()->name = key;
      return (this->*pvars).back();
    }

    val_t get(const auto& key, auto pvar, auto pvars) const {
      auto env = this->shared_from_this();
      while (env) {
        auto it = std::find_if((*env.*pvars).begin(), (*env.*pvars).end(),
            [&key, pvar](auto var) { return key == (*var).*pvar; });
        if (it != (*env.*pvars).end()) {
          return *it;
        }
        env = env->parent;
      }

      std::string key_str;
      if constexpr (std::is_arithmetic<decltype(key)>::value) {
        key_str = std::to_string(key);
      } else {
        key_str = key;
      }
      throw utils_n::fatal_error_t("env_t: '" + key_str + "' is not exists");
    }

    val_t def_func(const key_t& key) {
      return def(key, &env_t::funcs);
    }

    val_t get_func(const key_t& key) const {
      return get(key, &var_info_t::name, &env_t::funcs);
    }

    val_t get_func(size_t key) const {
      return get(key, &var_info_t::id, &env_t::funcs);
    }

    val_t def_var(const key_t& key) {
      return def(key, &env_t::vars);
    }

    val_t get_var(const key_t& key) const {
      return get(key, &var_info_t::name, &env_t::vars);
    }

    std::string show() const;
  };

  using env_sptr_t = std::shared_ptr<env_t>;
}
