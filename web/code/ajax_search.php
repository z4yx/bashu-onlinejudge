<?php
if(!isset($_GET['q']))
	die('Wrong argument.');
require('inc/database.php');
$q=mysql_real_escape_string($_GET['q']);
$result=mysql_query("select title from problem where title like '%$q%' limit 10");
$arr=array();
while($row=mysql_fetch_row($result))
	array_push($arr,$row[0]);
echo '{"arr":',json_encode($arr),'}';
