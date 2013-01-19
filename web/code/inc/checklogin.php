<?php
//NOT use global authorization
define('REQUIRE_AUTH',0);

define('DISALLOW_GOOGLEBOT',0);

session_start();
if(!isset($_SESSION['user']) && (DISALLOW_GOOGLEBOT || FALSE===strstr($_SERVER['HTTP_USER_AGENT'],'Googlebot'))){
  require_once 'inc/cookie.php';
  if(!check_cookie()) {
    if(REQUIRE_AUTH) {
      header("location: auth.php");
      exit;
    }
  }
}
