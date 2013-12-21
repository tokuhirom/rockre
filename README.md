RockRE
======

Perl6 compatible regular expresssion engine in C++

## Supported syntax

    a|b   alternative
    .     anychar
    (a)   capture
    [a|b] grouping
    ^     string head
    $     string tail

## Unsupported (yet) syntax

    *
    +
    ?
    *?
    +?
    ??
    'quoted'
    .**2
    ^^        line head
    $$        line tail

## See also

  * [S05](https://raw.github.com/perl6/specs/master/S05-regex.pod)

