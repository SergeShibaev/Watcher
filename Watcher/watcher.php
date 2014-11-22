<?php
class Logger {
	private $_log;
	function __construct($logfile) {
		$this->_log = fopen($logfile, "w+");
	}
	function Add($message) {
		fputs($this->_log, "$message\n");
	}
	function Close() {
		fclose($this->_log);
	}
}

$log = new Logger("watcher.log");
$log->Add($_POST['OS']);
$log->Add($_POST['test']);
$log->Close();

?>