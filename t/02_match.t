use strict;
use warnings;
use Test::More;
use t::Util;
use JKML;

for my $block (@{decode_jkml(do { local $/; <DATA> })}) {
    for my $case (@{$block->{cases}}) {
      my ($str, $expected) = @$case;
      die "Str is undefined.." unless defined $str;
      my $got = run_rockre('-p', $block->{re}, $str);
      $got =~ s/\n\z//;
      is($got, $expected ? 'OK' : 'FAIL', sprintf(qq{/%s/ =~ "%s"}, $block->{re}, $str));
    }
}

done_testing;

__DATA__
[
  {
    re => 'a',
    cases => [
      ['a', true],
    ]
  },
  {
    re => 'aa',
    cases => [
      ['aa', true],
      ['a', false],
    ]
  },
  {
    re => 'bc',
    cases => [
      ['bc',  true],
      ['abc', true],
      ['abe', false],
    ],
  },
  {
    re => 'a',
    cases => [
      ['a', true],
      ['b', false],
    ],
  },
  {
    re => '^ a',
    cases => [
      ['a', true],
      ['ba', false],
    ],
  },
  {
    re => 'a $',
    cases => [
      ['ba',  true],
      ['bao', false],
    ]
  },
  {
    re => 'a || b',
    cases => [
      ['a', true],
      ['b', true],
    ]
  },
  {
    re => '^ a || b',
    cases => [
      ['oa', false],
      ['ao', true],
      ['ob', true],
    ],
  },
  {
    re => 'a[b||c]',
    cases => [
      ['ab', true],
      ['ac', true],
      ['a',  false],
      ['aa', false],
    ],
  },
  {
    re => 'a.',
    cases => [
      ['aa', true],
      ['a',  false],
    ]
  },
  {
    re => 'a.c',
    cases => [
      ['abc', true],
      ['ac',  false],
      ['abe', false],
    ]
  },
  {
    re => 'hoa?',
    cases => [
      ['hoa', true],
      ['ho',  true],
      ['h',   false],
    ]
  },
  {
    re => '^ab*$',
    cases => [
      [ 'a',    true ],
      [ 'ab',   true ],
      [ 'abb',  true ],
      [ 'abbb', true ],
      [ 'abc',  false ],
    ]
  },
  {
    re => '^ab+$',
    cases => [
      [ 'a',    false ],
      [ 'ab',   true ],
      [ 'abb',  true ],
      [ 'abbb', true ],
      [ 'abc',  false ],
    ]
  },
  {
    re => '^^a',
    cases => [
      [ <<-EOS,    true ],
      ahoge
      EOS
      [ <<-EOS,    true ],
      ooo
      ahoge
      ooo
      EOS
      [ <<-EOS,    false ],
      oae
      EOS
    ]
  },
  {
    re => 'a$$',
    cases => [
      [ <<-EOS,    true ],
      hoga
      EOS
      [ <<-EOS,    true ],
      ooo
      hogea
      ooo
      EOS
      [ <<-EOS,    false ],
      oae
      EOS
    ]
  },
]

