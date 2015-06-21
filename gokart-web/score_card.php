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

.tg-inner {
	width: 10%;
	border-collapse: collapse;
	border-spacing: 0;
	border-color: #aabcfe;
	margin: 0px 0px;
	align: left;
	text-align: left;
}

.tg-inner td {
	vertical-align: middle;
	padding: 0px 0px;
	font-family: Arial, sans-serif;
	font-weight: bold;
	font-size: 10px;
	overflow: hidden;
	color: #039;
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
			public function processLapData () {
				/* Find total laptime (LastLap- Firstlap )*/
				$this->irTotalLapTime = round($this->irLapTime[$this->irLapCount-1] - $this->irLapTime[0], 3);
				$this->hallTotalLapTime = round($this->hallLapTime[$this->hallLapCount-1] - $this->hallLapTime[0], 3);

				/* Find best laptime */
				$prevLapTime = 0;
				for($lapNum=0; $lapNum <= $this->irLapCount; $lapNum++) {
					//echo "<br>lapNum".$lapNum;
					if (($prevLapTime != 0) && ($this->irLapTime[$lapNum] != 0)) { // !First lap hit
						if ($this->irBestLapTime <= 0) {
							$this->irBestLapTime = round(($this->irLapTime[$lapNum] - $prevLapTime), 3);
						} else if (($this->irLapTime[$lapNum] - $prevLapTime) != 0 &&
								 (($this->irLapTime[$lapNum] - $prevLapTime) < $this->irBestLapTime)) {
							$this->irBestLapTime = round(($this->irLapTime[$lapNum] - $prevLapTime), 3) ;
							$this->irBestLapNum = $lapNum ;
						}
					}
					if ($this->irLapTime[$lapNum] != 0) {
						$prevLapTime = $this->irLapTime[$lapNum];
					}					
				}
				
				$prevLapTime = 0;
				for($lapNum=0; $lapNum <= $this->hallLapCount; $lapNum++) {
					//echo "<br>lapNum".$lapNum."time:".$this->hallLapTime[$lapNum];
					if (($prevLapTime != 0) && ($this->hallLapTime[$lapNum] != 0)) { // !First lap hit
						if ($this->hallBestLapTime <= 0) {
							$this->hallBestLapTime = round(($this->hallLapTime[$lapNum] - $prevLapTime), 3);
						} else if (($this->hallLapTime[$lapNum] - $prevLapTime) != 0 &&
								 (($this->hallLapTime[$lapNum] - $prevLapTime) < $this->hallBestLapTime)) {
							$this->hallBestLapTime = round(($this->hallLapTime[$lapNum] - $prevLapTime), 3) ;
							$this->hallBestLapNum = $lapNum ;
						}
					}
					if ($this->hallLapTime[$lapNum] != 0) {
						$prevLapTime = $this->hallLapTime[$lapNum];
					}					
				}
			}			
			/* TODO: FIXME: Caveat - No support for multiple IR sections */
			public function irLapAdd ($lapTime, $count) {
				$this->irLapTime[$count] = $lapTime;
				if ($count > $this->irLapCount) {				
					$this->irLapCount = $count;
				}
				//echo "<br>ir lap add: list:".var_dump($this->irLapTime);
			}
			public function hallLapAdd ($lapTime, $count) {
				$this->hallLapTime[$count] = $lapTime;
				if ($count > $this->hallLapCount) {				
					$this->hallLapCount = $count;
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
		global $kart_drv_name;
		//$config = include 'config.php';
		// Get the configured names
		$kart_drv_name = include($_SERVER['DOCUMENT_ROOT']."/config.php");

		echo "<br> <br>";

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
			//sample: KART:1:TIME:105.836:LAP_NUM:120:BAT:95:DET_TYPE:0:DET_CODE:1:
			for ($i =0; $i < count($file_array); $i++) {
				$file_line = $file_array  [$i];
				/* get the formatted contents from the file */
				if ( FALSE == strtok ($file_line, ":")) {
					//					echo "invalid first token <br>";
					continue;
				}

				$kart_num = (int)strtok  (":"); // name
				strtok  (":");
				$curr_lap_time = (float)strtok (":");
				strtok (":");
				$curr_lap_num = (float)strtok (":");
				strtok (":");
				$bat_level = (int)strtok (":");
				strtok (":");
				$det_type = (int)strtok (":");
				strtok (":");
				$det_code	= (int)strtok (":");

				if ($curr_lap_time == 0 ) {
					continue;
				}

				//echo "<br> kart_name: ".$kart_name."time:".$curr_lap_time.
					//" lap_num:".$curr_lap_num." bat_level:".$bat_level." det_type:".
					//$det_type." det_code:".$det_code."<br>";

				// get lap number
				//sscanf($kart_name, "KART%d", );
				//echo $kart_name,"<br>",$kart_num,"<br>";

				if ($result_array[$kart_num] == null) {
					//echo "<br> new kartinfo:".$kart_num."<br";
					$result_array[$kart_num] = new kartInfo($kart_num);
				}

				if ($det_type == 0) { // IR
					/* TODO: Add support for multiple IR sections */
					$result_array[$kart_num]->irLapAdd($curr_lap_time, $curr_lap_num);
				} else {
					$result_array[$kart_num]->hallLapAdd($curr_lap_time, $curr_lap_num);
				}
			}
			
			if ($result_array[$kart_num] != null) {
				/* Do the calculations for the current kart */
				$result_array[$kart_num]->processLapData();
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
				// update the battery level and lapcount
				$kart_drv_name['LAPCOUNT_KART'.$kart_num] = $kart_info->irLapCountGet();
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
				$kart_drv_name['LAPCOUNT_KART'.$kart_num] = $kart_info->hallLapCountGet();				
			}
			$kart_drv_name['BATLEVEL_KART'.$kart_num] = $bat_level;
			
			//echo "<br>laplist count:".count($ir_lap_list)." <br>";

			$position++;
			//echo "sorted array row key<br>";
			//Create table entry for the kart and statistics
			echo "<tr>";
			echo "<td class=\"tg-5rcs\">".$kart_num."</td>";
			echo "<td class=\"tg-5rcs\">".$position."</td>";
			echo "<td class=\"tg-5rcs\">".$kart_drv_name['KART'.$kart_num]."</td>";
			echo "<td class=\"tg-".(($ir === true)?"5":"6")."rcs\"><table class=\"tg-inner\"><tr><td >Best Lap </td></tr></table>".$best_lap_time."</td>";
			echo "<td class=\"tg-".(($ir === true)?"5":"6")."rcs\">".$best_lap_num ."</td>";
			echo "<td class=\"tg-".(($ir === true)?"5":"6")."rcs\">".$total_lap_time."</td>";
			echo "<td class=\"tg-5rcs\"></td>";
			echo "</tr>";
			//Print last 4 laps
			for ($i=(($ir === true)?$kart_info->irLapCountGet():$kart_info->hallLapCountGet()), 
									$j=0; ($i >0) && ($j < 400); $i--, $j++) {
				echo "<tr>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				echo "<td class=\"tg-if20\"></td>";
				$k = $i;
				$ir_lap_time   = 0;
				$hall_lap_time = 0;
				while($k) {
					$k--;
					if (($ir_lap_time == 0) && $ir_lap_list[$k] && $ir_lap_list[$i]) {
						$ir_lap_time = round($ir_lap_list[$i] - $ir_lap_list[$k], 3);
					}
					if (($hall_lap_time == 0) && $hall_lap_list[$k] && $hall_lap_list[$i]) {
						$hall_lap_time = round($hall_lap_list[$i] - $hall_lap_list[$k], 3);
					}
					
					/* 0th Hit time will always be 0. So, adjust the 1st laptime accordingly */
					if ($i == 1) {
						$hall_lap_time = round($hall_lap_list[$i], 3);
						$ir_lap_time   = round($ir_lap_list[$i], 3);
					}
					
					if($ir_lap_time && $hall_lap_time)
						break;					
				}
				
				if ($ir == true) {
					echo "<td class=\"tg-if20\">".(int)($ir_lap_time/60),":",
							($ir_lap_time - (((int)($ir_lap_time/60))*60));
					if ($hall_lap_time != 0) {									
						echo "  (<font color=\"#E89619\" size=\"2\">".(int)($hall_lap_time/60),":",
								($hall_lap_time - (((int)($hall_lap_time/60))*60))."</font>)</td>";
					}
				} else {
					echo "<td class=\"tg-if20\"><font color=\"#E89619\">".(int)($hall_lap_time/60),":",
							($hall_lap_time - (((int)($hall_lap_time/60))*60))."</font>";
					if ($ir_lap_time != 0) {
						echo "  (<font size=\"2\">".(int)($ir_lap_time/60),":",
								($ir_lap_time - (((int)($ir_lap_time/60))*60))."</font>)</td>";
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
		// store the config
		file_put_contents('config.php', '<?php return ' . var_export($kart_drv_name, true) . '; ?>');
		
		//var_dump($result_array);
		?>

	</table>
</body>
</html>
