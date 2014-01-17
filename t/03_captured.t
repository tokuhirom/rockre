use strict;
use warnings;
use Test::More;
use t::Util;
use JKML;

for my $block (@{decode_jkml(do { local $/; <DATA> })}) {
    for my $case (@{$block->{cases}}) {
        my $got = run_rockre('-p', $block->{re}, $case->{str});
        $got =~ s/\n\z//;
        my $expected = $case->{expected};
        $expected =~ s/\n+\z//;
        is($got, $expected, sprintf(qq{/%s/ =~ "%s"}, $block->{re}, $case->{str}));
    }
}

done_testing;

__DATA__
[
    {
        re => 'a(..)d(...)',
        cases => [
            {
                str => r'abcdefg',
                expected => <<-EOD
                OK
                0:bc
                1:efg
                EOD
            }
        ]
    },
    {
        re => 'a(.)u(.)o',
        cases => [
            {
                str => 'aいuえo',
                expected => <<-EOD
                OK
                0:い
                1:え
                EOD
            }
        ],
    },
    {
        re => 'oa(.)dd',
        cases => [
            {
                str => 'xooaedd',
                expected => <<-EOD
                OK
                0:e
                EOD
            }
        ],
    },
    {
        re => '<(.*)>',
        cases => [
            {
                str => '<a><b><c>',
                expected => <<-EOD
                OK
                0:a><b><c
                EOD
            }
        ],
    },
    {
        re => '<(.*?)>',
        cases => [
            {
                str => '<a><b><c>',
                expected => <<-EOD
                OK
                0:a
                EOD
            }
        ],
    },
    {
        re => '<(.+)>',
        cases => [
            {
                str => '<a><b><c>',
                expected => <<-EOD
                OK
                0:a><b><c
                EOD
            }
        ],
    },
    {
        re => '<(.+?)>',
        cases => [
            {
                str => '<a><b><c>',
                expected => <<-EOD
                OK
                0:a
                EOD
            }
        ],
    },
    {
        re => '(.?)',
        cases => [
            {
                str => 'a',
                expected => <<-EOD
                OK
                0:a
                EOD
            }
        ],
    },
    {
        re => '(.??)',
        cases => [
            {
                str => 'a',
                expected => <<-EOD
                OK
                0:
                EOD
            }
        ],
    },
]
