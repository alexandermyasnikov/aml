#include "env.h"
#include "utils.h"

namespace aml::env_n {
  namespace utils_n = aml::utils_n;



  std::string var_info_t::show() const {
    return std::to_string(id) + "\t'" + name + "'\t" + std::to_string(offset);
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
