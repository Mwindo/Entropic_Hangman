/*
what this should do:
on game-dict.needs_reloading = true at the start of a new round (or on a more specific check, like new language added):
• read all (or the just-added) dict files line by line
• make sure no spaces, blank lines; if so, eliminate
• make sure only ascii characters or characters in the unicode_helper dict; if not, return error
• not hard to implement, but I'd rather work on something else for now
*/