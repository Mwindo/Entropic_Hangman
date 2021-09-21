Entropic Hangman was tested on an Intel Mac but should run on any POSIX system.

To build, open a shell, cd to this directory, and type make.

To run, cd to this directory and type ./entropic_hangman.

Code is in the src directory; game_menu.c is the entry point. Word lists are in the dicts directory.

The goals for this project:
• Build a simple C terminal application from scratch to practice C fundamentals: string manipulation, file I/O, memory allocation and pointer operations, etc.
• Implement a simple directory structure with a Makefile
• Design and incorporate a simple library
• Write clean code

All of the goals were fulfilled except, in some instances, the last. Certain parts of the code are better (e.g., Levenshteiner or dynamic_string_array) than others (e.g., game_menu or unicode_helper), but the opportunity cost of going through to refine things seemed too high given everything I still want to learn/practice and the relative unimportance of the project. (As Valéry wrote: un ouvrage n’est jamais achevé . . . mais abandonné.) Examples of what I would do if I *did* decide to refine things:
• Refactor the main source code and break it into smaller files. Break longer functions apart. Make sure coding conventions across the project are consistent.
• Include better error handling. Add more comments.
• Create an abstract menu template with centralized input handling.
• Certain functions malloc and return a pointer rather than ingesting a pointer and setting its value (e.g., dependencies/shared/str_funcs.c: to_upper), which is typically better when possible. I would fix this.

Reflections/What I learned:

• On a few occasions, I committed this sloppy, difficult-to-locate error: type * var = malloc(var) (rather than type * var = malloc(*var)). This caused some hours' work of frustration.
• Hofstadter's Testing Law: Testing is more important than you think, even taking this law into account. Although I wrote tests and tested throughout, I "optimized" certain parts of the code too early. Some of my tests should have been more robust early on to detect edge cases from the get-go.
• I used open-source word lists. At least one of them contained weird encoding that I could not for the life of me properly identify let alone decode. I finally realized that the encoding was garbled UTF that still managed to look ok in a text editor. Lesson: vet outside files!
• I have a further appreciation for higher-level languages, even after having worked with C in the past. It took me a couple of days of intermittent work to code essentially the same project in Python.
