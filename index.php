<html>
<body>

<?php
$hourly_images="{$_SERVER['DOCUMENT_ROOT']}/GuardDog/images/hourly/"
foreach(glob(hourly_images.'*') as $filename){
    echo basename($filename) . "\n";
}
?>

<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.2/jquery.min.js"></script>
<script type="text/javascript">
$(document).ready(function(){
var phpScript='controller.php';
var keyPressed='';

	$("button").click(function(){
		keyPressed=$(this).val(); // get value of key pressed
		$.post(phpScript,{data:keyPressed},function(response){
			alert(response);
		});
	});
});

</script>
</body>
</html>
