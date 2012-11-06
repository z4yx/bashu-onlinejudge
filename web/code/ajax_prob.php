<?php
if(!isset($_GET['page']))
	die('Wrong argument.');

$index=intval($_GET['page']);
$index*=100;

require('inc/database.php');

$query='select problem_id,title from problem where problem_id>='.$index.' and problem_id<'.($index+100);
//echo $query,"\n";
$result=mysql_query($query);
while($row=mysql_fetch_row($result)){
	echo "<li><a href=\"problempage.php?problem_id=$row[0]\" id=\"prob$row[0]\">$row[0]&nbsp;&nbsp;--&nbsp;",htmlspecialchars($row[1]),'</a></li>';
}
mysql_close();
?>