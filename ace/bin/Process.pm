# Process.pm,v 1.4 1999/06/16 21:12:00 coryan Exp

$inc = "Process_Unix.pm";
$DIR_SEPARATOR = "/";
$EXE_EXT = "";
$newwindow = "";

if ($^O eq "MSWin32")
{
  $inc = "Process_Win32.pm";
  $DIR_SEPARATOR = "\\";
  $EXE_EXT = ".exe";
  $newwindow = "no";
}

require $inc;

1;
