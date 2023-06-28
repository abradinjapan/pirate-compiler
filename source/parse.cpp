#pragma once

#include <iostream>
#include "lex.cpp"

namespace parser {
    enum name_type {
        is_uninitialized,
        is_value_name, // is the same for variables and numeral literals
        is_abstraction_name,
        is_offset,
    };

    class name {
    public:
        name_type p_name_type;
        std::string p_name_value;

        // default constructor
        name() {
            p_name_type = name_type::is_uninitialized;
            p_name_value = "";
        }

        // explicit constructor
        name(name_type type, std::string value) {
            p_name_type = type;
            p_name_value = value;
        }
    };

    class statement {
    public:
        name p_name;
        std::vector<name> p_inputs;
        std::vector<name> p_outputs;

        // default constructor
        statement() {
            p_name = name();
            p_inputs = std::vector<name>();
            p_outputs = std::vector<name>();
        }

        // initialize statement except for io
        statement(name name_value) {
            p_name = name_value;
        }

        // initialize statement as abstraction call
        statement(name name_value, std::vector<name> inputs, std::vector<name> outputs) {
            p_name = name_value;
            p_inputs = inputs;
            p_outputs = outputs;
        }
    };

    class abstraction {
    public:
        statement p_header;
        std::vector<statement> p_scope;

        abstraction() {
            p_header = statement();
            p_scope = std::vector<statement>();
        }
    };

    class program {
    public:
        std::vector<abstraction> p_abstractions;
    };

