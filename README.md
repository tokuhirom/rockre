RockRE
======

Perl6 compatible regular expresssion engine in C++

## Supported syntax

    a||b   alternative
    .     anychar
    (a)   capture
    [a||b] grouping
    ^     string head
    $     string tail
    ?     one or zero
    *     one or more
    +
    ^^        line head
    $$        line tail
    *?        zero or more(non greedy)
    +?        one or more(non greedy)
    ??        one or zero(non greedy)
    <[ ]>     character class

## Unsupported (yet) syntax

    'quoted'
    .**2
    \d
    \s
    \S
    \N

    a | b     longest match

## Implementation details

Current version of RockRE supports naive NFA engine.

## HACKING TIPS

Run tests with verbose output.

    make test ARGS="-V"

## See also

  * [S05](https://raw.github.com/perl6/specs/master/S05-regex.pod)

