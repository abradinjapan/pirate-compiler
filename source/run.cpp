#pragma once

#include <iostream>
#include <vector>
#include <stdint.h>

namespace runner {
    // defines
    typedef uint64_t cell; // register
    typedef uint64_t cell_ID;

    class buffer {
    public:
        std::vector<cell> p_cells;
    };

    class context {
    public:
        buffer p_cells;

        // constructor
        context() {
            p_cells = buffer();
        }

        // constructor
        context(cell_ID cell_count) {
            // initialize blank context
            for (cell_ID i = 0; i < cell_count; i++) {
                // add cell
                p_cells.p_cells.push_back(0);
            }
        }
    };

    enum instruction_type {
        quit,
        write_cell,
        copy_cell,
        print_cell_as_number,
        print_cell_as_character,
        create_new_context,
        restore_old_context,
        clear_inputs,
        clear_outputs,
        pass_input,
        get_input,
        pass_output,
        get_output,
        jump_to,
    };

    class instruction {
    public:
        instruction_type p_type;
        cell p_write_register_value;
        cell_ID p_input_0;
        cell_ID p_output_0;

        instruction() {
            p_type = instruction_type::quit;
            p_write_register_value = 0;
            p_input_0 = 0;
            p_output_0 = 0;
        }
    };

    class offset {
    public:
        int p_instruction_ID;

        offset() {
            p_instruction_ID = 0;
        }

        offset(int instruction_ID) {
            p_instruction_ID = instruction_ID;
        }
    };

    class program {
    public:
        std::vector<instruction> p_instructions;
    };

    class workspace {
    public:
        std::vector<instruction> p_program;
        std::vector<offset> p_offsets;
    };

    // add quit instruction to program
    void append__quit(program& program) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::quit;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add write cell instruction to program
    void append__write_cell(program& program, cell value, cell_ID destination) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::write_cell;
        temp_instruction.p_write_register_value = value;
        temp_instruction.p_output_0 = destination;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add copy cell instruction to program
    void append__copy_cell(program& program, cell_ID source, cell_ID destination) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::copy_cell;
        temp_instruction.p_input_0 = source;
        temp_instruction.p_output_0 = destination;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add print cell as number instruction to program
    void append__print_cell_as_number(program& program, cell_ID source) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::print_cell_as_number;
        temp_instruction.p_input_0 = source;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add print cell as character instruction to program
    void append__print_cell_as_character(program& program, cell_ID source) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::print_cell_as_character;
        temp_instruction.p_input_0 = source;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add create new context instruction to program
    void append__create_new_context(program& program, cell context_cell_count) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::create_new_context;
        temp_instruction.p_write_register_value = context_cell_count;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add restore old context instruction to program
    void append__restore_old_context(program& program) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::restore_old_context;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add clear inputs instruction to program
    void append__clear_inputs(program& program) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::clear_inputs;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add clear outputs instruction to program
    void append__clear_outputs(program& program) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::clear_outputs;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add pass input instruction to program
    void append__pass_input(program& program, cell_ID source) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::pass_input;
        temp_instruction.p_input_0 = source;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add get input instruction to program
    void append__get_input(program& program, cell_ID destination) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::get_input;
        temp_instruction.p_output_0 = destination;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add pass output instruction to program
    void append__pass_output(program& program, cell_ID source) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::pass_output;
        temp_instruction.p_input_0 = source;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add get output instruction to program
    void append__get_output(program& program, cell_ID destination) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::get_output;
        temp_instruction.p_output_0 = destination;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // add jump to instruction to program (jumps to the instruction in the program array specified in 'source')
    void append__jump_to(program& program, cell_ID source) {
        instruction temp_instruction = instruction();

        // setup instruction parameters
        temp_instruction.p_type = instruction_type::jump_to;
        temp_instruction.p_input_0 = source;

        // create new instruction
        program.p_instructions.push_back(temp_instruction);
    }

    // run a program
    buffer run_program(program program, bool& error_occured) {
        buffer output;
        bool running = true;
        int current_instruction = 0;
        std::vector<context> context_stack;
        buffer inputs;
        buffer outputs;

        // process instructions
        while (running == true) {
            // process instruction
            switch (program.p_instructions[current_instruction].p_type) {
            case instruction_type::quit:
                current_instruction++;

                running = false;

                break;
            case instruction_type::write_cell:
                // write data
                context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_output_0] = program.p_instructions[current_instruction].p_write_register_value;

                // next instruction
                current_instruction++;

                break;
            case instruction_type::copy_cell:
                // copy data
                context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_output_0] = context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0];

                // next instruction
                current_instruction++;

                break;
            case instruction_type::print_cell_as_number:
                // print
                std::cout << context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0];

                // next instruction
                current_instruction++;

                break;
            case instruction_type::print_cell_as_character:
                // print
                putchar((char)context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0]);

                // next instruction
                current_instruction++;

                break;
            case instruction_type::create_new_context:
                // push a new context onto the stack
                context_stack.push_back(context(program.p_instructions[current_instruction].p_write_register_value));

                // next instruction
                current_instruction++;

                break;
            case instruction_type::restore_old_context:
                // restore the previous context
                context_stack.pop_back();

                // next instruction
                current_instruction++;

                break;
            case instruction_type::clear_inputs:
                // clear inputs
                inputs.p_cells.clear();

                // next instruction
                current_instruction++;

                break;
            case instruction_type::clear_outputs:
                // clear inputs
                outputs.p_cells.clear();

                // next instruction
                current_instruction++;
                
                break;
            case instruction_type::pass_input:
                // add input
                inputs.p_cells.push_back(context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0]);

                // next instruction
                current_instruction++;

                break;
            case instruction_type::get_input:
                // retrieve input
                context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_output_0] = inputs.p_cells[inputs.p_cells.size() - 1];

                // clear last input
                inputs.p_cells.pop_back();

                // next instruction
                current_instruction++;

                break;
            case instruction_type::pass_output:
                // add output
                outputs.p_cells.push_back(context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0]);

                // next instruction
                current_instruction++;

                break;
            case instruction_type::get_output:
                // retrieve output
                context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_output_0] = outputs.p_cells[outputs.p_cells.size() - 1];

                // clear last output
                outputs.p_cells.pop_back();

                // next instruction
                current_instruction++;

                break;
            case instruction_type::jump_to:
                // jump
                current_instruction = context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0];

                break;
            default:
                std::cout << "Runner Error: Invalid Instruction ID" << std::endl;

                error_occured = true;

                break;
            }
        }

        return output;
    }
}
