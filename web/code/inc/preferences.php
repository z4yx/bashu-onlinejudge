<?php
class preferences{
	public $hidelogo;
	public $sharecode;
	public $backuptime;

	function __construct()
	{
		$this->hidelogo='off';
		$this->sharecode='off';
	}
}