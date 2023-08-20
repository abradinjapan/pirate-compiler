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
        jump_to_abstraction,
        jump_from_abstraction,
        jump_to,
        get_instruction_index,
        integer_add,
    };

    class instruction {
    public:
        instruction_type p_type;
        cell p_write_register_value;
        cell_ID p_input_0;
        cell_ID p_input_1;
        cell_ID p_output_0;

        instruction() {
            p_type = instruction_type::quit;
            p_write_register_value = 0;
            p_input_0 = 0;
            p_input_1 = 0;
            p_output_0 = 0;
        }
    };

    class program {
    public:
        std::vector<instruction> p_instructions;
    };

    // run a program
    buffer run_program(program program, bool& error_occured) {
        buffer output;
        bool running = true;
        int current_instruction = 0;
        std::vector<context> context_stack;
        std::vector<int> return_stack;
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
            case instruction_type::jump_to_abstraction:
                // push the instruction to be returned
                return_stack.push_back(current_instruction + 1);

                // jump
                current_instruction = context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0];

                break;
            case instruction_type::jump_from_abstraction:
                // jump
                current_instruction = return_stack[return_stack.size() - 1];
                return_stack.pop_back();

                break;
            case instruction_type::jump_to:
                // jump
                current_instruction = context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0];

                break;
            case instruction_type::get_instruction_index:
                // perform addition
                context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_output_0] = (cell)current_instruction;

                // next instruction
                current_instruction++;

                break;
            case instruction_type::integer_add:
                // perform addition
                context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_output_0] = context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_0] + context_stack[context_stack.size() - 1].p_cells.p_cells[program.p_instructions[current_instruction].p_input_1];

                // next instruction
                current_instruction++;

                break;
            default:
                std::cout << "Runner Error: Invalid Instruction ID" << std::endl;

                error_occured = true;

                break;
            }
        }

        return output;
    }

    // print program
    void print_program(program& program) {
        // print header
        std::cout << "Instructions:" << std::endl;

        // print instructions
        for (uint64_t i = 0; i < program.p_instructions.size(); i++) {
            // print instruction
            std::cout << "\t" << program.p_instructions[i].p_type << std::endl;
        }

        return;
    }
}
