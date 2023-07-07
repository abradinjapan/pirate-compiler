#pragma once

#include <vector>

#include "parse.cpp"

namespace variable_abstracter {
    class variable {
    public:
        std::string p_name;

        variable() {
            p_name = "";
        }

        variable(std::string name) {
            p_name = name;
        }
    };

    class scope {
    public:
        std::vector<variable> p_input_variables;
        std::vector<variable> p_output_variables;
        std::vector<variable> p_scope_variables;

        // check if inputs contains a variable
        bool input_variable_already_defined(variable variable) {
            // check each variable
            for (uint64_t i = 0; i < p_input_variables.size(); i++) {
                // check for variable declaration
                if (p_input_variables[i].p_name == variable.p_name) {
                    // it is defined
                    return true;
                }
            }

            // it is not defined
            return false;
        }

        // check if outputs contains a variable
        bool output_variable_already_defined(variable variable) {
            // check each variable
            for (uint64_t i = 0; i < p_output_variables.size(); i++) {
                // check for variable declaration
                if (p_output_variables[i].p_name == variable.p_name) {
                    // it is defined
                    return true;
                }
            }

            // it is not defined
            return false;
        }
        
        // check if scope contains a variable
        bool scope_variable_already_defined(variable variable) {
            // check each variable
            for (uint64_t i = 0; i < p_scope_variables.size(); i++) {
                // check for variable declaration
                if (p_scope_variables[i].p_name == variable.p_name) {
                    // it is defined
                    return true;
                }
            }

            // it is not defined
            return false;
        }

        // attempt to add an input variable, returns boolean for variable added or not
        bool add_input_variable(variable variable) {
            // make sure variable is not defined in the inputs or outputs already
            if (input_variable_already_defined(variable) || output_variable_already_defined(variable)) {
                // variable could not be added
                return false;
            }

            // add variable
            p_input_variables.push_back(variable);

            // variable added
            return true;
        }

        // attempt to add an output variable, returns boolean for variable added or not
        bool add_output_variable(variable variable) {
            // make sure variable is not defined in the inputs or outputs already
            if (input_variable_already_defined(variable) || output_variable_already_defined(variable)) {
                // variable could not be added
                return false;
            }

            // add variable
            p_input_variables.push_back(variable);

            // variable added
            return true;
        }

        // add the variable if it doesn't already exist
        void add_scope_variable(variable variable) {
            // add the variable if not defined
            if (scope_variable_already_defined(variable) == false) {
                // add variable to scope
                p_scope_variables.push_back(variable);
            }
        }
    };

    // all variables across all functions
    class scope_table {
    public:

    };

    // get all variables in an abstraction
    scope abstract_variables_in_abstraction(parser::abstraction& abstraction, bool& error_occured) {

    }
}
