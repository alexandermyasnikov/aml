#include "env.h"
#include "utils.h"

namespace aml::env_n {
  namespace utils_n = aml::utils_n;



  std::string var_info_t::show() const {
    return std::to_string(id) + "\t'" + name + "'\t" + std::to_string(offset);
  }



  env_t::val_t env_t::def(const auto& key, auto pvars) {
    auto it = std::find_if((this->*pvars).begin(), (this->*pvars).end(),
        [&key](auto var) { return key == var->name; });
    if (it != (this->*pvars).end())
      throw utils_n::fatal_error_t("env_t: '" + key + "' is exists");

    (this->*pvars).push_back(std::make_shared<var_info_t>());
    (this->*pvars).back()->id = id++;
    (this->*pvars).back()->name = key;
    return (this->*pvars).back();
  }

  env_t::val_t env_t::get(const auto& key, auto pvar, auto pvars) const {
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

  std::string env_t::show() const {
    std::string str;
    auto env = this->shared_from_this();
    size_t deep = {};
    while (env) {
      for (const auto& func : env->funcs) {
        str += "; func: " + utils_n::indent(deep) + func->show() + "\n";
      }
      for (const auto& var : env->vars) {
        str += "; var:  " + utils_n::indent(deep) + var->show() + "\n";
      }
      env = env->parent;
      deep++;
    }
    return str;
  }
}
