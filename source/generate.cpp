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

    void generate_abstraction(runner::workspace& workspace, accounter::skeleton::abstraction& abstraction, uint64_t abstraction_ID, bool& error_occured) {
        uint64_t variable_count;
        uint64_t call_index;

        // determine variable count
        variable_count = abstraction.p_variables.p_inputs.size() + abstraction.p_variables.p_outputs.size() + abstraction.p_variables.p_variables.size() + 1/* Return Instruction Variable */;

        // generate function prologue
        // create function start offset
        if (workspace.p_pass_type == runner::pass_type::pass_measure) {
            // setup abstraction container
            workspace.p_abstraction_offsets.push_back(runner::offsets());

            // setup offset
            workspace.p_abstraction_offsets[abstraction_ID].p_start = workspace.get_offset();
        }

        // create context
        runner::append__create_new_context(workspace, variable_count);

        // get variables in reverse order
        if (abstraction.p_variables.p_inputs.size() > 0) {
            for (int64_t input_ID = abstraction.p_variables.p_inputs.size() - 1; input_ID >= 0; input_ID--) {
                // get input
                runner::append__get_input(workspace, input_ID);
            }
        }

        // set call index
        call_index = 0;

        // generate function body
        for (uint64_t statement_ID = 0; statement_ID < abstraction.p_statement_map.size(); statement_ID++) {
            // generate statement
            if (abstraction.p_statement_map[statement_ID].p_type == accounter::skeleton::statement_type::is_call_statement) {
                // write code (NOTE: make sure that the cases are aligned with their respective run.cpp instructions!)
                switch (abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_header_ID) {
                // pirate.quit(0)(0)
                case runner::instruction_type::quit:
                    // write code
                    runner::append__quit(workspace);

                    break;
                // pirate.write_cell(1)(1)
                case runner::instruction_type::write_cell:
                    // determine type of input
                    if (abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_type == accounter::skeleton::argument_type::is_integer_literal) {
                        // constant is a integer literal, write code
                        runner::append__write_cell(workspace, abstraction.p_literals.p_literals[abstraction.lookup_literal_by_ID(statement_ID, 0, error_occured).p_ID].p_integer_value, calculate_variable_index(abstraction, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_outputs[0]));
                    }

                    break;
                // pirate.copy(1)(1)
                case runner::instruction_type::copy_cell:
                    // write code
                    runner::append__copy_cell(workspace, calculate_variable_index(abstraction, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0]), calculate_variable_index(abstraction, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_outputs[0]));

                    break;
                // pirate.print_cell_as_number(1)(0)
                case runner::instruction_type::print_cell_as_number:
                    // write code
                    runner::append__print_cell_as_number(workspace, calculate_variable_index(abstraction, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0]));

                    break;
                // pirate.print_cell_as_character(1)(0)
                case runner::instruction_type::print_cell_as_character:
                    // write code
                    runner::append__print_cell_as_character(workspace, calculate_variable_index(abstraction, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0]));

                    break;
                // user defined statement call
                default:
                    // DEBUG
                    std::cout << "Temporary Error: Generation error, call not defined: [ " << abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_header_ID << " ] statement_ID: " << statement_ID << std::endl;

                    break;
                }

                // next call index
                call_index++;
            } else if (abstraction.p_statement_map[statement_ID].p_type == accounter::skeleton::statement_type::is_offset_statement) {
                // define offset
                if (workspace.p_pass_type == runner::pass_type::pass_measure) {
                    //workspace.p_abstraction_offsets[abstraction_ID].p_code_defined_offsets.push_back(workspace.get_offset());
                }
            }
        }

        // generate function epilogue
        // create function return offset
        if (workspace.p_pass_type == runner::pass_type::pass_measure) {
            workspace.p_abstraction_offsets[abstraction_ID].p_end = workspace.get_offset();
        }

        // pass outputs
        for (uint64_t output_ID = 0; output_ID < abstraction.p_variables.p_outputs.size(); output_ID++) {
            // pass output
            runner::append__pass_output(workspace, abstraction.p_variables.p_inputs.size() + output_ID);
        }

        // delete context
        runner::append__restore_old_context(workspace);
    }

    // generate code from a program
    runner::program generate_program(accounter::skeleton::skeleton& skeleton, bool& error_occured) {
        runner::workspace workspace;

        // start the measuring pass
        workspace.start_pass_measure(skeleton.p_abstractions.size());

        // measure each abstraction
        for (uint64_t abstraction_ID = 0; abstraction_ID < skeleton.p_abstractions.size(); abstraction_ID++) {
            // check if abstraction has scope
            if (skeleton.p_abstractions[abstraction_ID].p_has_scope) {
                // turn into function
                generate_abstraction(workspace, skeleton.p_abstractions[abstraction_ID], abstraction_ID, error_occured);

                // check error
                if (error_occured) {
                    return workspace.p_program;
                }
            }
        }

        // build stopping of program
        runner::append__quit(workspace);

        return workspace.p_program;
    }
}
