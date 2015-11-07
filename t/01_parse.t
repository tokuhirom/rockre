use strict;
use warnings;
use Test::More;
use t::Util;
use JKML;

for my $block (@{decode_jkml(do { local $/; <DATA> })}) {
    is(run_rockre($block->{input}), $block->{expected}, $block->{input});
}

done_testing;

my $n = <<'...';


===
--- input: "あいう"
--- expected: (quote "あいう")

===
--- input: hoa?
--- expected: (quest (string "a"))
...

__DATA__
[
  {
    input => 'a',
    expected => "(char 'a')",
  },
  {
    input => '^^',
    expected => "(linehead)",
  },
  {
    input => '$$',
    expected => "(linetail)",
  },
  {
    input => '^',
    expected => "(head)",
  },
  {
    input => '$',
    expected => "(tail)",
  },
  {
    input => '^^aaa',
    expected => "(list (linehead) (char 'a') (char 'a') (char 'a'))",
  },
  {
    # ignore space
    input => '^^  aaa',
    expected => "(list (linehead) (char 'a') (char 'a') (char 'a'))",
  },
  {
    input => 'aaa||bbb',
    expected => "(alt (list (char 'a') (char 'a') (char 'a')) (list (char 'b') (char 'b') (char 'b')))",
  },
  {
    input => 'a||b||c',
    expected => "(alt (char 'a') (alt (char 'b') (char 'c')))",
  },
  {
    input => '( aaa || bbb )',
    expected => "(capture (alt (list (char 'a') (char 'a') (char 'a')) (list (char 'b') (char 'b') (char 'b'))))",
  },
  {
    input => '[ aaa || bbb ]',
    expected => "(group (alt (list (char 'a') (char 'a') (char 'a')) (list (char 'b') (char 'b') (char 'b'))))",
  },
  {
    input => 'a.',
    expected => "(list (char 'a') (anychar))",
  },
  {
    input => r'a\.',
    expected => "(list (char 'a') (char '.'))",
  },
  {
    input => "a\t",
    expected => r"(list (char 'a') (char '	'))",
  },
  {
    input => 'あいう',
    expected => "(list (char 'あ') (char 'い') (char 'う'))",
  },
  {
    input => 'ab*',
    expected => "(list (char 'a') (* (char 'b')))",
  },
]
