////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_EXPRESSION_CONTEXT_H
#define ARANGOD_AQL_EXPRESSION_CONTEXT_H 1

#include "Basics/Common.h"
#include "Basics/Exceptions.h"
#include "Aql/AqlValue.h"
#include "Aql/Variable.h"

namespace arangodb {
namespace aql {

class ExpressionContext {
 public:
  ExpressionContext(size_t startPos, AqlItemBlock const* argv, 
         std::vector<Variable const*> const& vars, std::vector<RegisterId> const& regs)
         : _startPos(startPos), _argv(argv), _vars(&vars), _regs(&regs) {}

  ~ExpressionContext() {}

  size_t numRegisters() const { return _regs->size(); }
  
  AqlValue const& getRegisterValue(size_t i) const {
    return _argv->getValueReference(_startPos, (*_regs)[i]);
  }
  
  Variable const* getVariable(size_t i) const {
    return (*_vars)[i];
  }

  AqlValue getVariableValue(Variable const* variable, bool doCopy, bool& mustDestroy) const {
    mustDestroy = false;

    size_t i = 0;
    for (auto it = (*_vars).begin(); it != (*_vars).end(); ++it, ++i) {
      if ((*it)->id == variable->id) {
        if (doCopy) {
          mustDestroy = true; // as we are copying
          return _argv->getValueReference(_startPos, (*_regs)[i]).clone();
        }
        return _argv->getValueReference(_startPos, (*_regs)[i]);
      }
    }
  
    std::string msg("variable not found '");
    msg.append(variable->name);
    msg.append("' in executeSimpleExpression()");
    THROW_ARANGO_EXCEPTION_MESSAGE(TRI_ERROR_INTERNAL, msg.c_str());
  }

  /// @brief temporary storage for expression data context
  size_t _startPos;
  AqlItemBlock const* _argv;
  std::vector<Variable const*> const* _vars;
  std::vector<RegisterId> const* _regs;
};
}
}

#endif
