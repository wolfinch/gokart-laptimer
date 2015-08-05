<?php
define (MAX_TIME, 999999);
define (MAX_KART_NUM, 8);
define (DEFAULT_NAME, "Name");
define(UPLOAD_DIR, "uploads/");

global $GlobalData;
date_default_timezone_set('Asia/Kolkata');

class sessionData {
	private $kartId;
	public $drvName;
	private $started;
	public $batLevel;
	public $currLapCount;
	public $configLapCount;	

	public function isStarted() {
		return $this->started;
	}
	
	public function start($cmd) {
		global $GlobalData;
		$this->started = $cmd;
		$GlobalData->start ($cmd);			
	}
	
	public function __construct($Id, $Name = DEFAULT_NAME) {
		$this->kartId = $Id;
		$this->drvName = $Name;
		$this->batLevel = 100;
		$this->currLapCount = 0;
		$this->configLapCount = 10;
		$this->isStarted = false;
	}
};

class globalData {
	public $serverName;
	public $serverIp;
	public $serverPort;
	public $inEdit;
	private $startCount;
	
	public function isStarted () {
		if ($this->startCount > 0) {
			return TRUE;
		}
		return FALSE;
	}
	public function start ($cmd) {
		if ($cmd === TRUE) {
			$this->startCount++;
		} else {
			$this->startCount--;
		}
	}
	
	public function updateServer($name, $ip, $port) {
		$this->serverName = $name;
		$this->serverIp	  = $ip;
		$this->serverPort = $port;		
	}
	/*Best lap times array here
	 **** Display best 5 laps****
	 * Array will contain 6 laps, 6th one will not be logically correct */
	private $dayBest 	;//= array();
	private $monthBest  ;//= array();
	private $yearBest 	;//= array();
	private $lastPurgeTime;
	
	private function purgeOldBests() {
		
		if(date('d') != date('d', $this->lastPurgeTime)) {
			//echo "d:".date('d')."lastD:".date('d', $this->lastPurgeTime);			
			/*last purge date not in same day, clear day's bests*/
			foreach ($this->dayBest as $key=>$list) {
				$this->dayBest[$key] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
			}
		}
		if(date('m') != date('m', $this->lastPurgeTime)) {
			/*last purge month not in same month, clear month bests*/
			//echo "m:".date('m')."lastM:".date('m', $this->lastPurgeTime);
			foreach ($this->monthBest as $key=>$list) {
				$this->monthBest[$key] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
			}
		}
		if(date('Y') != date('Y', $this->lastPurgeTime)) {
			//echo "Y:".date('Y')."lastY:".date('Y', $this->lastPurgeTime);			
			/*last purge year not in same year, clear year bests*/
			foreach ($this->yearBest as $key=>$list) {
				$this->yearBest[$key] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
			}
		}
		$this->lastPurgeTime = time();		
	}
	
	private function array_search ($needle, $haystack) {
		foreach ($haystack as $entry) {
			if (($entry['name'] == $needle['name']) && 
					($entry['laptime'] == $needle['laptime']) ) {
						return TRUE;						
			}
		}
		return FALSE;
	}
	
	private static function findBest ($arr1, $arr2) {
		if ($arr1['laptime'] == $arr2['laptime'])
			return 0;
		if ($arr1['laptime'] < $arr2['laptime']) {
			//return ($arr2[0] == 0)?-1:1;
			return -1;
		}
		return 1;
	}
	
	public function insertIfBestLap ($currLapTime, $name) {	
		/*purge old best lists, in case */
		//var_dump($this);
		$this->purgeOldBests();
		
		/*See if the entry exists already */
		if (FALSE !== $this->array_search(array('laptime'=>round($currLapTime, 3), 'name'=>$name), $this->dayBest)) {
			//echo "already exists: ". $name, " time:". $currLapTime."\n";
			return;
		}

		/*Set Day's best */
		$this->dayBest[5] = $this->monthBest[5] = $this->yearBest[5] = 
						array('laptime'=>round($currLapTime, 3), 'name'=>$name, 'time'=>time());
		//echo "AFTERNEWTIME";	var_dump($this);						
		usort($this->dayBest, 'globalData::findBest');
		usort($this->monthBest, 'globalData::findBest');
		usort($this->yearBest, 'globalData::findBest');		
	}
	
	public function getDayBestList () {
		return $this->dayBest;	
	}
	
	public function getMonthBestList () {
		return $this->monthBest;	
	}

	public function getYearBestList () {
		return $this->yearBest;	
	}
		
	public function __construct($Name, $Ip, $Port) {
		$this->startCount = 0;
		$this->inEdit	  = FALSE;
		$this->serverName = $Name;
		$this->serverIp	  = $Ip;
		$this->serverPort = $Port;
		$this->dayBest[0] = $this->monthBest[0] = $this->yearBest[0] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
		$this->dayBest[1] = $this->monthBest[1] = $this->yearBest[1] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
		$this->dayBest[2] = $this->monthBest[2] = $this->yearBest[2] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
		$this->dayBest[3] = $this->monthBest[3] = $this->yearBest[3] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
		$this->dayBest[4] = $this->monthBest[4] = $this->yearBest[4] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
		$this->dayBest[5] = $this->monthBest[5] = $this->yearBest[5] = array ('laptime'=>MAX_TIME, 'name'=>'', 'time'=>time());
		
	}
};

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
		$this->irTotalLapTime = round($this->irLapTime[$this->irLapCount] - $this->irLapTime[0], 3);
		$this->hallTotalLapTime = round($this->hallLapTime[$this->hallLapCount] - $this->hallLapTime[0], 3);

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

?>