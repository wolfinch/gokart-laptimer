<?php
global $kart_drv_name;
$defaultName = "Name";
$UPLOAD_DIR = "/var/www/uploads/";
$kart_drv_name = include($_SERVER['DOCUMENT_ROOT']."/config.php");

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
	//echo "Invalid request cmd:".$cmd."<br>";
	//return FALSE;
	//}

	$request = "GET ".$cmd." HTTP/1.1\r\n";
	//$request = "GET HTTP/1.1\r\n";
	//	echo "request:".$request."<br>";

	//  write request to socket
	fwrite($fp, $request);

	//  read the status line
	$line  = fgets($fp, 2048);
	if(FALSE !== strpos($line, "200 OK")) {
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

if (FALSE == sendHttpRequest("STATUS:") ) {
	echo "Failed to send STATUS request <br>";
}

//echo json_encode($lapCounts);

?>