#!/usr/bin/perl

#Usage: ./do-fwd.pl 

# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

@exceptions = ( 'Item', 'In', 'X', 'Data', 'signal', 'NodeType', 'Status', 'GraphVertexCopier', 'GraphEdgeCopier', 'StorageImpl', 'Cont', 'OROCOS_SIGNAL_N', 'OROCOS_SIGNAL_CONNECTION_N', 'FindProp' );

my @files= @ARGV;

# We first create a hash of old => new
# headers.txt is the output of:
#  find src/ -name "*.hpp" |grep -v boost | grep -v oro_ | grep -v fosi  > headers.txt; echo -e "src/rtt-config.h\nsrc/transports/corba/rtt-corba-config.h\n">> headers.txt
open(INFILE, "<headers.txt") or die "I need the headers.txt file to do my work\n Use ' find src/ -name \"*.hpp\" -o -name \"*.h.in\" |grep -v boost | grep -v oro_ | sed -e 's/.h.in/.h/g' | grep -v fosi  > headers.txt '\n to generate it in the orocos-rtt main directory.\n";
open(OUTFILE, ">class-dump.txt");

my @rlines = <INFILE>;
my %spaces = ();
foreach(@rlines) {
    my $tmp = $_;
    my $file = $_;
    $tmp =~ s|src/(.*)\n|\1|;
    # fwd mapping:
    ($nspace,$class) = $tmp =~ /(.*\/)(.*?)\.\w+$/;
    $nspace =~ s|.*?(\w+)/$|\1|g;
    if ($nspace eq "") {
	$nspace = "rtt";
    }
    #print "$nspace :: $class\n";
    
    open(INFILE, "$file") or next;

    my @clines = <INFILE>;
    my $line = "";
    foreach( @clines ) {
	$line .= $_;
    }
    @classes = ();
    while ( $line =~ m/(template\s*\<[^{;]+?>\s*)?(class|struct)\s+(RTT_API\s+)?(\w+)\s*(\{|\:|\#)/gs ) {
	#print ">>$1$2$4\n";
	if (grep $_ eq $4, @exceptions) {
	    next; # skip exceptions
	}
	if ( $4 =~ m/^[a-z]/ ) {
	    next; # skip lower case classes.
	}
	if ($1) {
	    push( @classes , "        " . trim($1) . "\n". "        " . "$2 " . trim($4) . ";\n");
	} else {
	    push( @classes , "        " . "$2 " . trim($4) . ";\n");
	}
	print OUTFILE "class $nspace\:\:$4\n";
    }

    #print "$nspace\:\:$class\n@classes[0]\n";

    push( @{$spaces{$nspace}}, @classes);
}
#die;

while (my ($nspace,@classes) = each(%spaces) ) {

    my $list = "#ifndef ORO_RTT_${nspace}_FWD_HPP\n#define ORO_RTT_${nspace}_FWD_HPP\n\nnamespace RTT {\n    namespace $nspace {\n";

    @classes = sort( @{ $spaces{$nspace}} );
    #$prev = 'nonesuch';
    #@classes = grep($_ ne $prev && (($prev) = $_), @classes);


    foreach ( @classes ) {
	s/(.*?)\s*\=.*?([,>])/\1\2/g;
	s/>\s*>/>/gm; #hack
	s/>\s*>/>/gm; #hack #2
	$list .= $_;
    }

    $list .= "    }\n"; 
    $list .= "    namespace detail {\n"; 
    $list .= "        using namespace $nspace;\n"; 
    $list .= "    }\n";
    $list .= "}\n#endif\n"; 

    if ($nspace eq "rtt") {
	#next;
    }
    open(OUTFILE, ">src/$nspace/rtt-$nspace-fwd.hpp") or open(OUTFILE, ">rtt-$nspace-fwd.hpp") or die "can't open rtt-$nspace-fwd.hpp: $!";

    print "Writing rtt-$nspace-fwd.hpp\n";
    print OUTFILE $list;
}
