<?php
class preferences{
	public $hidelogo;
	public $sharecode;
	public $backuptime;

	function __construct()
	{
		$this->hidelogo='on';
		$this->sharecode='off';
	}
}