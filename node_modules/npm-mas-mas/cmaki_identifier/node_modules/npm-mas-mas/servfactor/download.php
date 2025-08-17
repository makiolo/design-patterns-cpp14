<?php
include 'util.php';
include 'stats.php';
// Globally namespaced version of the class
class u extends \utilphp\util { }

$artifacts = $_SERVER['DOCUMENT_ROOT'].'/packages';

if (!is_writable($artifacts))
{
	header("HTTP/1.0 404 Not Found");
	exit;
}

$file = $artifacts . "/" . $_REQUEST['file'];
if (!file_exists($file))
{
	header("HTTP/1.0 404 Not Found");
	exit;
}

// file are valids in pairs
if(u::ends_with($file, "-cmake.tar.gz"))
{
	// assert exists no cmake file (big bin)
	if (!file_exists(str_replace("-cmake.tar.gz", ".tar.gz", $file)))
	{
		header("HTTP/1.0 404 Not Found");
		exit;
	}
}
else
{
	// assert exists cmake file (small bin)
	if (!file_exists(str_replace(".tar.gz", "-cmake.tar.gz", $file)))
	{
		header("HTTP/1.0 404 Not Found");
		exit;
	}
}

$data = read_stats();
$data = inc_stats($data, $file);
write_stats($data);

header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Disposition: attachment; filename="'.basename($file).'"');
header('Content-Transfer-Encoding: binary');
header('Expires: 0');
header('Pragma: public');
header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
header('Content-Length: ' . filesize($file));
ob_clean();
flush();
readfile($file);
exit;
?>
