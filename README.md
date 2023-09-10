# Pirate Compiler

A pirate themed programming language.

# Pirate Tutorial

The pirate language is very simple and revolves around one concept, abstractions (functions).

Abstractions have multiple inputs and multiple outputs, expressed as a name with two sets of arguments.

> abstraction_name(input_1 input_2)(output_1 output_2) = {  }

And each abstraction body is made up of exclusively three things, instructions, abstraction calls and offsets.

In pirate there are no expressions, no if statements, no for loops, and much less.

The only way to do computational work is by using instructions and abstraction calls.

Instructions and abstraction calls are identical in syntax.

Here is an example function to show the syntax of instructions, comments, offsets and abstractions.

> [ this is a comment ]
> abstraction(input.1)(output.1) = \{
>     [ instruction ]
>     instruction(blah)(blah)
>
>     [ abstraction call ]
>     abstraction_call(blah)(blah)
>
>     [ offset ]
>     @offset.name
> \}

The main function in pirate language is a function call pirate.sail()().

> pirate.sail()() = {  }
