<html>
<head>
<link rel="stylesheet" type="text/css" href="css/style.css">
<meta http-equiv="refresh" content="30"> <!-- refresh page automatically -->
</head>
<body>

<?php

function getImages($dir){
		$result = array();
		$docRoot = $_SERVER["DOCUMENT_ROOT"] . '/';
		$dir = $docRoot . $dir;
		$fullFilenames = glob($dir.'*');
		foreach($fullFilenames as $fullFilename){
			$time = filemtime($fullFilename);
			$result[] = array(
				'file' => substr($fullFilename, strlen($docRoot)),
				'date' => date ("d F H:i:s", $time),
			);
		}
		return array_reverse($result);
}

function printImageCollection($dir, $title){
	echo '<div class=imagecollection>';
	echo "<h2>$title</h2>";
	$images = getImages($dir);
	foreach($images as $img) {
		echo '<a href="' . $img['file'] . '">';
		echo '<div class="photo grow">';
  	echo '<img src="' . $img['file'] . '" />';
		echo "<div class='desc'>{$img['date']}</div>";
		echo '</div>';
		echo '</a>';
	}
	echo '</div>';
}

printImageCollection("GuardDog/images/hourly/", "Past hourly pictures");
printImageCollection("GuardDog/images/events/", "Recent events");

?>

</script>
</body>
</html>
