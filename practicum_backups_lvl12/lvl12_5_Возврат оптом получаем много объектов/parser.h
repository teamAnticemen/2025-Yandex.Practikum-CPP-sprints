#pragma once

#include "budget_manager.h"
#include <string_view>

void ParseAndProcessQuery(BudgetManager& manager, std::string_view line);