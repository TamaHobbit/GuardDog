<?php
$data=$_POST['data'];

//exec("controller.exe {$data}",$output);
print exec("uptime");
passthru("who");
foreach($output as $line)
{
	echo $line;
}
?>
