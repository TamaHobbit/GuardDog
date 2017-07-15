<html>
<head>
<link rel="stylesheet" type="text/css" href="css/style.css">
</head>
<body>

<?php

function getImages($dir){
		$result = array();
		$docRoot = $_SERVER["DOCUMENT_ROOT"] . '/';
		$dir = $docRoot . $dir;
		$fullFilenames = glob($dir.'*');
		foreach($fullFilenames as $fullFilename){
			$result[] = substr($fullFilename, strlen($docRoot));
		}
		return $result;
}

echo '<div class=imagecollection>';
echo "<h2>Past hourly pictures</h2>";
$hourly_images = getImages("GuardDog/images/hourly/");
foreach($hourly_images as $img) {
  echo '<img class="photo grow" src="' . $img . '" />';
}
echo '</div>';

echo "<br />";

echo '<div class=imagecollection>';
echo "<h2>Recent events</h2>";
$images = getImages("GuardDog/images/events/");
foreach($images as $img) {
  echo '<img class="photo grow" src="' . $img . '" />';
}
echo '</div>';

?>

</script>
</body>
</html>
