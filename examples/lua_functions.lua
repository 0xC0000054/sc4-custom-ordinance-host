--#-package:3c2028a9# -- package signature --
-- Any Lua files that depend on this file must have higher package numbers.

-- Calculates the ordinance's monthly expense/income.
function null45_get_monthly_income(monthlyConstantIncome)
local extra = game.g_city_rci_population * -0.001
return monthlyConstantIncome + extra
end

-- Returns a value indicating if the ordinance should
-- be available to select in the ordinance list.
function null45_check_conditions()
return true
end

-- EOF