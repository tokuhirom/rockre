use strict;
use warnings;
use Test::More;
use Test::Base::Less;
use POSIX;

for my $block (blocks) {
    is(run_rockre($block->input), $block->expected, $block->input);
}

done_testing;

sub run_rockre {
    my ($re) = @_;
    my ($out, $exit_code) = run_cmd(['./test_rockre', $re], '');
    POSIX::WIFEXITED($exit_code) or fail("Command is not exited");
    POSIX::WEXITSTATUS($exit_code)==0 or fail("Command does not existed by zero: " . POSIX::WEXITSTATUS($exit_code));
    return $out;
}

sub run_cmd {
    my ($cmd, $in) = @_;
    $in and die "TBI";

    pipe my $logrh, my $logwh
        or die "failed to create pipe:$!";

    my $pid = fork();
    if (!defined $pid) {
        die "Cannot fork: $!";
    } elsif ($pid == 0) {
        close $logrh;
        open STDERR, '>&', $logwh
            or die "failed to redirect STDERR to logfile";
        open STDOUT, '>&', $logwh
            or die "failed to redirect STDOUT to logfile";
        close $logwh;
        exec @$cmd;
    } else {
        my $out = '';
        close $logwh;
        $out .= $_ while <$logrh>;
        while (wait == -1) {}
        return ($out, $?);
    }
}

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
--- input: あいう
--- expected: (string "あいう")

