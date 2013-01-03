<?php
//NOT use global authorization
define('REQUIRE_AUTH',0);

session_start();
if(!isset($_SESSION['user'])){
  require_once 'inc/cookie.php';
  if(!check_cookie()) {
    if(REQUIRE_AUTH) {
      header("location: auth.php");
      exit;
    }
  }
}
