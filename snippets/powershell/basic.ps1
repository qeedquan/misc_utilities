#!/usr/bin/env pwsh

# Parameters
param (
	$server = "http://defaultserver",
    [Parameter(Mandatory=$true)]$username,
    $password = $( Read-Host "Input password, please" )
)

$server
$username
$password

# check if an argument was set
if ($PSBoundParameters.ContainsKey('server')) {
	echo "SERVER WAS SET"
} else {
	echo "SERVER NOT SET"
}


# set variable if it doesnt exist
$myVar = if ($env:MY_ENV_VAR) { $env:MY_ENV_VAR } else { "default value" };
$myVar

# Declare a function doing some stuff
function arithmetic($x, $y) {
	$x + 2*$y
}

function print_contact($c) {
	$c.First
	$c.Last
	$c.Phone
	if ($c.First -eq "Jon") {
		echo "FFXX"
	}
	if ($c.Last -ne "Snow") {
		echo "ZZZ"
	}
}

$a = 10
$b = 20
arithmetic $a $b

$a = 30
$b = 20
arithmetic $a $b

# Make a table
$yourData = @(
    @{Computer="Data";Service="Data";State="Data";StartupMode="Data"},
    @{Computer="Data";Service="Data";State="Data";StartupMode="Data"},
    @{Computer="Data";Service="Data";State="Data";StartupMode="Data"},
    @{Computer="Data";Service="Data";State="Data";StartupMode="Data"}) | % { New-Object object | Add-Member -NotePropertyMembers $_ -PassThru }

# Make an object
$Contact = New-Object PSObject -Property @{
	First="Jon";
	Last="Doe";
	Phone="111-222-3333"
}

# Copy an object
$Foo = $Contact.PsObject.Copy()
print_contact $Contact

# Change it to see it doesn't change
$Contact.First = "WAVE"
echo $Contact.First
echo $Foo.First

# Hashtable
$hash = @{
	"Contact" = $Contact;
	"Foo" = $Foo
}

$hash["Foo"]
if (!$hash.Contains("Z")) {
	echo "NO"
}

# Push/pop directorys
pushd .
popd

# Path separator is : on UNIX, ; on Windows
"XAZ"
$str = "{0}{1}{2}" -f "G", ([IO.Path]::PathSeparator), "xZ"
echo $str

# Convert output to utf8 encoding and write it to a file
write-output "your text" | out-file -append -encoding utf8 "filename"

# Emulate tail -f
Get-Content -Path "File" -Wait

