<?php
// Debug PHP
ini_set('display_errors', 'On'); 
error_reporting(E_ALL);

ini_set('upload_max_filesize', '800M');
ini_set('post_max_size', '800M');
ini_set('max_input_time', 300);
ini_set('max_execution_time', 300);

include 'util.php';
class u extends \utilphp\util { }

$artifacts = $_SERVER['DOCUMENT_ROOT'].'/packages';
echo "<h1>" . $artifacts . "</h1>";

if(!is_writable($artifacts))
{
	echo "I don't have permission<br />";
	exit(1);
}

$uploaded_file = $artifacts . "/" . basename($_FILES['uploaded']['name']);

// if(isset($_FILES['uploaded']) && file_exists($uploaded_file))
// {
// 	echo "file: ".$uploaded_file." already esxists!";
// 	exit(1);
// }

if (move_uploaded_file($_FILES['uploaded']['tmp_name'], $uploaded_file))
{
	echo "El fichero es valido y se subio con exito: ". $uploaded_file .".\n";
}
else
{
?>
	<form enctype="multipart/form-data" action="upload.php" method="POST">
	    Enviar este fichero: <input name="uploaded" type="file" />
	    <input type="submit" value="Enviar fichero" />
	</form>
	<br />
<?php
	$code = $_FILES['uploaded']['error'];
	switch ($code) { 
	case UPLOAD_ERR_INI_SIZE: 
		$message = "The uploaded file exceeds the upload_max_filesize directive in php.ini"; 
		break; 
	case UPLOAD_ERR_FORM_SIZE: 
		$message = "The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form";
		break; 
	case UPLOAD_ERR_PARTIAL: 
		$message = "The uploaded file was only partially uploaded"; 
		break; 
	case UPLOAD_ERR_NO_FILE: 
		$message = "No file was uploaded"; 
		break; 
	case UPLOAD_ERR_NO_TMP_DIR: 
		$message = "Missing a temporary folder"; 
		break; 
	case UPLOAD_ERR_CANT_WRITE: 
		$message = "Failed to write file to disk"; 
		break; 
	case UPLOAD_ERR_EXTENSION: 
		$message = "File upload stopped by extension"; 
		break; 
	default: 
		$message = "Unknown upload error"; 
		break; 
	}
	echo "Error: " . $message;
	phpinfo();
}

?>

