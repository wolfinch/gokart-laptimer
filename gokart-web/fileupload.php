<?php
// In PHP versions earlier than 4.1.0, $HTTP_POST_FILES should be used instead
// of $_FILES.

define ('UPLOAD_DIR', 'uploads/');
$uploadfile = UPLOAD_DIR . basename($_FILES['file']['name']);

if(False === is_dir(UPLOAD_DIR)) {
	mkdir(UPLOAD_DIR, 0777);
}

echo '<pre>';
echo "targetfile=".$uploadfile."\n";
echo "source tmpfile=".basename($_FILES['file']['tmp_name'])."\n";

if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadfile)) {
    echo "File is valid, and was successfully uploaded.\n";
} else {
    echo "Possible file upload attack!\n";
}

//echo 'Here is some more debugging info:';
//print_r($_FILES);

print "</pre>";

?>
