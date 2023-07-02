#include <iostream>
#include <fstream>

#include "lex.cpp"
#include "parse.cpp"
#include "abstract.cpp"
#include "run.cpp"

std::string load_file(std::string file_path) {
    std::ifstream file(file_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}

void compile(std::string user_code) {
    lexer::lexlings lexlings;
    parser::program parse_tree;
    abstracter::header_table header_table;
    bool lex_error = false;
    bool parse_error = false;
    bool abstraction_error = false;

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
        return;
    }

    // parse file
    parse_tree = parser::parse_file(lexlings, parse_error);

    // print parse tree
    parser::print_program(parse_tree);

    // do not proceed if error occured
    if (parse_error) {
        return;
    }

    // collect headers
    header_table = abstracter::get_header_table(parse_tree, abstraction_error);

    // print headers
    abstracter::print_header_table(header_table);

    // do not proceed if error occured
    if (abstraction_error) {
        return;
    }

    // check if all headers have a match
    std::cout << "All abstraction calls have a matching header: ";
    if (abstracter::verify_all_headers(header_table, parse_tree) == true) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;

        return;
    }

    return;
}

void test_runner() {
    runner::program program;
    bool error_occured = false;

    // build program
    runner::append__create_new_context(program, 3);
    runner::append__write_cell(program, 1002, 0);
    runner::append__write_cell(program, '\n', 1);
    runner::append__print_cell_as_number(program, 0);
    runner::append__print_cell_as_character(program, 1);
    //runner::append__write_cell(program, 3, 2);
    //runner::append__jump_to(program, 2);
    runner::append__restore_old_context(program);
    runner::append__quit(program);

    // run program
    runner::run_program(program, error_occured);
}

int main() {
    std::cout << "☠️ This here infernal contraption be a compiler! ARRGH!☠️ " << std::endl;

    // compile
    compile(load_file("programs/test.pirate"));
    compile(load_file("programs/test2.pirate"));
    compile(load_file("programs/test3.pirate"));

    // test runner
    test_runner();

    return 0;
}
