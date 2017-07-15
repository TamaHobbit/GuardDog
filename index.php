<html>
<body>

<?php
$hourly_images="{$_SERVER['DOCUMENT_ROOT']}/GuardDog/images/hourly/"
foreach(glob(hourly_images.'*') as $filename){
    echo basename($filename) . "\n";
}
?>

</script>
</body>
</html>
