use strict;
use warnings;
use Test::More;
use t::Util;
use JKML::PP;

for my $block (@{decode_jkml(do { local $/; <DATA> })}) {
    for my $case (@{$block->{cases}}) {
      my $got = run_rockre('-p', $block->{re}, $case->{str});
      $got =~ s/\n\z//;
      is($got, $case->{expected}, sprintf(qq{/%s/ =~ "%s"}, $block->{re}, $case->{str}));
    }
}

done_testing;

__DATA__
[
  {
    re => 'a',
    cases => [
      {
        str => 'a',
        expected => 'OK',
      }
    ]
  },
  {
    re => 'aa',
    cases => [
      {
        str => 'aa',
        expected => 'OK',
      }
    ]
  },
  {
    re => 'bc',
    cases => [
      {
        str => 'abc',
        expected => 'OK',
      }
    ],
  },
  {
    re => 'a',
    cases => [
      {
        str => 'b',
        expected => 'FAIL',
      }
    ],
  },
  {
    re => '^ a',
    cases => [
      {
        str => 'a',
        expected => 'OK',
      },
      {
        str => 'ba',
        expected => 'FAIL',
      }
    ],
  },
  {
    re => 'a $',
    cases => [
      {
        str => 'ba',
        expected => 'OK',
      },
      {
        str => 'bao',
        expected => 'FAIL',
      }
    ]
  },
  {
    re => 'a | b',
    cases => [
      {
        str => 'a',
        expected => 'OK',
      },
      {
        str => 'b',
        expected => 'OK',
      }
    ]
  },
  {
    re => '^ a | b',
    cases => [
      {
        str => 'oa',
        expected => 'FAIL',
      },
      {
        str => 'ao',
        expected => 'OK',
      },
      {
        str => 'ob',
        expected => 'OK',
      },
    ],
  },
  {
    re => 'a[b|c]',
    cases => [
      {
        str => 'ab',
        expected => 'OK',
      },
      {
        str => 'ac',
        expected => 'OK',
      },
      {
        str => 'a',
        expected => 'FAIL',
      },
      {
        str => 'aa',
        expected => 'FAIL',
      },
    ],
  },
  {
    re => 'a.',
    cases => [
      {
        str=> 'aa',
        expected => 'OK',
      },
      {
        str=> 'a',
        expected => 'FAIL',
      }
    ]
  },
  {
    re => 'a.c',
    cases => [
      {
        str => 'abc',
        expected => 'OK',
      },
    ]
  },
  {
    re => 'hoa?',
    cases => [
      {
        str => 'hoa',
        expected => 'OK',
      },
      {
        str => 'ho',
        expected => 'OK',
      },
      {
        str => 'h',
        expected => 'FAIL',
      },
    ]
  },
  {
    re => '^ab*$',
    cases => [
      {
        str => 'a',
        expected => 'OK',
      },
      {
        str => 'ab',
        expected => 'OK',
      },
      {
        str => 'abb',
        expected => 'OK',
      },
      {
        str => 'abbb',
        expected => 'OK',
      },
      {
        str => 'abc',
        expected => 'FAIL',
      },
    ]
  },
]

