#!/usr/bin/perl

# A script to remove phrases that delete content words
# from the phrase table

use strict;
use utf8;
use FileHandle;
use Getopt::Long;
use List::Util qw(sum min max shuffle);
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

my $SRC_COL = 0;
my $TRG_COL = 1;
my $SHOW_FILTERED = 0;
my $SKIP_NUMBER = 0;
my $SKIP_POLAR = 0;
my $SKIP_DET = 0;
my $SKIP_MD = 0;
my $SKIP_PRN = 0;
my $SKIP_PREP = 0;
GetOptions(
"src-col=s" => \$SRC_COL,
"trg-col=s" => \$TRG_COL,
"show-filtered" => \$SHOW_FILTERED,
"skip-number"   => \$SKIP_NUMBER,
"skip-polar"  => \$SKIP_POLAR,
"skip-det"  => \$SKIP_DET,
"skip-md"  => \$SKIP_MD,
"skip-prn"  => \$SKIP_PRN,
"skip-prep"  => \$SKIP_PREP,
);

if(@ARGV != 0) {
    print STDERR "Usage: $0\n";
    exit 1;
}



# Make sure that all single-word numbers are translated as the identity
sub number_ok {
    my ($ja, $en, $jaw, $enw) = @_;
    my %num;
    while($en =~ /"([0-9]+)"/g) { $num{$1}++; }
    my $newj = $ja;
    $newj =~ tr/一ニ三四五六七八九〇/1234567890/;
    $newj =~ tr/１２３４５６７８９０/1234567890/;
    if(keys(%num)) { while($newj =~ /"([0-9]+)"/g) { $num{$1}--; } }
    for(values %num) { return 0 if $_ != 0; }
    return 1;
}

sub polar_ok {
    my ($ja, $en, $jaw, $enw) = @_;
    my $en_polar = ($en =~ /"(not|n't|no|without|unable)"/) ? 1 : 0;
    my $ja_polar = ($jaw =~ /([せわかがさたなら] ず|ま せ ん|な い|な かっ た|な く|な けれ|いいえ)/) ? 1 : 0;
    $ja_polar = 0 if ($ja_polar == 1) and ($jaw =~ /か も/);
    return ($en_polar == $ja_polar) ? 1 : 0;
}

sub det_ok {
    my ($ja, $en, $jaw, $enw) = @_;
    return (($enw =~ /^(the|a)$/) and ($jaw !~ /^([そこあ]の|)$/)) ? 0 : 1;
}

sub prn_ok {
    my ($ja, $en, $jaw, $enw) = @_;
    $jaw =~ s/ //g;
    if($enw =~ /^(i|me)$/)         { return ($jaw =~ /^((私|僕|俺)(は|が|に|を|)|)$/) ? 1 : 0; }
    elsif($enw =~ /^us$/)          { return ($jaw =~ /^(((私|僕|俺)(ら|達|たち)|我々)(は|が|に|を|)|)$/) ? 1 : 0; }
    elsif($enw =~ /^you$/)         { return ($jaw =~ /^((あなた|君)(は|が|に|を|)|)$/) ? 1 : 0; }
    elsif($enw =~ /^he$/)          { return ($jaw =~ /^((かれ|彼)(は|が|に|を|)|)$/) ? 1 : 0; }
    elsif($enw =~ /^she$/)         { return ($jaw =~ /^(彼女(は|が|に|を|)|)$/) ? 1 : 0; }
    elsif($enw =~ /^(they|them)$/) { return ($jaw =~ /^((彼女|かれ|彼)(ら|達|たち)(は|が|に|を|)|)$/) ? 1 : 0; }
    elsif($enw =~ /^my$/)          { return ($jaw =~ /^(私|僕|俺)の$/) ? 1 : 0; }
    elsif($enw =~ /^ours$/)        { return ($jaw =~ /^((私|僕|俺)(ら|達|たち)|我々)の$/) ? 1 : 0; }
    elsif($enw =~ /^your$/)        { return ($jaw =~ /^(あなた|君)の$/) ? 1 : 0; }
    elsif($enw =~ /^his$/)         { return ($jaw =~ /^(かれ|彼)の$/) ? 1 : 0; }
    elsif($enw =~ /^(her|her)$/)   { return ($jaw =~ /^彼女の$/) ? 1 : 0; }
    elsif($enw =~ /^their$/)       { return ($jaw =~ /^(彼女|かれ|彼)(ら|達|たち)の$/) ? 1 : 0; }
    return 1;
}

