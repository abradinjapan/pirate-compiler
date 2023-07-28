#pragma once

#include <vector>

#include "account.cpp"
#include "run.cpp"

namespace generator {
    class offset {
    public:
        uint64_t p_instruction_ID;

        offset() {
            p_instruction_ID = 0;
        }

        offset(int instruction_ID) {
            p_instruction_ID = instruction_ID;
        }
    };

    class offsets {
    public:
        offset p_start;
        std::vector<offset> p_code_defined_offsets;
    };

    enum pass_type {
        pass_measure,
        pass_build,
    };

    class workspace {
    public:
        runner::program p_program;
        std::vector<offsets> p_abstraction_offsets;
        uint64_t p_instruction_count;
        pass_type p_pass_type;

        void start_pass_measure(uint64_t abstraction_count) {
            // DEBUG
            std::cout << "Starting pass measure, abstraction count: " << abstraction_count << std::endl;

            p_abstraction_offsets.resize(abstraction_count);
            p_pass_type = pass_type::pass_measure;
            p_instruction_count = 0;
        }

        void finish_pass_measure() {
            // DEBUG
            std::cout << "Finishing pass measure." << std::endl;

            p_program.p_instructions.resize(p_instruction_count);
        }

        void start_pass_build() {
            // DEBUG
            std::cout << "Starting pass build." << std::endl;

            p_instruction_count = 0;
            p_pass_type = pass_type::pass_build;
        }

        uint64_t get_offset() {
            return p_instruction_count;
        }
    };

    namespace write_instructions {
        void write__quit(workspace& workspace) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::quit;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__write_cell(workspace& workspace, runner::cell value, runner::cell_ID destination) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::write_cell;
                temp_instruction.p_write_register_value = value;
                temp_instruction.p_output_0 = destination;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__copy_cell(workspace& workspace, runner::cell_ID source, runner::cell_ID destination) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::copy_cell;
                temp_instruction.p_input_0 = source;
                temp_instruction.p_output_0 = destination;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__print_cell_as_number(workspace& workspace, runner::cell_ID source) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::print_cell_as_number;
                temp_instruction.p_input_0 = source;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__print_cell_as_character(workspace& workspace, runner::cell_ID source) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::print_cell_as_character;
                temp_instruction.p_input_0 = source;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__create_new_context(workspace& workspace, runner::cell cell_count) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::create_new_context;
                temp_instruction.p_write_register_value = cell_count;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__restore_old_context(workspace& workspace) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::restore_old_context;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }

