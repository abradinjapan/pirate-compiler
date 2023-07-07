#pragma once

/* Notes */
/*
    The goal of this file is to filter out all abstraction definitions and abstraction calls.
    The code must contain one unique abstraction header for each abstraction.
*/

#include "parse.cpp"

#include <vector>

namespace accounter {
    class header {
    public:
        std::string p_name;
        int p_input_count;
        int p_output_count;

        header() {
            p_name = "";
            p_input_count = 0;
            p_output_count = 0;
        }

        header(std::string name, int input_count, int output_count) {
            p_name = name;
            p_input_count = input_count;
            p_output_count = output_count;
        }
    };

    // check if two headers have matching signatures
    bool match_headers(header a, header b) {
        return (a.p_name == b.p_name) && (a.p_input_count == b.p_input_count) && (a.p_output_count == b.p_output_count);
    }

    // a type that keeps track of all header declarations
    class header_table {
    public:
        std::vector<header> p_headers;
        
        // check to see if a header is registered
        bool header_registered(header header) {
            // check each header
            for (uint64_t i = 0; i < p_headers.size(); i++) {
                // check header for match
                if (match_headers(p_headers[i], header) == true) {
                    // match found
                    return true;
                }
            }

            // no matches found
            return false;
        }

        // attempt to register a new header, false means a duplicate was found, true means registration was successful
        bool try_register_header(header header) {
            // check to see if header already exists, if so, reject
            if (header_registered(header) == true) {
                // header could not be added
                return false;
            }

            // header was not already registered, add new header
            p_headers.push_back(header);

            // header added successfully
            return true;
        }
    };

    header_table get_header_table(parser::program& program, bool& error_occured) {
        header_table output;

        // add abstractions to header table
        for (uint64_t i = 0; i < program.p_abstractions.size(); i++) {
            // try to add a header
            if (output.try_register_header(header(program.p_abstractions[i].p_header.p_name.p_name_value, program.p_abstractions[i].p_header.p_inputs.size(), program.p_abstractions[i].p_header.p_outputs.size())) == false) {
                // error, header re-registered
                std::cout << "Abstracting Error: Duplicate Header Found : " << program.p_abstractions[i].p_header.p_name.p_name_value << std::endl;

                // set error
                error_occured = true;

                return output;
            }
        }

        return output;
    }

    void print_header_table(header_table table) {
        std::cout << "Header Table:" << std::endl;

        // print each header
        for (uint64_t i = 0; i < table.p_headers.size(); i++) {
            // print header
            std::cout << "\tHeader: " << table.p_headers[i].p_name << "(" << table.p_headers[i].p_input_count << ")(" << table.p_headers[i].p_output_count << ")" << std::endl;
        }

        // new line
        std::cout << std::endl;
    }

    // verifies that all statements match a header
    bool verify_all_headers(header_table header_table, parser::program& program) {
        // for each abstraction
        for (uint64_t i = 0; i < program.p_abstractions.size(); i++) {
            // for each statement in each abstraction
            for (uint64_t j = 0; j < program.p_abstractions[i].p_scope.size(); j++) {
                // check header
                if (program.p_abstractions[i].p_scope[j].p_name.p_name_type == parser::name_type::is_abstraction_name && header_table.header_registered(header(program.p_abstractions[i].p_scope[j].p_name.p_name_value, program.p_abstractions[i].p_scope[j].p_inputs.size(), program.p_abstractions[i].p_scope[j].p_outputs.size())) == false) {
                    // header not registered, error
                    return false;
                }
            }
        }

        // all headers matched correctly
        return true;
    }

    class variable {
    public:
        std::string p_name;
        uint64_t p_declaration_index;

        variable(std::string name, uint64_t declaration_index) {
            p_name = name;
            p_declaration_index = declaration_index;
        }
    };

    #define declared_as_input -2
    #define declared_as_output -1

    class variable_table {
    public:
        std::vector<variable> p_inputs;
        std::vector<variable> p_outputs;
        std::vector<variable> p_variables;

        // checks if the input list has a specific input
        bool table_contains_input(std::string name) {
            // for each variable
            for (uint64_t input_ID = 0; input_ID < p_inputs.size(); input_ID++) {
                // check if it is a match
                if (p_inputs[input_ID].p_name == name) {
                    // match found, return found as true
                    return true;
                }
            }

            // not found
            return false;
        }

        // checks if the output list has a specific output
        bool table_contains_output(std::string name) {
            // for each variable
            for (uint64_t output_ID = 0; output_ID < p_outputs.size(); output_ID++) {
                // check if it is a match
                if (p_outputs[output_ID].p_name == name) {
                    // match found, return found as true
                    return true;
                }
            }

            // not found
            return false;
        }

        // checks if the variable list has a specific variable
        bool table_contains_variable(std::string name) {
            // for each variable
            for (uint64_t variable_ID = 0; variable_ID < p_variables.size(); variable_ID++) {
                // check if it is a match
                if (p_variables[variable_ID].p_name == name) {
                    // match found, return found as true
                    return true;
                }
            }

            // not found
            return false;
        }

        bool try_register_input(variable variable) {
            // check to see if the variable is already registered in the abstraction header
            if (table_contains_input(variable.p_name) == true || table_contains_output(variable.p_name) == true) {
                // if so, error
                return false;
            }

            // register input
            p_inputs.push_back(variable);

            // registered successfully
            return true;
        }

