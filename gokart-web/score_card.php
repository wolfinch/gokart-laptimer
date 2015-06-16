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

.tg .tg-6rcs {
	font-weight: bold;
	font-size: 20px;
	color: #E89619;
	text-align: center
}

.tg .tg-if20b {
	background-color: #D2E4FC;
	font-weight: bold;
	font-size: 14px;
	align: center
	text-align: center
	padding: 0px 0px;
	border-style: solid;
	border-width: 0px;
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
		class kartInfo {
			private $kartId = 0;
			private $hallBestLapTime = 0;
			private $hallBestLapNum = 0;

			private $irBestLapTime	= 0;
			private $irBestLapNum = 0;

			private $hallLapCount = 0;
			private $irLapCount	= 0;

			private $hallTotalLapTime = 0;
			private $irTotalLapTime	= 0;

			private $hallLapTime = array();
			private $irLapTime	 = array();

			public function __construct($Id) {
				$this->kartId = $Id;
			}
			/* TODO: FIXME: Caveat - No support for multiple IR sections */
			public function irLapAdd ($lapTime, $count) {
				$this->irLapTime[$count] = $lapTime;
				$this->irLapCount++;
				$this->irTotalLapTime += $lapTime;
				if ($this->irBestLapTime === 0) {
					$this->irBestLapTime = $lapTime;
					$this->irBestLapNum	 = $count;
				} elseif ($lapTime < $this->irBestLapTime) {
					$this->irBestLapTime = $lapTime;
					$this->irBestLapNum	 = $count;
				}
				//echo "<br>ir lap add: list:".var_dump($this->irLapTime);
			}
			public function hallLapAdd ($lapTime, $count) {
				$this->hallLapTime[$count] = $lapTime;
				$this->hallLapCount++;
				$this->hallTotalLapTime += $lapTime;
				if ($this->hallBestLapTime === 0) {
					$this->hallBestLapTime = $lapTime;
					$this->hallBestLapNum	 = $count;
				}elseif ($lapTime < $this->hallBestLapTime) {
					$this->hallBestLapTime = $lapTime;
					$this->hallBestLapNum	= $count;
				}
			}
			public function irLapCountGet () {
				return $this->irLapCount;
			}
			public function hallLapCountGet () {
				return $this->hallLapCount;
			}
			public function irTotalLapTimeGet () {
				return $this->irTotalLapTime;
			}
			public function hallTotalLapTimeGet () {
				return $this->hallTotalLapTime;
			}
			public function irLapListGet () {
				return $this->irLapTime;
			}
			public function hallLapListGet () {
				return $this->hallLapTime;
			}
			public function irBestLapTimeGet () {
				return $this->irBestLapTime;
			}
			public function hallBestLapTimeGet () {
				return $this->hallBestLapTime;
			}
			public function irBestLapNumGet () {
				return $this->irBestLapNum;
			}
			public function hallBestLapNumGet () {
				return $this->hallBestLapNum;
			}
		};

		$result_array = array();

		$UPLOAD_DIR = "uploads/";
		//$config = include 'config.php';
		// Get the configured names
		$kart_drv_name = include($_SERVER['DOCUMENT_ROOT']."/config.php");

		echo "<br> <br> <br>";
		echo "<br> <br><br>";

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

			// read the entire file
			$file_array = file ($file);
			if(count($file_array) <= 1) {
				//skip the files with only one line (first lap detected)
				continue;
			}
			// read the array backward
			//echo "FIRST kart_name: ".$kart_name."start_time:".$start_time."end_time:".$end_time."<br>";
			//sample: KART:1:START_TIME:105.836:END_TIME:120.338:BAT:95:DET_TYPE:0:DET_CODE:1:
			for ($i = count($file_array)-1; $i >=1; $i--) {
				$file_line = $file_array  [$i];
				/* get the formatted contents from the file */
				if ( FALSE == strtok ($file_line, ":")) {
					//					echo "invalid first token <br>";
					continue;
				}

				//sample: KART:DEV2_KEY1:START_TIME:1428594608.534828:END_TIME:1428594767.840776:

				$kart_num = (int)strtok  (":"); // name
				strtok  (":");
				$start_time = (float)strtok (":");
				strtok (":");
				$end_time = (float)strtok (":");
				strtok (":");
				$bat_level = (int)strtok (":");
				strtok (":");
				$det_type = (int)strtok (":");
				strtok (":");
				$det_code	= (int)strtok (":");

				if ($end_time == 0 || $start_time == 0 ) {
					continue;
				}

				$lap_time = round(($end_time - $start_time), 4);
				//echo "<br> kart_name: ".$kart_name."start_time:".$start_time.
					//" end_time:".$end_time." bat_level:".$bat_level." det_type:".
					//$det_type." det_code:".$det_code." lap_time: ".$lap_time."<br>";

				// get lap number
				//sscanf($kart_name, "KART%d", );
				//echo $kart_name,"<br>",$kart_num,"<br>";

				if ($result_array[$kart_num] == null) {
					//echo "<br> new kartinfo:".$kart_num."<br";
					$result_array[$kart_num] = new kartInfo($kart_num);
				}

				if ($det_type == 0) { // IR
					/* TODO: Add support for multiple IR sections */
					$result_array[$kart_num]->irLapAdd($lap_time, $i);
				} else {
					$result_array[$kart_num]->hallLapAdd($lap_time, $i);
				}
			}
		}
		//var_dump ($result_array);
		//echo "<br>";
		//echo "unset <br>";
		//echo "count kart2: ",count($result_array[2]),"<br>";
		//echo "count kart1: ",count($result_array[1]),"<br>";

		function count_sort($arr1, $arr2) {
			//echo "<br>sorting array1<br>";
			//var_dump ($arr1);
			//echo "<br>sorting array2<br>";
			//var_dump ($arr2);
			$arr1lapCount = ($arr1->irLapCountGet() > $arr1->hallLapCountGet()?
								$arr1->irLapCountGet():$arr1->hallLapCountGet());
			$arr1totalTime = ($arr1->irTotalLapTimeGet() > $arr1->irTotalLapTimeGet() ?
								$arr1->irTotalLapTimeGet(): $arr1->irTotalLapTimeGet());
										
			$arr2lapCount = ($arr2->irLapCountGet() > $arr2->hallLapCountGet()?
								$arr2->irLapCountGet():$arr2->hallLapCountGet());	
			$arr2totalTime = ($arr2->irTotalLapTimeGet() > $arr2->irTotalLapTimeGet() ?
								$arr2->irTotalLapTimeGet(): $arr2->irTotalLapTimeGet());												
			if ($arr1lapCount == $arr1lapCount) {
				//if lap counts are same, compare the total time (hall time will be more precise)
				return ($arr1totalTime - $arr2totalTime);
			} else {
				//if lap counts are different, the more the lap is the best
				return ($arr1lapCount - $arr2lapCount);
			}
		}

		uasort($result_array, 'count_sort');
		//echo "<br>sorted array<br>";
		//var_dump ($result_array);
		$position = 0;
		foreach ($result_array as $kart_num => $kart_info) {
			if($kart_num == 0) {
				continue;
			}

			$ir_lap_list   = $kart_info->irLapListGet();
			$hall_lap_list = $kart_info->hallLapListGet();
				
			if ( $kart_info->irLapCountGet() >= $kart_info->hallLapCountGet()) {
				$best_lap_time = (int)(($kart_info->irBestlapTimeGet()/60)).":".
						($kart_info->irBestLapTimeGet() -
						(((int)($kart_info->irBestLapTimeGet()/60))*60));
				$best_lap_num = $kart_info->irBestLapNumGet();
				$total_lap_time = (int)($kart_info->irTotalLapTimeGet()/60).":".
						($kart_info->irTotalLapTimeGet() -
						(((int)($kart_info->irTotalLapTimeGet()/60))*60));
				$ir = true;		
			}else {
				$best_lap_time = (int)(($kart_info->hallBestlapTimeGet()/60)).":".
						($kart_info->hallBestLapTimeGet() -
						(((int)($kart_info->hallBestLapTimeGet()/60))*60));
				$best_lap_num = $kart_info->hallBestLapNumGet();
				$total_lap_time = (int)($kart_info->hallTotalLapTimeGet()/60).":".
						($kart_info->hallTotalLapTimeGet() -
						(((int)($kart_info->hallTotalLapTimeGet()/60))*60));
				$ir = false;								
			}
			//echo "<br>laplist count:".count($ir_lap_list)." <br>";

			$position++;
			//echo "sorted array row key<br>";
			//Create table entry for the kart and statistics
			echo "<tr>";
			echo "<td class=\"tg-5rcs\">".$kart_num."</td>";
			echo "<td class=\"tg-5rcs\">".$position."</td>";
			echo "<td class=\"tg-5rcs\">".$kart_drv_name['KART'.$kart_num]."</td>";
			echo "<td class=\"tg-".(($ir === true)?"5":"6")."rcs\">".$best_lap_time." (Best)</td>";
			echo "<td class=\"tg-".(($ir === true)?"5":"6")."rcs\">".$best_lap_num ."</td>";
			echo "<td class=\"tg-".(($ir === true)?"5":"6")."rcs\">".$total_lap_time."</td>";
			echo "<td class=\"tg-5rcs\"></td>";
			echo "</tr>";
			//Print last 4 laps
			for ($i=(($ir === true)?$kart_info->irLapCountGet():$kart_info->hallLapCountGet()), 
									$j=0; ($i >0) && ($j < 4); $i--, $j++) {
				echo "<tr>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				if ($ir == true) {
					echo "<td class=\"tg-if20\">".(int)($ir_lap_list[$i]/60),":",
							($ir_lap_list[$i] - (((int)($ir_lap_list[$i]/60))*60));
					if ($hall_lap_list[$i] != 0) {									
						echo "  (<font color=\"#E89619\" size=\"2\">".(int)($hall_lap_list[$i]/60),":",
								($hall_lap_list[$i] - (((int)($hall_lap_list[$i]/60))*60))."</font>)</td>";
					}
				} else {
					echo "<td class=\"tg-if20\"><font color=\"#E89619\">".(int)($hall_lap_list[$i]/60),":",
							($hall_lap_list[$i] - (((int)($hall_lap_list[$i]/60))*60))."</font>";
					if ($ir_lap_list[$i] != 0) {		
						echo "  (<font size=\"2\">".(int)($ir_lap_list[$i]/60),":",
								($ir_lap_list[$i] - (((int)($ir_lap_list[$i]/60))*60))."</font>)</td>";
					}
				}
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