    // parse arguments
    std::vector<name> parse_arguments(lexer::lexlings& lexlings, int& lexling_index, bool& error_occured) {
        std::vector<name> output;

        // check for opener
        if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::left_parenthesis) {
            // next lexling
            lexling_index++;

            // get arguments until end of arguments
            while (lexling_index < lexlings.count()) {
                // get named arguments
                if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::name) {
                    // add argument
                    output.push_back(name(name_type::is_value_name, lexlings.p_lexlings[lexling_index].p_value));

                    // next lexling
                    lexling_index++;
                } else if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::offset_marker && lexlings.p_lexlings[lexling_index + 1].p_type == lexer::type::name) {
                    // add argument
                    output.push_back(name(name_type::is_offset, lexlings.p_lexlings[lexling_index + 1].p_value));

                    // next lexling
                    lexling_index += 2;
                // invalid argument lexling, quit loop
                } else {
                    break;
                }
            }

            // skip past ending parenthesis
            if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::right_parenthesis) {
                lexling_index++;
            } else {
                // display error
                std::cout << "Parse Error: Argument Missing Closing Parethesis." << std::endl;

                error_occured = true;
            }
        } else {
            // display error
            std::cout << "Parse Error: Argument Missing Opening Parethesis." << std::endl;

            error_occured = true;
        }

        return output;
    }

    // parse statement
    statement parse_statement(lexer::lexlings& lexlings, int& lexling_index, bool& error_occured) {
        statement output;

        // get abstraction call
        if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::name) {
            // assign name
            output.p_name = name(name_type::is_abstraction_name, lexlings.p_lexlings[lexling_index].p_value);
            lexling_index++;

            // get inputs
            output.p_inputs = parse_arguments(lexlings, lexling_index, error_occured);

            // check error
            if (error_occured) {
                return output;
            }

            // get outputs
            output.p_outputs = parse_arguments(lexlings, lexling_index, error_occured);

            // check error
            if (error_occured) {
                return output;
            }
        // get offset marker
        } else if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::offset_marker && lexlings.p_lexlings[lexling_index + 1].p_type == lexer::type::name) {
            // get offset name
            output.p_name = name(name_type::is_offset, lexlings.p_lexlings[lexling_index + 1].p_value);

            // next lexling
            lexling_index += 2;
        // error
        } else {
            // display error
            std::cout << "Parse Error: Unrecognized Statement Type." << std::endl << "Current Lexling: " << lexlings.p_lexlings[lexling_index].p_value << std::endl;

            // set error
            error_occured = true;
        }

        return output;
    }

    abstraction parse_abstraction(lexer::lexlings& lexlings, int& lexling_index, bool& error_occured) {
        abstraction output;

        // get abstraction name
        output.p_header = parse_statement(lexlings, lexling_index, error_occured);

        // get scope
        if (lexlings.p_lexlings[lexling_index].p_type == lexer::type::left_curly_bracket) {
            // next scope
            lexling_index++;

            // get all statements
            while (lexlings.p_lexlings[lexling_index].p_type != lexer::type::right_curly_bracket) {
                // parse statement
                output.p_scope.push_back(parse_statement(lexlings, lexling_index, error_occured));

                // check error
                if (error_occured) {
                    return output;
                }
            }

            // skip past scope end
            lexling_index++;
        } else {
            // print error
            std::cout << "Parse Error: Scope Opener '{' Missing." << std::endl;

            // set error
            error_occured = true;
        }

        return output;
    }

    // parse file
    program parse_file(lexer::lexlings lexlings, bool& error_occured) {
        program output = program();
        int lexling_index = 0;

        // get abstractions
        while (lexling_index < lexlings.count()) {
            // new abstraction
            output.p_abstractions.push_back(parse_abstraction(lexlings, lexling_index, error_occured));

            // check error
            if (error_occured) {
                return output;
            }
        }

        return output;
    }

    // print arguments
    void print_inputs(int tab_depth, std::vector<name>& arguments) {
        // print arguments
        for (uint64_t i = 0; i < arguments.size(); i++) {
            // print tabs
            for (uint64_t  i = 0; i < tab_depth; i++) {
                std::cout << "\t";
            }

            // print starter
            std::cout << "Input[ " << i << " ] : ";

            // print argument type
            switch (arguments[i].p_name_type) {
            case name_type::is_value_name:
                std::cout << "value_name";
                break;
            case name_type::is_abstraction_name:
                std::cout << "abstraction_name";
                break;
            case name_type::is_offset:
                std::cout << "offset_name";
                break;
            default:
                std::cout << "UNINITIALIZED_name";
                break;
            }

            // print the rest
            std::cout << " : " << arguments[i].p_name_value << std::endl;
        }

        return;
    }

    // print arguments
    void print_outputs(int tab_depth, std::vector<name>& arguments) {
        // print arguments
        for (uint64_t i = 0; i < arguments.size(); i++) {
            // print tabs
            for (uint64_t  i = 0; i < tab_depth; i++) {
                std::cout << "\t";
            }

            // print starter
            std::cout << "Output[ " << i << " ] : ";

            // print argument type
            switch (arguments[i].p_name_type) {
            case name_type::is_value_name:
                std::cout << "value_name";
                break;
            case name_type::is_abstraction_name:
                std::cout << "abstraction_name";
                break;
            case name_type::is_offset:
                std::cout << "offset_name";
                break;
            default:
                std::cout << "UNINITIALIZED_name";
                break;
            }

            // print the rest
            std::cout << " : " << arguments[i].p_name_value << std::endl;
        }

        return;
    }

    // print program
    void print_program(program program) {
        // print header
        std::cout << "Parse Tree:" << std::endl;

        // print all abstractions
        for (unsigned int abstraction = 0; abstraction < program.p_abstractions.size(); abstraction++) {
            // print abstraction name
            std::cout << "Abstraction Name: " << program.p_abstractions[abstraction].p_header.p_name.p_name_value << std::endl;

            // print abstraction io
            print_inputs(1, program.p_abstractions[abstraction].p_header.p_inputs);
            print_outputs(1, program.p_abstractions[abstraction].p_header.p_outputs);

            // print statements header
            std::cout << "\tStatements:" << std::endl;

            // print abstraction statements
            for (unsigned int statement = 0; statement < program.p_abstractions[abstraction].p_scope.size(); statement++) {
                // print statments by type
                if (program.p_abstractions[abstraction].p_scope[statement].p_name.p_name_type == name_type::is_offset) {
                    // print offset
                    std::cout << "\t\tOffset Name: @" << program.p_abstractions[abstraction].p_scope[statement].p_name.p_name_value << std::endl;
                } else {
                    // print name
                    std::cout << "\t\tStatement Name: " << program.p_abstractions[abstraction].p_scope[statement].p_name.p_name_value << std::endl;

                    // print io
                    print_inputs(3, program.p_abstractions[abstraction].p_scope[statement].p_inputs);
                    print_inputs(3, program.p_abstractions[abstraction].p_scope[statement].p_outputs);
                }
            }
        }

        return;
    }
}
