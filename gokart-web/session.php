<?php
// Refresh the page in every N secs,
// This will send STATUS request to server and get files updated
global $Started;
global $kart_drv_name;

$kart_drv_name = include($_SERVER['DOCUMENT_ROOT']."/config.php");
$Started = $kart_drv_name['STARTED'];
$page = $_SERVER['PHP_SELF'];
$sec = "5";
if ($Started == True) {
	header("Refresh: $sec; url=$page");
}
?>
<html>
<style type="text/css">
.tg {
	border-collapse: collapse;
	border-spacing: 0;
	border-color: #aabcfe;
}

.tg td {
	font-family: Arial, sans-serif;
	font-size: 14px;
	padding: 10px 5px;
	border-style: solid;
	border-width: 1px;
	overflow: hidden;
	word-break: normal;
	border-color: #aabcfe;
	color: #669;
	background-color: #e8edff;
}

.tg th {
	font-family: Arial, sans-serif;
	font-size: 14px;
	font-weight: normal;
	padding: 10px 5px;
	border-style: solid;
	border-width: 1px;
	overflow: hidden;
	word-break: normal;
	border-color: #aabcfe;
	color: #039;
	background-color: #b9c9fe;
}

.tg .tg-vn4c {
	background-color: #D2E4FC
}

.tg .tg-2fkl {
	font-weight: bold;
	font-size: 36px;
	text-align: center
}

.tg .tg-if22 {
	background-color: #D2E4FC;
	font-weight: bold;
	font-size: 22px
}

.tg .tg-5rcs {
	font-weight: bold;
	font-size: 20px
}

.tg .tg-sh0f {
	font-weight: bold;
	font-size: 20px;
	text-align: right
}

.tg .tg-m08s {
	background-color: #D2E4FC;
	font-weight: bold;
	font-size: 20px
}