        void write__jump_to(workspace& workspace, runner::cell_ID source) {
            runner::instruction temp_instruction;

            // create instruction
            if (workspace.p_pass_type == pass_type::pass_build) {
                // set type
                temp_instruction.p_type = runner::instruction_type::jump_to;
                temp_instruction.p_input_0 = source;

                // write instruction
                workspace.p_program.p_instructions[workspace.p_instruction_count] = temp_instruction;
            }

            // next instruction
            workspace.p_instruction_count++;
        }
    }

    void generate_abstraction(workspace& workspace, accounter::skeleton::abstraction& abstraction, uint64_t abstraction_ID, bool& error_occured) {
        uint64_t variable_count;

        // determine variable count
        variable_count = abstraction.p_variables.p_variables.size();

        // setup offsets container
        if (workspace.p_pass_type == pass_type::pass_measure) {
            workspace.p_abstraction_offsets[abstraction_ID].p_code_defined_offsets.resize(abstraction.p_offsets.p_offsets.size());
        
            // setup function start offset
            workspace.p_abstraction_offsets[abstraction_ID].p_start = workspace.get_offset();
        // DEBUG
        } else {
            std::cout << "Offset Count: " << workspace.p_abstraction_offsets[abstraction_ID].p_code_defined_offsets.size() << std::endl;
        }

        // generate function prologue
        // create context
        write_instructions::write__create_new_context(workspace, variable_count);

        // generate function body
        for (uint64_t statement_ID = 0; statement_ID < abstraction.p_statement_map.size(); statement_ID++) {
            // generate statement
            if (abstraction.p_statement_map[statement_ID].p_type == accounter::skeleton::statement_type::is_call_statement) {
                // write code (NOTE: make sure that the cases are aligned with their respective run.cpp instructions!)
                switch (abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_header_ID) {
                // pirate.quit(0)(0)
                case runner::instruction_type::quit:
                    // write code
                    write_instructions::write__quit(workspace);

                    break;
                // pirate.write_cell(1)(1)
                case runner::instruction_type::write_cell:
                    // determine type of input
                    if (abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_type == accounter::skeleton::argument_type::is_integer_literal) {
                        // constant is an integer literal, write code
                        write_instructions::write__write_cell(workspace, abstraction.p_literals.p_literals[abstraction.lookup_literal_by_ID(statement_ID, 0, error_occured).p_ID].p_integer_value, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_outputs[0].p_ID);
                    } else if (abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_type == accounter::skeleton::argument_type::is_offset) {
                        // if pass is measure
                        if (workspace.p_pass_type == pass_type::pass_measure) {
                            // write dummy instruction
                            write_instructions::write__write_cell(workspace, 0, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_outputs[0].p_ID);
                        // if pass is build
                        } else {
                            // DEBUG
                            std::cout << "Offset ID #" << abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_ID << std::endl;

                            // constant is an offset, write code
                            write_instructions::write__write_cell(workspace, workspace.p_abstraction_offsets[abstraction_ID].p_code_defined_offsets[abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_ID].p_instruction_ID, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_outputs[0].p_ID);
                        }
                    }

                    break;
                // pirate.copy(1)(1)
                case runner::instruction_type::copy_cell:
                    // write code
                    write_instructions::write__copy_cell(workspace, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_ID, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_outputs[0].p_ID);

                    break;
                // pirate.print_cell_as_number(1)(0)
                case runner::instruction_type::print_cell_as_number:
                    // write code
                    write_instructions::write__print_cell_as_number(workspace, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_ID);

                    break;
                // pirate.print_cell_as_character(1)(0)
                case runner::instruction_type::print_cell_as_character:
                    // write code
                    write_instructions::write__print_cell_as_character(workspace, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_ID);

                    break;
                // pirate.jump_to(1)(0)
                case runner::instruction_type::jump_to:
                    // write code
                    write_instructions::write__jump_to(workspace, abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_inputs[0].p_ID);

                    break;
                // user defined statement call
                default:
                    // DEBUG
                    std::cout << "Temporary Error: Generation error, user code defined calls are not implemented: [ " << abstraction.p_calls[abstraction.p_statement_map[statement_ID].p_ID].p_header_ID << " ] statement_ID: " << statement_ID << std::endl;

                    break;
                }
            } else if (abstraction.p_statement_map[statement_ID].p_type == accounter::skeleton::statement_type::is_offset_statement) {
                // define offset
                if (workspace.p_pass_type == pass_type::pass_measure) {
                    // DEBUG
                    std::cout << "Defined abstraction offset #" << abstraction.p_statement_map[statement_ID].p_ID << " at instruction #" << workspace.get_offset() << std::endl;

                    // write offset
                    workspace.p_abstraction_offsets[abstraction_ID].p_code_defined_offsets[abstraction.p_statement_map[statement_ID].p_ID].p_instruction_ID = workspace.get_offset();
                }
            }
        }

        // delete context
        write_instructions::write__restore_old_context(workspace);
    }

    // generate code from a program
    runner::program generate_program(accounter::skeleton::skeleton& skeleton, bool& error_occured) {
        workspace workspace;

        // start pass
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

        // finish pass
        workspace.finish_pass_measure();

        // start pass
        workspace.start_pass_build();

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

        return workspace.p_program;
    }
}
