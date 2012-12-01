<?php
//if(!isset($_POST['url']))
//	die('Invalid argument.');
session_start();
session_unset();

setcookie('SID', '', 31415926);

?>