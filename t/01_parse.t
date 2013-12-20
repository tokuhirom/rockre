use strict;
use warnings;
use Test::More;
use Test::Base::Less;
use t::Util;

for my $block (blocks) {
    is(run_rockre($block->input), $block->expected, $block->input);
}

done_testing;

__END__

===
--- input: a
--- expected: (string "a")

===
--- input: ^^
--- expected: (head)

===
--- input: $$
--- expected: (tail)

===
--- input: ^^
--- expected: (head)

===
--- input: ^^aaa
--- expected: (list (head) (string "aaa"))

=== Ignore space.
--- input: ^^ aaa
--- expected: (list (head) (string "aaa"))

===
--- input: aaa|bbb
--- expected: (or (string "aaa") (string "bbb"))

===
--- input: a|b|c
--- expected: (or (string "a") (or (string "b") (string "c")))

===
--- input: ( aaa | bbb )
--- expected: (capture (or (string "aaa") (string "bbb")))

===
--- input: [ aaa | bbb ]
--- expected: (group (or (string "aaa") (string "bbb")))

===
--- input: a. 
--- expected: (list (string "a") (anychar))

===
--- input: a\.
--- expected: (string "a.")

===
--- input: a\t
--- expected: (string "a	")

===
--- input: あいう
--- expected: (string "あいう")

