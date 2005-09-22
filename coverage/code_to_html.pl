#!/usr/bin/perl

my $FUNCTION_COVERAGE;
my $GCOV_FILE;
my $OUTPUT;

open $FUNCTION_COVERAGE, $ARGV[0] or die "Can't open function coverage: $!";
open $GCOV_FILE, $ARGV[1] or die "Can't open coverage file: $!";
open $OUTPUT, ">$ARGV[2]" or die "Can't open output file: $!";

print $OUTPUT <<EOF;
<!-- This is a generated file, do not edit -->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
    <meta name="author" content="APR Developers" /><meta name="email" content="dev@apr.apache.org" />
    <title>Test Coverage</title>
  </head>
  <body bgcolor="#ffffff" text="#000000" link="#525D76">
<p><a href="/"><img src="./images/apr_logo_wide.png" alt="The Apache Portable Runtime Project" border="0"/></a></p>
 <table border="0" width="100%" cellspacing="4">
  <tr>
   <!-- LEFT SIDE NAVIGATION -->
   <td valign="top" nowrap="nowrap">
    <a href="http://apachecon.com/"
    ><img src="http://www.apache.org/images/ac2003-150.gif" height="86"
    width="150" border="0" alt="ApacheCon" /></a>
          <p><b>Get Involved</b></p>
   <menu compact="compact">
         <li><a href="/anoncvs.txt">CVS</a></li>
         <li><a href="/mailing-lists.html">Mailing Lists</a></li>
         <li><a href="http://cvs.apache.org/snapshots/apr/">Snapshots</a></li>
         <li><a href="/compiling_win32.html">Build on Win32</a></li>
         <li><a href="/compiling_unix.html">Build on Unix</a></li>
       </menu>
     <p><b>Download!</b></p>
   <menu compact="compact">
         <li><a href="http://www.apache.org/dyn/closer.cgi/apr/">from a mirror</a></li>
       </menu>
     <p><b>Docs</b></p>
   <menu compact="compact">
         <li><a href="/docs/apr/">APR</a></li>
         <li><a href="/docs/apr-util/">APR-util</a></li>
         <li>APR-iconv</li>
       </menu>
     <p><b>Guidelines</b></p>
   <menu compact="compact">
         <li><a href="/guidelines.html">Project Guidelines</a></li>
         <li><a href="/patches.html">Contributing</a></li>
         <li><a href="/versioning.html">Version Numbers</a></li>
      </menu>
    <p><b><a href="/info/">Miscellaneous</a></b></p>
  <menu compact="compact">
        <li><a href="http://www.apache.org/LICENSE.txt">License</a></li>
        <li><a href="/projects.html">Projects using APR</a></li>
      </menu>
  </td>
  <!-- RIGHT SIDE INFORMATION -->
  <td align="left" valign="top">
  <table border="1" width="100%" cellspacing="0" rules="groups">
    <thead>
      <tr bgcolor=#f0f0f0>
        <td>percentage</td>
        <td>function</td>
      </tr>
    </thead>
    <tbody>
EOF

while (<$FUNCTION_COVERAGE>) {
    my ($percentage, $func) = m/(.*)% of [0-9]* lines executed in function (.*)/;
    last if ($percentage eq "");
    my $bgcolor;
    if ($percentage < 33) {
        $bgcolor = "#ffaaaa";
    }
    elsif ($percentage < 66) {
        $bgcolor = "#ffff77";
    }
    else {
        $bgcolor = "#aaffaa";
    }
    print $OUTPUT "      <tr bgcolor=$bgcolor><td>$percentage%</td>\n";
    print $OUTPUT "      <td>$func</td></tr>\n";
}

print $OUTPUT <<EOF;
    </tbody>
  </table>

  <br />

  <table border="1" width="100%" cellspacing="0" rules="groups">
    <thead>
      <tr bgcolor=#f0f0f0>
        <td>line #</td>
        <td>source</td>
      </tr>
    </thead>
    <tbody>
EOF

while (<$GCOV_FILE>) {
    my ($count, $line, $src) = m/\s+(.+):\s+(\d+):(.*)/;

    next if ($line == 0);
    
    my $bgcolor;
    if ($count eq "#####") {
        $bgcolor = "#ffaaaa";
    }
    elsif (not $count eq "-") {
        $bgcolor = "#aaffaa";
    }
    else {
        $bgcolor = "#ffffff"
    }
    print $OUTPUT "      <tr bgcolor=$bgcolor><td>$line</td>\n";
    $src =~ s/ /&nbsp;/g;
    $src =~ s/\t/&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;/g;
    print $OUTPUT "      <td>$src</td></tr>\n";
}

print $OUTPUT <<EOF;
    </tbody>
  </table>

</td></tr>
</table>
   <!-- FOOTER -->
   <tr><td colspan="2"><hr noshade="noshade" size="1"/></td></tr>
   <tr><td colspan="2" align="center">
        <font size="-1">
         <em>Copyright &#169; 1999-2004, The Apache Software Foundation</em>
        </font>
       </td>
   </tr>
  </table>
 </body>
</html>
EOF

