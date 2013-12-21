use strict;
use warnings;
use Test::More;
use Test::Base::Less;
use t::Util;

for my $block (blocks) {
    my $got = run_rockre('-p', $block->re, $block->str);
    $got =~ s/\n\z//;
    my $expected = $block->expected;
    $expected =~ s/\n+\z//;
    is($got, $expected, sprintf(qq{/%s/ =~ "%s"}, $block->re, $block->str));
}

done_testing;

__END__

===
--- re:  a(..)d(...)
--- str: abcdefg
--- expected
OK
0:bc
1:efg

===
--- re:  a(.)u(.)o
--- str: aいuえo
--- expected
OK
0:い
1:え
