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

runner::program compile(std::string user_code) {
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
        return output;
    }

    // parse file
    parse_tree = parser::parse_file(lexlings, parse_error);

    // print parse tree
    parser::print_program(parse_tree);

    // do not proceed if error occured
    if (parse_error) {
        return output;
    }

    // account program
    skeleton.get_skeleton(parse_tree, accounting_error);

    // print program
    skeleton.print_skeleton();

    // do not proceed if error occured
    if (accounting_error) {
        return output;
    }

    // generate program code
    return generator::generate_program(skeleton, generation_error);
}

void test_runner() {
    runner::program program;
    bool error_occured = false;

    // build program
    runner::append__create_new_context(program, 4);
    runner::append__write_cell(program, 1002, 0);
    runner::append__write_cell(program, '\n', 1);
    runner::append__jump_offset(program, 0);
    runner::append__write_cell(program, 1004, 0);
    runner::append__print_cell_as_number(program, 0);
    runner::append__print_cell_as_character(program, 1);
    runner::append__offset(program);
    runner::append__print_cell_as_number(program, 0);
    runner::append__print_cell_as_character(program, 1);
    runner::append__restore_old_context(program);
    runner::append__quit(program);

    // run program
    runner::run_program(program, error_occured);
}

void compile_and_run(std::string user_code) {
    runner::program program;
    bool compilation_error = false;
    bool run_time_error = false;

    // compile program
    program = compile(user_code);

    // run if no errors occured
    if (compilation_error == false) {
        // run code
        runner::run_program(program, run_time_error);
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

    // test runner
    test_runner();

    return 0;
}