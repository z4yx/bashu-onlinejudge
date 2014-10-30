<?php
class preferences{
	public $hidelogo;
	public $hidehotkey;
	public $sharecode;
	public $backuptime;

	function __construct()
	{
		$this->hidelogo='on';
		$this->hidehotkey='off';
		$this->sharecode='off';
	}
}