sub md_ok {
    my ($ja, $en, $jaw, $enw) = @_;
    $jaw =~ s/ //g;
    if($enw =~ /^(was|were)$/) { return ($jaw =~ /^(た|かった|かったです|(っ|であっ|ありまし|されまし|してい|されまし|でし|いまし|[てで]い|しまし|あっ|され|られ|していまし|してしまいまし|しまいまし|し|ってしまっ|い|だっ)[てた])$/) ? 1 : 0; }
    elsif($enw =~ /^will$/) { return ($jaw =~ /^(し|り|い|き|させていただき)ます$/) ? 1 : 0; }
    elsif($enw eq "have") { return (($jaw =~ /^(ことが|が|は|)(あって|ある|あります|ございます|しています)(|が|よ|ね)$/) or ($jaw =~ /(しまっ|してい)[てた]/)) ? 1 : 0; }
    elsif($enw eq "please") { return (($jaw =~ /^(|って|て|で|して|しておいて)(下|くだ)さい$/) or ($jaw =~ /^(お願いします|いただいて|どうぞ、|どうぞ)$/)) ? 1 : 0; }
    elsif($enw eq "do") { return (($jaw =~ /^(する|します|こと[がは](あります|ある))$/)) ? 1 : 0; }
    elsif($enw =~ /^do n[o']t$/) { return (($jaw =~ /^(しない|しません|こと[がは](ありません|ない))$/)) ? 1 : 0; }
    return 1;
}

sub prep_ok {
    my ($ja, $en, $jaw, $enw) = @_;
    $jaw =~ s/ //g;
    if($enw =~ /^up$/)      { return ($jaw =~ /上/) ? 1 : 0; }
    elsif($enw =~ /^down$/) { return ($jaw =~ /下/) ? 1 : 0; }
    elsif($enw =~ /^in$/)   { return ($jaw =~ /^(に|で|の中の|中|の)$/) ? 1 : 0; }
    elsif($enw =~ /^out$/)  { return ($jaw =~ /^(外|から)$/) ? 1 : 0; }
    elsif($enw =~ /^to$/)   { return ($jaw =~ /^(に|へ|まで|こと)$/) ? 1 : 0; }
    elsif($enw =~ /^on$/)   { return ($jaw =~ /^(で|上|に)$/) ? 1 : 0; }
    elsif($enw =~ /^for$/)  { return ($jaw =~ /^(用|用の|の|ための|のための|のために|へ|に対する|に対して)$/) ? 1 : 0; }
    return 1;
}

sub words {
    my @arr;
    while($_[0] =~ /"([^ "]+)"/g) {
        push @arr, $1;
    }
    return "@arr";
}

while(<STDIN>) {
    chomp;
    my @arr = split(/ \|\|\| /);
    # Get the values
    my $src = $arr[$SRC_COL];
    my $trg = $arr[$TRG_COL];
    my $srcw = words($src);
    my $trgw = words($trg);
    # Filter
    my $ok = 1;
    if($ok and not $SKIP_NUMBER) { $ok = number_ok($src, $trg, $srcw, $trgw); }
    if($ok and not $SKIP_POLAR) { $ok = polar_ok($src, $trg, $srcw, $trgw); }
    if($ok and not $SKIP_DET) { $ok = det_ok($src, $trg, $srcw, $trgw); }
    if($ok and not $SKIP_MD) { $ok = md_ok($src, $trg, $srcw, $trgw); }
    if($ok and not $SKIP_PRN) { $ok = prn_ok($src, $trg, $srcw, $trgw); }
    if($ok and not $SKIP_PREP) { $ok = prep_ok($src, $trg, $srcw, $trgw); }
    print "$_\n" if 
        ($ok and (not $SHOW_FILTERED)) or 
        ((not $ok) and $SHOW_FILTERED);
}