.button {
	border: 1px solid #0a3c59;
	background: #3e779d;
	background: -webkit-gradient(linear, left top, left bottom, from(#65a9d7),
		to(#3e779d) );
	background: -webkit-linear-gradient(top, #65a9d7, #3e779d);
	background: -moz-linear-gradient(top, #65a9d7, #3e779d);
	background: -ms-linear-gradient(top, #65a9d7, #3e779d);
	background: -o-linear-gradient(top, #65a9d7, #3e779d);
	background-image: -ms-linear-gradient(top, #65a9d7 0%, #3e779d 100%);
	padding: 10.5px 21px;
	-webkit-border-radius: 6px;
	-moz-border-radius: 6px;
	border-radius: 6px;
	-webkit-box-shadow: rgba(255, 255, 255, 0.4) 0 1px 0, inset
		rgba(255, 255, 255, 0.4) 0 1px 0;
	-moz-box-shadow: rgba(255, 255, 255, 0.4) 0 1px 0, inset
		rgba(255, 255, 255, 0.4) 0 1px 0;
	box-shadow: rgba(255, 255, 255, 0.4) 0 1px 0, inset
		rgba(255, 255, 255, 0.4) 0 1px 0;
	text-shadow: #7ea4bd 0 1px 0;
	color: #06426c;
	font-size: 14px;
	font-family: helvetica, serif;
	text-decoration: none;
	vertical-align: middle;
}

.button:hover {
	border: 1px solid #0a3c59;
	text-shadow: #1e4158 0 1px 0;
	background: #3e779d;
	background: -webkit-gradient(linear, left top, left bottom, from(#65a9d7),
		to(#3e779d) );
	background: -webkit-linear-gradient(top, #65a9d7, #3e779d);
	background: -moz-linear-gradient(top, #65a9d7, #3e779d);
	background: -ms-linear-gradient(top, #65a9d7, #3e779d);
	background: -o-linear-gradient(top, #65a9d7, #3e779d);
	background-image: -ms-linear-gradient(top, #65a9d7 0%, #3e779d 100%);
	color: #fff;
}

.button:active {
	text-shadow: #1e4158 0 1px 0;
	border: 1px solid #0a3c59;
	background: #65a9d7;
	background: -webkit-gradient(linear, left top, left bottom, from(#3e779d),
		to(#3e779d) );
	background: -webkit-linear-gradient(top, #3e779d, #65a9d7);
	background: -moz-linear-gradient(top, #3e779d, #65a9d7);
	background: -ms-linear-gradient(top, #3e779d, #65a9d7);
	background: -o-linear-gradient(top, #3e779d, #65a9d7);
	background-image: -ms-linear-gradient(top, #3e779d 0%, #65a9d7 100%);
	color: #fff;
}

.button:disabled {
	text-shadow: #1e4158 0 1px 0;
	border: 1px solid #0a3c59;
	background: #65a9d7;
	background: -webkit-gradient(linear, left top, left bottom, from(#3a779d),
		to(#3a779d) );
	background: -webkit-linear-gradient(top, #3a779d, #65a9d7);
	background: -moz-linear-gradient(top, #3a779d, #65a9d7);
	background: -ms-linear-gradient(top, #3a779d, #65a9d7);
	background: -o-linear-gradient(top, #3a779d, #65a9d7);
	background-image: -ms-linear-gradient(top, #3e779d 0%, #65a9d7 100%);
	color: #fff;
}

input[type="text"]:disabled {
	background: #ffffff;
}
</style>

<?php
global $Started;
global $kart_drv_name;
$defaultName = "Name";
$UPLOAD_DIR = "uploads/";

//$kart_drv_name = array ($defaultName, $defaultName, $defaultName, $defaultName,
//						$defaultName, $defaultName, $defaultName, $defaultName);

function sendHttpRequest_internal($cmd) {
	global $kart_drv_name;

	if (empty($kart_drv_name['SRV_IP'])) {
		if(empty($kart_drv_name['SRV_NAME'])) {
			echo "invalid hostname and ip:".$kart_drv_name['SRV_NAME']."<br>";
			return FALSE;
		}
		$host_ip = gethostbyname ($kart_drv_name['SRV_NAME']);
		if (!empty($host_ip)) {
			echo "host IP resolved for hostname and ip:".$kart_drv_name['SRV_NAME'].$host_ip."<br>";
			$kart_drv_name['SRV_IP'] = $host_ip;
		} else {
			echo "gethostbyname failure on host:".$kart_drv_name['SRV_NAME']."<br>";
			return FALSE;
		}
	} else {
		$host_ip = $kart_drv_name['SRV_IP'];
	}

	$port = $kart_drv_name['SRV_PORT'];
	if(empty($port)) {
		echo "invalid port for host".$kart_drv_name['SRV_NAME'].$kart_drv_name['SRV_PORT']."<br>";
		return FALSE;
	}

	if (!($fp = fsockopen($host_ip, $port))) {
		echo "Could not connect to server:".$host_ip."<br>";
		return FALSE;
	}

	//  construct request
	//if ($cmd != "START" && $cmd != "STOP" && $cmd != "STATUS") {
	//	echo "Invalid request cmd:".$cmd."<br>";
	//	return FALSE;
	//}

	$request = "GET ".$cmd." HTTP/1.1\r\n";
	//$request = "GET HTTP/1.1\r\n";
	//	echo "request:".$request."<br>";

	//  write request to socket
	fwrite($fp, $request);

	//  read the status line
	$line  = fgets($fp, 2048);
	if(FALSE != strpos($line, "200 OK")) {
		//		echo "status OK Recvd: ".$line."<br>";
		fclose ($fp);
		return TRUE;
	}
	//$status = explode(" ", $line);
	$line = "";
	while (!feof($fp)) {
		$line .= fgets($fp, 2048);
	}
	echo "command failed! Recvd:".$line."<br>";
	return FALSE;
}

function sendHttpRequest($cmd) {
	return sendHttprequest_internal($cmd);
	//return True;
}

// Cleanup session data
function cleanupSession () {
	// Get the configured names
	global $UPLOAD_DIR;

	//if the dir doesn't exist, return
	if(False === is_dir($UPLOAD_DIR)) {
		return;
	}

	$dir = new DirectoryIterator($UPLOAD_DIR);
	foreach ($dir as $fileinfo) {
		if ($fileinfo->isDot()) {
			continue;
		}
		$file = $fileinfo->getPathname();
		//		echo $file."<br>";
		unlink ($file);
	}
}

if (FALSE == sendHttpRequest("STATUS:") ) {
	echo "Failed to send STATUS request <br>";
}

//$Started = False;
if ( isset($_POST["startBtn"]) && $Started == False){
	if ($defaultName != $_POST['kart1Text']) {
		$kart_drv_name['DEV1_KEY1'] = $_POST['kart1Text'];
	}
	if ($defaultName != $_POST['kart2Text']) {
		$kart_drv_name['DEV2_KEY1'] = $_POST['kart2Text'];
	}
	if ($defaultName != $_POST['kart3Text']) {
		$kart_drv_name['DEV3_KEY1'] = $_POST['kart3Text'];
	}
	if ($defaultName != $_POST['kart4Text']) {
		$kart_drv_name['DEV4_KEY1'] = $_POST['kart4Text'];
	}
	if ($defaultName != $_POST['kart5Text']) {
		$kart_drv_name['DEV5_KEY1'] = $_POST['kart5Text'];
	}
	if ($defaultName != $_POST['kart6Text']) {
		$kart_drv_name['DEV6_KEY1'] = $_POST['kart6Text'];
	}
	if ($defaultName != $_POST['kart7Text']) {
		$kart_drv_name['DEV7_KEY1'] = $_POST['kart7Text'];
	}
	if ($defaultName != $_POST['kart8Text']) {
		$kart_drv_name['DEV8_KEY1'] = $_POST['kart8Text'];
	}
	if ("Server" != $_POST['serverName']) {
		$kart_drv_name['SRV_NAME'] = $_POST['serverName'];
	}
	if ("Ip" != $_POST['serverIp']) {
		$kart_drv_name['SRV_IP'] = $_POST['serverIp'];
	}
	if ("Port" != $_POST['serverPort']) {
		$kart_drv_name['SRV_PORT'] = $_POST['serverPort'];
	}

	//echo("StartBtn: " . $kart_drv_name['DEV1'] . "<br/>\n\n");

	// store the config
	file_put_contents('config.php', '<?php return ' . var_export($kart_drv_name, true) . ';');

	// Send Start command
	if (FALSE == sendHttpRequest("START:ALL:") ) {
		echo "Failed to send START request <br>";
		return FALSE;
	}
	$Started = True;
	$kart_drv_name['STARTED'] = true;
	file_put_contents('config.php', '<?php return ' . var_export($kart_drv_name, true) . ';');
} else if ( isset($_POST["stopBtn"])){
	//echo ("StopBtn: " . $kart_drv_name['DEV1'] . "<br/>\n\n");
	// Send Start command
	if (FALSE == sendHttpRequest("STOP:ALL:") ) {
		echo "Failed to send STOP request <br>";
		return FALSE;
	}
	$Started = False;
	$kart_drv_name['STARTED'] = false;
	file_put_contents('config.php', '<?php return ' . var_export($kart_drv_name, true) . ';');
} else if (isset($_POST["newBtn"]) && $Started == False) {
	//echo ("new session: <br/>\n\n");
	// cleanup the session data
	cleanupSession ();
	// Clear the driver names
	foreach ($kart_drv_name as $key => $val ) {
		if ($key == 'SRV_NAME' || $key == 'SRV_IP' ||
		$key == 'SRV_PORT' || $key == 'STARTED') {
			continue;
		}
		$kart_drv_name[$key] = $defaultName;
	}
	$kart_drv_name['STARTED'] = false;
	// store the config
	file_put_contents('config.php', '<?php return ' . var_export($kart_drv_name, true) . ';');
}
?>

<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post">
	<table class="tg">
		<tr>
			<th class="tg-2fkl" colspan="3">Session Entry</th>
		</tr>
		<tr>
			<td class="tg-if22">Kart No</td>
			<td class="tg-if22">Driver Name</td>
			<td class="tg-if22">Lap Time</td>
		</tr>
		<tr>
			<td class="tg-5rcs" align="center">1</td>
			<td class="tg-5rcs"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV1_KEY1'] ?> name="kart1Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-sh0f" align="center"></td>
		</tr>
		<tr>
			<td class="tg-m08s" align="center">2</td>
			<td class="tg-vn4c"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV2_KEY1'] ?> name="kart2Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-vn4c" align="center"></td>
		</tr>
		<tr>
			<td class="tg-5rcs" align="center">3</td>
			<td class="tg-031e"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV3_KEY1'] ?> name="kart3Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-031e" align="center"></td>
		</tr>
		<tr>
			<td class="tg-m08s" align="center">4</td>
			<td class="tg-vn4c"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV4_KEY1'] ?> name="kart4Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-vn4c" align="center"></td>
		</tr>
		<tr>
			<td class="tg-5rcs" align="center">5</td>
			<td class="tg-031e"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV5_KEY1'] ?> name="kart5Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-031e" align="center"></td>
		</tr>
		<tr>
			<td class="tg-m08s" align="center">6</td>
			<td class="tg-vn4c"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV6_KEY1'] ?> name="kart6Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-vn4c" align="center"></td>
		</tr>
		<tr>
			<td class="tg-5rcs" align="center">7</td>
			<td class="tg-031e"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV7_KEY1'] ?> name="kart7Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-031e" align="center"></td>
		</tr>
		<tr>
			<td class="tg-m08s" align="center">8</td>
			<td class="tg-vn4c"><input type="text" size=20
				Value=<?php echo $kart_drv_name['DEV8_KEY1'] ?> name="kart8Text"
				maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-vn4c" align="center"></td>
		</tr>
	</table>
	<br>
	<table class="tg">
		<tr>
			<th class="tg-if22" colspan="3">Server Details</th>
		</tr>
		<tr>
			<td class="tg-if22">Server Name</td>
			<td class="tg-if22">Server IP</td>
			<td class="tg-if22">Port</td>
		</tr>
		<tr>
			<td class="tg-5rcs"><input type="text" size=20
				Value=<?php if (empty($kart_drv_name['SRV_NAME'])) {echo "Server";} else {echo $kart_drv_name['SRV_NAME'];} ?>
				name="serverName" maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-5rcs"><input type="text" size=20
				Value=<?php if (empty($kart_drv_name['SRV_IP'])) {echo "Ip";} else {echo $kart_drv_name['SRV_IP'];} ?>
				name="serverIp" maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
			<td class="tg-5rcs"><input type="text" size=20
				Value=<?php if (empty($kart_drv_name['SRV_PORT'])) {echo "Port";} else {echo $kart_drv_name['SRV_PORT'];} ?>
				name="serverPort" maxlength="15" style="background-color: #D2E4FC;"
<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
			
			</td>
		</tr>
	</table>


	<br>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; <input type="submit"
		Value="Start" class="button" name="startBtn"
		<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>
	&nbsp; &nbsp; 
	<input type="submit" Value="Stop"
		class="button" name="stopBtn" 
		<?php if($Started == True){echo "/>";} else {echo "disabled />";} ?> 
	&nbsp; &nbsp; 
	<input type="submit"
		Value="New Session" class="button" name="newBtn" 
		<?php if($Started == True){echo "disabled />";} else {echo "/>";} ?>

</form>
<script type="text/javascript">
function buttonOnClick(id) {
	alert('Bye!'); 
	if (id == "startbtn") {
		print "abcd";
		return False;
	}
}
</script>

</html>
