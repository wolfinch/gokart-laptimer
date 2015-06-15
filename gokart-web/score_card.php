<html>
<head>
<title>Gokart Session</title>
</head>
<style type="text/css">
.tg {
	width: 100%;
	border-collapse: collapse;
	border-spacing: 0;
	border-color: #aabcfe;
	margin: 0px auto;
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

.tg .tg-raw {
	font-weight: bold;
	font-size: 36px;
	text-align: center
}

.tg .tg-if22 {
	background-color: #D2E4FC;
	font-weight: bold;
	font-size: 22px;
	text-align: center
}

.tg .tg-if20 {
	background-color: #D2E4FC;
	font-weight: bold;
	font-size: 14px;
	text-align: center
}

#
.tg .tg-99w3 {
	background-color: #D2E4FC;
	font-weight: bold;
	font-size: 18px
}

.tg .tg-5rcs {
	font-weight: bold;
	font-size: 20px;
	color: #039;
	text-align: center
}
</style>
<body>
	<table class="tg">
		<tr>
			<th class="tg-raw" colspan="7">Results</th>
		</tr>
		<tr>
			<td class="tg-if22" width="50">Kart<br> #</td>
			<td class="tg-if22" width="70">Position</td>
			<td class="tg-if22">Driver Name</td>
			<td class="tg-if22">Lap Time</td>
			<td class="tg-if22" width="70">Lap<br> #</td>
			<td class="tg-if22">Elapsed Time</td>
			<td class="tg-if22"></td>
		</tr>
		<?php
		$UPLOAD_DIR = "uploads/";
		//$config = include 'config.php';
		// Get the configured names
		$kart_drv_name = include($_SERVER['DOCUMENT_ROOT']."/config.php");

		echo "<br> <br> <br>";
		echo "<br> <br><br>";

		$result_array[][] = array();
		//if the dir doesn't exist, return
		if(False === is_dir($UPLOAD_DIR)) {
			//	mkdir($UPLOAD_DIR, 0777);			
			return;
		}

		$dir = new DirectoryIterator($UPLOAD_DIR);
		foreach ($dir as $fileinfo) {
			if ($fileinfo->isDot()) {
				continue;
			}
			$file = $fileinfo->getPathname();
			//			echo $file."<br>";
			if((FALSE != strripos ($file, ".jpg") )) {
				continue;       //Skip the image files now.
			}

			if((FALSE == strpos ($file, "KART") )) {
				//				echo "skipping ".$file."<br>";
				continue;       //Skip the non interested files.
			}

			#if 0
			$file_hndl = fopen($file, "r") or die("Unable to open file!");
			$lap_num = 0;
			$lap_time = 0;
			$total_time = 0;
			// Output one line until end-of-file
			while(!feof($file_hndl)) {
				$file_line = fgets($file_hndl);
				//				echo $file_line."<br>";
				/* get the formatted contents from the file */
				// sscanf ($file_line, "KART:%s:START_TIME:%f:END_TIME:%f",$kart_name, $start_time, $end_time);
				if ( FALSE == strtok ($file_line, ":")) {
					//					echo "invalid first token <br>";
					continue;
				}

				//echo "FIRST kart_name: ".$kart_name."start_time:".$start_time."end_time:".$end_time."<br>";
				//sample: KART:DEV2_KEY1:START_TIME:1428594608.534828:END_TIME:1428594767.840776:

				$kart_name = strtok  (":"); // name
				strtok  (":");
				$start_time = strtok (":");
				strtok (":");
				$end_time = strtok (":");


				//				echo "SEC kart_name: ".$kart_name."start_time:".$start_time."end_time:".$end_time."<br>";

				if ($lap_num) {
					$lap_time = $end_time - $start_time;
					$total_time += $lap_time;
				}
				// get lap number
				$lap_num++;

			}
			fclose($file_hndl);
			#endif
			// read the entire file
			$file_array = file ($file);
			if(count($file_array) <= 1) {
				//skip the files with only one line (first lap detected)
				continue;
			}
			// read the array backward
			$lap_num = 0;
			$total_time = 0;
			$raw_count = 0;

			for ($i = count($file_array)-1; $i >=1; $i--) {
				$file_line = $file_array  [$i];
				/* get the formatted contents from the file */
				// sscanf ($file_line, "KART:%s:START_TIME:%f:END_TIME:%f",$kart_name, $start_time, $end_time);
				if ( FALSE == strtok ($file_line, ":")) {
					//					echo "invalid first token <br>";
					continue;
				}

				//echo "FIRST kart_name: ".$kart_name."start_time:".$start_time."end_time:".$end_time."<br>";
				//sample: KART:DEV2_KEY1:START_TIME:1428594608.534828:END_TIME:1428594767.840776:

				$kart_name = strtok  (":"); // name
				strtok  (":");
				$start_time = strtok (":");
				strtok (":");
				$end_time = strtok (":");
					
				if ($end_time == 0 || $start_time == 0 ) {
					continue;
				}
				//echo "<br> kart_name: ".$kart_name."start_time:".$start_time."end_time:".$end_time."<br>";


				$lap_time = round(($end_time - $start_time), 4);
				$total_time += $lap_time;
				// get lap number
				sscanf($kart_name, "KART%d", $kart_num);
				//echo $kart_name,"<br>",$kart_num,"<br>";

				//	$result_array = array($kart_num, $i, round($lap_time, 4));
				$result_array[$kart_num][$i] = $lap_time;
				$best_time = $result_array[$kart_num][0][1];
				$lap_num = $result_array[$kart_num][0][2];
				//Keep total time in 0th element
				if (($best_time == 0)|| ($lap_time < $best_time)){
					$best_time = $lap_time;
					$lap_num	= $i;
				}
				$result_array[$kart_num][0] = array($total_time, $best_time, $lap_num);
				//echo $result_array[$kart_num][0][1],"besttime<br>";
			}

		}
		//var_dump ($result_array);
		//echo "<br>";
		//echo "unset <br>";
		//echo "count kart2: ",count($result_array[2]),"<br>";
		//echo "count kart1: ",count($result_array[1]),"<br>";

		function count_sort($arr1, $arr2) {
			if ((count($arr2) == count($arr1))) {
				return $arr1[0][0] - $arr2[0][0];
			} else {
				return (count($arr2) - count($arr1));
			}
		}
		uasort($result_array, 'count_sort');
		//echo "sorted array<br>";
		//var_dump ($result_array);
		$position = 0;
		foreach ($result_array as $kart_num => $lap_list) {
			if($kart_num == 0) {
				continue;
			}
				
			$position++;
			//echo "sorted array row key<br>";
			//Create table entry for the kart and statistics
			echo "<tr>";
			echo "<td class=\"tg-5rcs\">".$kart_num."</td>";
			echo "<td class=\"tg-5rcs\">".$position."</td>";
			echo "<td class=\"tg-5rcs\">".$kart_drv_name['KART'.$kart_num]."</td>";
			echo "<td class=\"tg-5rcs\">".(int)($lap_list[0][1]/60),":",($lap_list[0][1] - (((int)($lap_list[0][1]/60))*60))."</td>";
			echo "<td class=\"tg-5rcs\">".$lap_list[0][2]."</td>";
			echo "<td class=\"tg-5rcs\">".(int)($lap_list[0][0]/60),":",($lap_list[0][0] - (((int)($lap_list[0][0]/60))*60))."</td>";
			echo "<td class=\"tg-5rcs\"></td>";
			echo "</tr>";
			//Print last 4 laps
			for ($i=count($lap_list)-1, $j=0; ($i >=1) && ($j < 3); $i--, $j++) {
				echo "<tr>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\">".(int)($lap_list[$i]/60),":",($lap_list[$i] - (((int)($lap_list[$i]/60))*60))."</td>";
				echo "<td class=\"tg-if20\">".$i."</td>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				echo "</tr>";
			}
			//var_dump ($kart_num);
			//echo "sorted array row val <br>count",count($lap_list);

			//var_dump ($lap_list);
			//echo "<br>";
		}
		unset($result_array);
		//var_dump($result_array);
		?>

	</table>
</body>
</html>
