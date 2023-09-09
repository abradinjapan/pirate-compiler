#pragma once

#include "run.cpp"
#include "generate.cpp"

namespace test_runner {
    void test_runner_gen_program(generator::workspace& workspace) {
        // first abstraction
        generator::write_instructions::write__create_new_context(workspace, 5);
        generator::write_instructions::write__write_cell(workspace, (runner::cell)13, 0);
        generator::write_instructions::write__write_cell(workspace, (runner::cell)1234, 1);
        generator::write_instructions::write__write_cell(workspace, (runner::cell)9876, 2);
        generator::write_instructions::write__pass_input(workspace, 1);
        generator::write_instructions::write__pass_input(workspace, 2);
        generator::write_instructions::write__jump_to_abstraction(workspace, 0);
        generator::write_instructions::write__get_output(workspace, 4);
        generator::write_instructions::write__get_output(workspace, 3);
        generator::write_instructions::write__print_cell_as_number(workspace, 3);
        generator::write_instructions::write__print_cell_as_number(workspace, 4);
        generator::write_instructions::write__restore_old_context(workspace);
        generator::write_instructions::write__quit(workspace);
        
        // second abstraction
        generator::write_instructions::write__create_new_context(workspace, 5);
        generator::write_instructions::write__get_input(workspace, 2);
        generator::write_instructions::write__get_input(workspace, 1);
        generator::write_instructions::write__write_cell(workspace, 97, 0);
        generator::write_instructions::write__print_cell_as_character(workspace, 0);
        generator::write_instructions::write__print_cell_as_character(workspace, 0);
        generator::write_instructions::write__print_cell_as_character(workspace, 0);
        generator::write_instructions::write__print_cell_as_character(workspace, 0);
        generator::write_instructions::write__integer_add(workspace, 1, 0, 3);
        generator::write_instructions::write__integer_add(workspace, 2, 0, 4);
        generator::write_instructions::write__pass_output(workspace, 3);
        generator::write_instructions::write__pass_output(workspace, 4);
        generator::write_instructions::write__restore_old_context(workspace);
        generator::write_instructions::write__jump_from_abstraction(workspace);
    }

    void test_runner() {
        generator::workspace workspace;
        bool error_occured = false;

        // start first pass
        workspace.start_pass_measure(1);

        // measure program
        test_runner_gen_program(workspace);

        // finish first pass
        workspace.finish_pass_measure();

        // start second pass
        workspace.start_pass_build();

        // build program
        test_runner_gen_program(workspace);

        // finish second pass
        workspace.finish_pass_build();

        // run program
        runner::run_program(workspace.p_program, error_occured);
    }
}