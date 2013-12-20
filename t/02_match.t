use strict;
use warnings;
use Test::More;
use Test::Base::Less;
use t::Util;

for my $block (blocks) {
    my $got = run_rockre('-r', $block->re, $block->str);
    $got =~ s/\n\z//;
    is($got, $block->expected, sprintf(qq{/%s/ =~ "%s"}, $block->re, $block->str));
}

done_testing;

__END__

===
--- re:  a
--- str: a
--- expected: OK

===
--- re:  aa
--- str: aa
--- expected: OK

===
--- re:  bc
--- str: abc
--- expected: OK

===
--- re:  a
--- str: b
--- expected: FAIL

===
--- re:  ^^ a
--- str: a
--- expected: OK

===
--- re:  ^^ a
--- str: ba
--- expected: FAIL

===
--- re:  a $$
--- str: ba
--- expected: OK

===
--- re:  a $$
--- str: bao
--- expected: FAIL

===
--- SKIP
--- re:  a | b
--- str: a
--- expected: OK
