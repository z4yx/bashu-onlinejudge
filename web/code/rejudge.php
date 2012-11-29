<?php
require('inc/lang_conf.php');

function posttodaemon($data){
	$encoded="";
	while(list($k,$v) = each($data)){
		$encoded.=($encoded ? "&" : "");
		$encoded.=rawurlencode($k)."=".rawurlencode($v);
	}
	if(!($fp=fsockopen('127.0.0.1', 8881)))
		return ("Submit failed, can not connect to server.\n");

	fputs($fp, "POST /submit_prob HTTP/1.0\r\n");
	fputs($fp, "Host: 127.0.0.1\r\n");
	fputs($fp, "Content-type: application/x-www-form-urlencoded\r\n");
	fputs($fp, "Content-length: " . strlen($encoded) . "\r\n");
	fputs($fp, "Connection: close\r\n\r\n");

	fputs($fp, "$encoded\r\n");

	$line = fgets($fp,128);
	if(!strstr($line,"HTTP/1.0 200"))
		return ("Submit failed, internal error.\n");

	$results="";
	while(!feof($fp))
		$results.=fgets($fp,128);
	/*$inheader=true;
	while(!feof($fp)) {
		$line=fgets($fp,128);
		if($inheader && $line=="\r\n")
			$inheader=false;
		else if(!$inheader)
			$results.=$line;
	}*/
	fclose($fp);

	return $results;
}
session_start();
header('Content-Type: text/plain; charset=utf-8');

if(!isset($_SESSION['user']) || !isset($_SESSION['administrator']))
	die('Not Logged in.');

if(!isset($_GET['problem_id']))
	die('No argument.');

$prob=intval($_GET['problem_id']);

require('inc/database.php');

$res=mysql_query("select case_time_limit,memory_limit,case_score,compare_way from problem where problem_id=$prob");
if(!($row=mysql_fetch_row($res)))
	die('No such problem');

$data=array(
	'a'=>$prob,
	'c'=>$row[0],
	'd'=>$row[1],
	'e'=>$row[2],
	'h'=>"rejudge".$prob,
	'j'=>$row[3],
	'k'=>1 //TYPE_rejudge
);
$result = posttodaemon($data);

if(strstr($result,"OK"))
	echo "Rejudge $prob started.";
else if(strstr($result,"another"))
	echo "Sorry, another rejudge thread is running.";
else
	echo "Sorry, system error.";
?>