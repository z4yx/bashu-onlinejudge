<?php
//NOT use global authorization
define('REQUIRE_AUTH',0);

define('DISALLOW_GOOGLEBOT',0);

session_start();

if(!isset($_SESSION['user']) && (DISALLOW_GOOGLEBOT || !isset($_SERVER['HTTP_USER_AGENT']) || FALSE===strstr($_SERVER['HTTP_USER_AGENT'],'Googlebot'))){
	if(REQUIRE_AUTH) {
	  header("location: auth.php");
	  exit;
	}
}
