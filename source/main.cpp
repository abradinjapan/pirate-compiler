#include <iostream>
#include <fstream>

#include "lex.cpp"
#include "parse.cpp"
#include "account.cpp"
#include "generate.cpp"
#include "run.cpp"
#include "test-runner.cpp"

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

    // do not proceed if error occured
    if (generation_error) {
        compilation_error = true;
        
        return output;
    }

    // print code
    print_program(output);

    return output;
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
    compile_and_run(load_file("programs/test7.pirate"));
    compile_and_run(load_file("programs/test8.pirate"));
    compile_and_run(load_file("programs/test9.pirate"));
    compile_and_run(load_file("programs/test10.pirate"));
    compile_and_run(load_file("programs/memory.pirate"));

    // test runner
    //test_runner::test_runner();

    return 0;
}
