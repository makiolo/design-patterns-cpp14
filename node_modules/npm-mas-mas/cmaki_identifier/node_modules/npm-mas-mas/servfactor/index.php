<?php

$packages_dir = '/var/www/html/packages';

$debug_mode = false;
if(isset($_REQUEST['debug']))
{
	$debug_mode = true;
	// Debug PHP
	ini_set('display_errors', 'On'); 
	ini_set('display_startup_errors', 1);
	error_reporting(E_ALL);
}

$quiet_mode = false;
if(isset($_REQUEST['quiet']))
{
	$quiet_mode = true;
	header("Content-Type: text/plain");
}

function formatSize( $bytes )
{
	$types = array( 'B', 'KB', 'MB', 'GB', 'TB' );
	for( $i = 0; $bytes >= 1024 && $i < ( count( $types ) -1 ); $bytes /= 1024, $i++ );
	return( round( $bytes, 2 ) . " " . $types[$i] );
}

if(!$quiet_mode)
{

/* get disk space free (in bytes) */
$df = disk_free_space($packages_dir);
/* and get disk space total (in bytes)  */
$dt = disk_total_space($packages_dir);
/* now we calculate the disk space used (in bytes) */
$du = $dt - $df;
/* percentage of disk used - this will be used to also set the width % of the progress bar */
$dp = sprintf('%.2f',($du / $dt) * 100);

/* and we formate the size from bytes to MB, GB, etc. */
$df = formatSize($df);
$du = formatSize($du);
$dt = formatSize($dt);

?>

<style type='text/css'>

.progress {
        border: 2px solid #5E96E4;
        height: 32px;
        width: 540px;
        margin: 30px auto;
}
.progress .prgbar {
        background: #A7C6FF;
        width: <?php echo $dp; ?>%;
        position: relative;
        height: 32px;
        z-index: 999;
}
.progress .prgtext {
        color: #286692;
        text-align: center;
        font-size: 13px;
        padding: 9px 0 0;
        width: 540px;
        position: absolute;
        z-index: 1000;
}
.progress .prginfo {
        margin: 3px 0;
}

</style>
<!-- <a href="https://app.keymetrics.io/#/bucket/58ab049327a87d2558279e52/dashboard">dashboard</a><br /> -->
<!-- <a href="services.json">services</a><br /> -->
<div class='progress'>
        <div class='prgtext'><?php echo $dp; ?>% Disk Used</div>
        <div class='prgbar'></div>
        <div class='prginfo'>
                <span style='float: left;'><?php echo "$du of $dt used"; ?></span>
                <span style='float: right;'><?php echo "$df of $dt free"; ?></span>
                <span style='clear: both;'></span>
        </div>
</div>
<?php
} // end !quiet

include 'util.php';
include 'stats.php';

// Globally namespaced version of the class
class u extends \utilphp\util { }
// ok, php, my friend

if($quiet_mode)
{
	print "package;version;platform;download;hits;last_download\n";
}

$data = read_stats();
// u::var_dump($data);

// order by last download
$arr = array();    
$files = scandir($packages_dir);
$dir = $packages_dir;
foreach ($files as $file)
{

	$hits_info = get_hits($data, $file);
	$hits = $hits_info[0];
	$last_download = $hits_info[1];
	if($last_download === NULL)
	{
		$arr[$file] = 0;
	}
	else
	{
		$arr[$file] = strtotime($last_download->format("c"));
	}
}
arsort($arr);
$arr = array_keys($arr);

if(!$quiet_mode)
{
	if($dp > 95)
	{
		for ($i = 1; $i <= 10; $i++) {
			$last_file = array_pop($arr);
			if(u::ends_with($last_file, "-cmake.tar.gz"))
			{
				$big_file = str_replace("-cmake.tar.gz", ".tar.gz", $last_file);
				if(!unlink($dir . $last_file))
				{
					echo "error removing ".$last_file."<br />";
				}
				else
				{
					echo "removed ".$last_file."<br />";
				}

				if(!unlink($dir . $big_file))
				{
					echo "error removing ".$dir.$big_file."<br />";
				}
				else
				{
					echo "removed ".$dir.$big_file."<br />";
				}
				break;
			}
		}
	}
}

foreach($arr as $file)
{
	// bug si el package tiene "-"
	if(u::ends_with($file, "-cmake.tar.gz"))
	{
		// $substance = $file;

		preg_match('/([\w-]+)-([0-9\.]+)-([\w-\.]+)-cmake.tar.gz/', $file, $matches);
		$package = $matches[1];
		$version = $matches[2];
		$platform = $matches[3];

		// $substance = substr($substance, 0, strrpos($substance, "-"));
		// $platform = substr($substance, strrpos($substance, "-")+1);
		// $substance = substr($substance, 0, strrpos($substance, "-"));
		// $version = substr($substance, strrpos($substance, "-")+1);
		// $substance = substr($substance, 0, strrpos($substance, "-"));
		// $package = $substance;
		if(!isset($_REQUEST['platform']) || ($_REQUEST['platform'] == $platform))
		{
			$hits_info = get_hits($data, $file);
			$hits = $hits_info[0];
			$last_download = $hits_info[1];
			if($last_download === NULL)
			{
				if(!$quiet_mode)
				{
					$formatted = "never downloaded";
				}
				else
				{
					$formatted = "---";
				}
			}
			else
			{
				if(!$quiet_mode)
				{
					$formatted = $last_download->format("d-m-Y H:i");
				}
				else
				{
					$formatted = $last_download->format("c");
				}
			}
			if(!$quiet_mode)
			{
				echo "" . $package ." (" . $version . ") ";
				if($hits > 0)
				{
					echo "<a href='download.php?file=".$file."'>$platform</a> (".$hits." hits, last use: ".$formatted.")";
				}
				else
				{
					echo "<a href='download.php?file=".$file."'>$platform</a> (".$hits." hits)";
				}
				echo "<br />";
			}
			else
			{
				print $package.";".$version.";".$platform.";"."download.php?file=".$file.";".$hits.";".$formatted."\n";
			}
		}
	}
}

?>

