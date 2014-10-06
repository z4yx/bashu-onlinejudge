<?php
require('inc/lang_conf.php');

function posttodaemon($data){
	$encoded="";
	while(list($k,$v) = each($data)){
		$encoded.=($encoded ? "&" : "");
		$encoded.=rawurlencode($k)."=".rawurlencode($v);
	}
	if(!($fp=fsockopen('127.0.0.1', 8881)))
		return ('Submit failed, can not connect to server.');

	fputs($fp, "POST /submit_prob HTTP/1.0\r\n");
	fputs($fp, "Host: 127.0.0.1\r\n");
	fputs($fp, "Content-type: application/x-www-form-urlencoded\r\n");
	fputs($fp, "Content-length: " . strlen($encoded) . "\r\n");
	fputs($fp, "Connection: close\r\n\r\n");

	fputs($fp, "$encoded\r\n");

	$line = fgets($fp,128);
	if(!strstr($line,"HTTP/1.0 200"))
		return ('Submit failed, internal error.');

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
require('inc/checklogin.php');

if(!isset($_SESSION['user']) || strlen($_SESSION['user'])==0)
	die('Not Logged in.');
if(!isset($_POST['language'],$_POST['problem']))
	die('Wrong argument');

$lang=intval($_POST['language']);
if(!array_key_exists($lang,$LANG_NAME))
	die('Invalid language');

$prob=intval($_POST['problem']);

if(!isset($_POST['source']))
	die('No source code.');
$code=$_POST['source'];
if(strlen($code)>29990)
	die('Code is too long.');

require('inc/database.php');

$res=mysql_query("select case_time_limit,memory_limit,case_score,compare_way from problem where problem_id=$prob");
if(!($row=mysql_fetch_row($res)))
	die('No such problem');

$_SESSION['lang']=$lang;
mysql_query("update users set language=$lang where user_id='".$_SESSION['user']."'");
mysql_query("update problem set in_date=NOW() where problem_id=$prob");

$key=md5('key'.time().rand());
$share_code=(isset($_POST['public']) ? 1 : 0);

$data=array(
	'a'=>$prob,
	'b'=>$lang,
	'c'=>$row[0],
	'd'=>$row[1],
	'e'=>$row[2],
	'f'=>$code,
	'g'=>$_SESSION['user'],
	'h'=>$key,
	'i'=>$share_code,
	'j'=>$row[3]
);
ignore_user_abort(TRUE);
$result = posttodaemon($data);
//echo $result;
if(strstr($result,"OK"))
	header("location: wait.php?key=$key");
else
	die($result);
?>