use strict;
use warnings;
use utf8;
use Test::More;
use POSIX;

is(run_rockre('a'), '(string "a")');
is(run_rockre('^^'), '(head)');
is(run_rockre('$$'), '(tail)');

done_testing;

sub run_rockre {
    my ($re) = @_;
    my ($out, $exit_code) = run(['./test_rockre', $re], '');
    POSIX::WIFEXITED($exit_code) or fail("Command is not exited");
    POSIX::WEXITSTATUS($exit_code)==0 or fail("Command does not existed by zero: " . POSIX::WEXITSTATUS($exit_code));
    return $out;
}

sub run {
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
