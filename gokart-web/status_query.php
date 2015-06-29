<?php
define (MAX_KART_NUM, 8);
global $kart_session_data;
global $kart_cfg_data;

$UPLOAD_DIR = "/var/www/uploads/";
$kart_session_data  = unserialize(include($_SERVER['DOCUMENT_ROOT']."/data_session.php"));
$kart_cfg_data 		= unserialize(include($_SERVER['DOCUMENT_ROOT']."/data_config.php"));

function sendHttpRequest_internal($cmd) {
	global $kart_session_data;

	if (empty($kart_cfg_data->serverIp)) {
		if(empty($kart_cfg_data->serverName)) {
			echo "invalid hostname and ip:".$kart_cfg_data->serverName."<br>";
			return FALSE;
		}
		$host_ip = gethostbyname ($kart_cfg_data->serverName);
		if (!empty($host_ip)) {
			//echo "host IP resolved for hostname and ip:".$kart_cfg_data->serverName.$host_ip."<br>";
			$kart_cfg_data['SRV_IP'] = $host_ip;
		} else {
			echo "gethostbyname failure on host:".$kart_cfg_data->serverName."<br>";
			return FALSE;
		}
	} else {
		$host_ip = $kart_cfg_data->serverIp;
	}

	$port = $kart_cfg_data->serverPort;
	if(empty($port)) {
		echo "invalid port for host".$kart_cfg_data->serverIp.$kart_cfg_data->serverPort."<br>";
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

echo "<html>";
echo "<div>";
for ($i = 1; $i <= MAX_KART_NUM; $i++) {
	echo "<div id=\"lap_count".$i."\">";
	if ((int)$kart_session_data[$i]->currLapCount == 0) {
		echo "-";
	} else {
		echo $kart_session_data[$i]->currLapCount;
	}
	echo "</div>";
	echo "<div id=\"battery_level".$i."\">";	
	if ((int)$kart_session_data[$i]->batLevel == 0) {
		echo "-";
	} else {
		echo $kart_session_data[$i]->batLevel."%";
	}
	echo "</div>";
}
echo "</div>";
echo "</html>";

//echo json_encode($lapCounts);
?>
