<?php
if(!isset($_POST['id']))
	die('Invalid argument');
session_start();
if(!isset($_SESSION['user']))
	die('Not logged in');
$id=intval($_POST['id']);
require('inc/database.php');
$uid=mysql_real_escape_string($_SESSION['user']);
mysql_query("update solution set public_code=(!public_code) where solution_id=$id and user_id='$uid'");
if(1==mysql_affected_rows())
	echo "success";
else
	echo "failed";
?>