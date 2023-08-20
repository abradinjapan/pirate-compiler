#include <iostream>
#include <fstream>

#include "lex.cpp"
#include "parse.cpp"
#include "account.cpp"
#include "generate.cpp"
#include "run.cpp"

std::string load_file(std::string file_path) {
    std::ifstream file(file_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}

runner::program compile(std::string user_code, bool& compilation_error) {
    runner::program output;
    lexer::lexlings lexlings;
    parser::program parse_tree;
    accounter::skeleton::skeleton skeleton;
    bool lex_error = false;
    bool parse_error = false;
    bool accounting_error = false;
    bool generation_error = false;

    // print original file
    std::cout << "Original File:" << std::endl << user_code << std::endl;

    // lex file
    lexlings = lexer::lex(user_code, lex_error);

    // print lexlings
    std::cout << "Lexlings:" << std::endl;
    for (unsigned int i = 0; i < lexlings.p_lexlings.size(); i++) {
        std::cout << "\tType: " << lexlings.p_lexlings[i].p_type << " Value: " << lexlings.p_lexlings[i].p_value << std::endl;
    }

    // do not proceed if error occured
    if (lex_error) {
        compilation_error = true;

        return output;
    }

    // parse file
    parse_tree = parser::parse_file(lexlings, parse_error);

    // print parse tree
    parser::print_program(parse_tree);

    // do not proceed if error occured
    if (parse_error) {
        compilation_error = true;

        return output;
    }

    // account program
    skeleton.get_skeleton(parse_tree, accounting_error);

    // print program
    skeleton.print_skeleton();

    // do not proceed if error occured
    if (accounting_error) {
        compilation_error = true;

        return output;
    }

    // generate program code
    output = generator::generate_program(skeleton, generation_error);

    // print code
    print_program(output);
    
    // do not proceed if error occured
    if (generation_error) {
        compilation_error = true;
        
        return output;
    }

    return output;
}

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

void compile_and_run(std::string user_code) {
    runner::program program;
    bool compilation_error = false;
    bool run_time_error = false;

    // compile program
    program = compile(user_code, compilation_error);

    // run if no errors occured
    if (compilation_error == false) {
        // inform user of program start
        std::cout << "Starting program..." << std::endl;

        // run code
        runner::run_program(program, run_time_error);
    } else {
        std::cout << "Code not run, compilation error." << std::endl;
    }
}

int main() {
    std::cout << "☠️ This here infernal contraption be a compiler! ARRGH!☠️ " << std::endl;

    // compile
    //compile(load_file("programs/test.pirate"));
    //compile(load_file("programs/test2.pirate"));
    //compile(load_file("programs/test3.pirate"));
    //compile(load_file("programs/test4.pirate"));
    compile_and_run(load_file("programs/test5.pirate"));
    //compile_and_run(load_file("programs/test6.pirate"));
    compile_and_run(load_file("programs/test7.pirate"));

    // test runner
    test_runner();

    return 0;
}
