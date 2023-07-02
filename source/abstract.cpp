#pragma once

/* Notes */
/*
    The goal of this file is to filter out all abstraction definitions and abstraction calls.
    The code must contain one unique abstraction header for each abstraction.
*/

#include "parse.cpp"

namespace abstracter {
    class header {
    public:
        std::string p_name;
        int p_input_count;
        int p_output_count;

        header() {
            p_name = "";
            p_input_count = 0;
            p_output_count = 0;
        }

        header(std::string name, int input_count, int output_count) {
            p_name = name;
            p_input_count = input_count;
            p_output_count = output_count;
        }
    };

    // check if two headers have matching signatures
    bool match_headers(header a, header b) {
        return (a.p_name == b.p_name) && (a.p_input_count == b.p_input_count) && (a.p_output_count == b.p_output_count);
    }

    // a type that keeps track of all header declarations
    class header_table {
    public:
        std::vector<header> p_headers;
        
        // check to see if a header is registered
        bool header_registered(header header) {
            // check each header
            for (uint64_t i = 0; i < p_headers.size(); i++) {
                // check header for match
                if (match_headers(p_headers[i], header) == true) {
                    // match found
                    return true;
                }
            }

            // no matches found
            return false;
        }

        // attempt to register a new header, false means a duplicate was found, true means registration was successful
        bool try_register_header(header header) {
            // check to see if header already exists, if so, reject
            if (header_registered(header) == true) {
                // header could not be added
                return false;
            }

            // header was not already registered, add new header
            p_headers.push_back(header);

            // header added successfully
            return true;
        }
    };

    header_table get_header_table(parser::program& program, bool& error_occured) {
        header_table output;

        // add abstractions to header table
        for (uint64_t i = 0; i < program.p_abstractions.size(); i++) {
            // try to add a header
            if (output.try_register_header(header(program.p_abstractions[i].p_header.p_name.p_name_value, program.p_abstractions[i].p_header.p_inputs.size(), program.p_abstractions[i].p_header.p_outputs.size())) == false) {
                // error, header re-registered
                std::cout << "Abstracting Error: Duplicate Header Found : " << program.p_abstractions[i].p_header.p_name.p_name_value << std::endl;

                // set error
                error_occured = true;

                return output;
            }
        }

        return output;
    }

    void print_header_table(header_table table) {
        std::cout << "Header Table:" << std::endl;

        // print each header
        for (uint64_t i = 0; i < table.p_headers.size(); i++) {
            // print header
            std::cout << "\tHeader: " << table.p_headers[i].p_name << "(" << table.p_headers[i].p_input_count << ")(" << table.p_headers[i].p_output_count << ")" << std::endl;
        }

        // new line
        std::cout << std::endl;
    }

    // verifies that all statements match a header
    bool verify_all_headers(header_table header_table, parser::program& program) {
        // for each abstraction
        for (uint64_t i = 0; i < program.p_abstractions.size(); i++) {
            // for each statement in each abstraction
            for (uint64_t j = 0; j < program.p_abstractions[i].p_scope.size(); j++) {
                // check header
                if (header_table.header_registered(header(program.p_abstractions[i].p_scope[j].p_name.p_name_value, program.p_abstractions[i].p_scope[j].p_inputs.size(), program.p_abstractions[i].p_scope[j].p_outputs.size())) == false) {
                    // header not registered, error
                    return false;
                }
            }
        }

        // all headers matched correctly
        return true;
    }
}
