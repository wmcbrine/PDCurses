PDCurses for Plan9
==================

This directory contains PDCurses source code files specific to Plan9.


Building
--------

- cd to the plan9 sub directory

- Build it:

        mk
        mk install
        
- Optionally, you can cross compile by setting the variable $objtype,
  for example:

        objtype=amd64 mk

Distribution Status
-------------------

See README.Plan9 for the copyright notice of Federico G. Benavento with
a permissive license.


Acknowledgements
----------------

Federico G. Benavento made the original port to PDCurses 3.0
   <https://comp.os.plan9.narkive.com/YWgPwjjW/9fans-i-did-it-curses-port>

Jens Staal updated it