        bool try_register_output(variable variable) {
            // check to see if the variable is already registered in the abstraction header
            if (table_contains_input(variable.p_name) == true || table_contains_output(variable.p_name) == true) {
                // if so, error
                return false;
            }

            // register output
            p_outputs.push_back(variable);

            // registered successfully
            return true;
        }
        
        void register_variable(variable variable) {
            // check to see if the variable is already registered anywhere
            if (table_contains_input(variable.p_name) == true || table_contains_output(variable.p_name) == true || table_contains_variable(variable.p_name) == true) {
                // variable already declared
                return;
            }

            // register variable
            p_variables.push_back(variable);

            // registered successfully
            return;
        }
    };

    variable_table get_variable_table(parser::abstraction& abstraction, bool& error_occured) {
        variable_table output;

        // register the input variables
        for (uint64_t input_ID = 0; input_ID < abstraction.p_header.p_inputs.size(); input_ID++) {
            // try to register input
            if (output.try_register_input(variable(abstraction.p_header.p_inputs[input_ID].p_name_value, declared_as_input)) == false) {
                // duplicate header argument error occured
                error_occured = true;

                // inform user of failure
                std::cout << "Accouting Error: Duplicate header argument: " << abstraction.p_header.p_inputs[input_ID].p_name_value << std::endl;

                // return early
                return output;
            }
        }

        // register the output variables
        for (uint64_t output_ID = 0; output_ID < abstraction.p_header.p_outputs.size(); output_ID++) {
            // try to register output
            if (output.try_register_output(variable(abstraction.p_header.p_outputs[output_ID].p_name_value, declared_as_output)) == false) {
                // duplicate header argument error occured
                error_occured = true;

                // inform user of failure
                std::cout << "Accouting Error: Duplicate header argument: " << abstraction.p_header.p_outputs[output_ID].p_name_value << std::endl;

                // return early
                return output;
            }
        }

        // in each statement
        for (uint64_t statement_ID = 0; statement_ID < abstraction.p_scope.size(); statement_ID++) {
            // ensure statement is an abstraction call
            if (abstraction.p_scope[statement_ID].p_type == parser::statement_type::is_abstraction_call) {
                // get variables
                for (uint64_t output_ID = 0; output_ID < abstraction.p_scope[statement_ID].p_outputs.size(); output_ID++) {
                    // check to be sure that the name is a variable name
                    if (abstraction.p_scope[statement_ID].p_outputs[output_ID].p_name_type == parser::name_type::is_value_name) {
                        // try to add variable to variable list
                        output.register_variable(variable(abstraction.p_scope[statement_ID].p_outputs[output_ID].p_name_value, statement_ID));
                    }
                }
            }
        }

        // table created, return
        return output;
    }

    class accounting_table {
    public:
        header_table p_header_table;
        std::vector<variable_table> p_abstraction_variable_tables;
    };

    // account program
    accounting_table account_program(parser::program& program, bool& error_occured) {
        accounting_table output;
        
        // get header table
        output.p_header_table = get_header_table(program, error_occured);

        // verify header table
        if (accounter::verify_all_headers(output.p_header_table, program) == true) {
            std::cout << "All headers and statements match correctly." << std::endl;
        } else {
            std::cout << "Error: Headers and statements do not match." << std::endl;
            error_occured = true;
        }

        // get variable tables
        for (uint64_t abstraction_ID = 0; abstraction_ID < program.p_abstractions.size(); abstraction_ID++) {
            // get table
            output.p_abstraction_variable_tables.push_back(get_variable_table(program.p_abstractions[abstraction_ID], error_occured));

            // check for error
            if (error_occured) {
                return output;
            }
        }

        return output;
    }

    // print variable table
    void print_variable_table(variable_table table) {
        // start information
        std::cout << "Variable Table:" << std::endl;

        // print inputs
        std::cout << "\tAbstraction Inputs:" << std::endl;
        for (uint64_t i = 0; i < table.p_inputs.size(); i++) {
            std::cout << "\t\tInput: " << table.p_inputs[i].p_name << " [ " << (long long)table.p_inputs[i].p_declaration_index << " ]" << std::endl;
        }

        // print outputs
        std::cout << "\tAbstraction Ouputs:" << std::endl;
        for (uint64_t i = 0; i < table.p_outputs.size(); i++) {
            std::cout << "\t\tOutput: " << table.p_outputs[i].p_name << " [ " << (long long)table.p_outputs[i].p_declaration_index << " ]" << std::endl;
        }

        // print variables
        std::cout << "\tAbstraction Variables:" << std::endl;
        for (uint64_t i = 0; i < table.p_variables.size(); i++) {
            std::cout << "\t\tVariable: " << table.p_variables[i].p_name << " [ " << (long long)table.p_variables[i].p_declaration_index << " ]" << std::endl;
        }
    }

    // print accouting table
    void print_accouting_table(accounting_table table) {
        // print header table
        print_header_table(table.p_header_table);

        // print all variable tables
        for (uint64_t table_ID = 0; table_ID < table.p_abstraction_variable_tables.size(); table_ID++) {
            // print variable table
            print_variable_table(table.p_abstraction_variable_tables[table_ID]);
        }
    }
}
