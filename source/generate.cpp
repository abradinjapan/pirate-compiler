#pragma once

#include <vector>

#include "account.cpp"
#include "run.cpp"

namespace generator {
    int calculate_variable_index(accounter::skeleton::abstraction& abstraction, accounter::skeleton::argument& argument) {
        // calculate variable ID based on type and index
        if (argument.p_type == accounter::skeleton::argument_type::is_input) {
            // calculate input variable index
            return argument.p_ID;
        } else if (argument.p_type == accounter::skeleton::argument_type::is_output) {
            // calculate output variable index
            return abstraction.p_variables.p_inputs.size() + argument.p_ID;
        } else {
            // calculate variable index
            return abstraction.p_variables.p_inputs.size() + abstraction.p_variables.p_outputs.size() + argument.p_ID;
        }
    }

    void generate_statement(runner::program& program, accounter::skeleton::abstraction& abstraction, uint64_t statement_ID, bool& error_occured) {
        // try to generate it as an instruction
        // generate it as a pirate.write_cell(1)(1)
        if (abstraction.p_statements[statement_ID].p_header_ID == 0) {
            // write code
            runner::append__write_cell(program, abstraction.p_literals.p_literals[abstraction.lookup_literal_by_ID(statement_ID, 0, error_occured).p_ID].p_integer_value, calculate_variable_index(abstraction, abstraction.p_statements[statement_ID].p_outputs[0]));
        // generate statement as pirate.copy(1)(1)
        } else if (abstraction.p_statements[statement_ID].p_header_ID == 1) {
            // write code
            runner::append__copy_cell(program, calculate_variable_index(abstraction, abstraction.p_statements[statement_ID].p_inputs[0]), calculate_variable_index(abstraction, abstraction.p_statements[statement_ID].p_outputs[0]));
        // generate statement as pirate.print_cell_as_number(1)(0)
        } else if (abstraction.p_statements[statement_ID].p_header_ID == 2) {
            // write code
            runner::append__print_cell_as_number(program, calculate_variable_index(abstraction, abstraction.p_statements[statement_ID].p_inputs[0]));
        // generate statement as pirate.print_cell_as_character(1)(0)
        } else if (abstraction.p_statements[statement_ID].p_header_ID == 3) {
            // write code
            runner::append__print_cell_as_character(program, calculate_variable_index(abstraction, abstraction.p_statements[statement_ID].p_inputs[0]));
        }
    }

    void generate_abstraction(runner::program& program, accounter::skeleton::abstraction& abstraction, bool& error_occured) {
        uint64_t variable_count;

        // determine variable count
        variable_count = abstraction.p_variables.p_inputs.size() + abstraction.p_variables.p_outputs.size() + abstraction.p_variables.p_variables.size();

        // generate function prologue
        // create context
        runner::append__create_new_context(program, variable_count);

        // get variables in reverse order
        if (abstraction.p_variables.p_inputs.size() > 0) {
            for (int64_t input_ID = abstraction.p_variables.p_inputs.size() - 1; input_ID >= 0; input_ID--) {
                // get input
                runner::append__get_input(program, input_ID);
            }
        }

        // generate function body
        for (uint64_t statement_ID = 0; statement_ID < abstraction.p_statements.size(); statement_ID++) {
            // generate statement
            generate_statement(program, abstraction, statement_ID, error_occured);
        }

        // generate function epilogue
        // pass outputs
        for (uint64_t output_ID = 0; output_ID < abstraction.p_variables.p_outputs.size(); output_ID++) {
            // pass output
            runner::append__pass_output(program, abstraction.p_variables.p_inputs.size() + output_ID);
        }

        // delete context
        runner::append__restore_old_context(program);
    }

    // generate code from a program
    runner::program generate_program(accounter::skeleton::skeleton& skeleton, bool& error_occured) {
        runner::program output;

        // build each abstraction
        for (uint64_t abstraction_ID = 0; abstraction_ID < skeleton.p_abstractions.size(); abstraction_ID++) {
            // check if abstraction has scope
            if (skeleton.p_abstractions[abstraction_ID].p_has_scope) {
                // turn into function
                generate_abstraction(output, skeleton.p_abstractions[abstraction_ID], error_occured);
            }
        }

        // build stopping of program
        runner::append__quit(output);

        return output;
    }
}
