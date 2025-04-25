$watcom = "$Env:SYSTEMDRIVE:\WATCOM"

if (!(Test-Path -Path $watcom)) {
  Throw "OpenWatcom not found at $watcom"
}

$env:WATCOM = $watcom
$env:EDPATH = "$watcom\EDDAT"
$env:WHTMLHELP = "$watcom\BINNT\HELP"
$env:WIPFC = "$watcom\WIPFC"

$env:LIBPATH = "$watcom\lib386\"
$env:INCLUDE = "$watcom\H"

$env:Path += ";$watcom\BINNT64;$watcom\BINNT"
