<?php
function CMP_TYPE($way, $precision)
{
	if($way=='tra')
		return 0;
	else if($way=='float')
		return (1 << 16)+ ($precision & 65535);
	else if($way=='int')
		return 2 << 16;
	else if($way=='spj')
		return 3 << 16;
	return 0;
}
session_start();
if(!isset($_SESSION['user'])||!isset($_SESSION['administrator']))
	die('You are not administrator');
if(!isset($_POST['op']))
	die('Invalid operation.');

require('inc/database.php');
$time=intval($_POST['time']);
$memory=intval($_POST['memory']);
$compare_way=isset($_POST['compare']) ? CMP_TYPE($_POST['compare'], intval($_POST['precision'])) : 0;
$score=intval($_POST['score']);
$title=isset($_POST['title']) ? mysql_real_escape_string($_POST['title']) : '';
$des=isset($_POST['description']) ? mysql_real_escape_string($_POST['description']) : '';
$input=isset($_POST['input']) ? mysql_real_escape_string($_POST['input']) : '';
$output=isset($_POST['output']) ? mysql_real_escape_string($_POST['output']) : '';
$samp_in=isset($_POST['sample_input']) ? mysql_real_escape_string($_POST['sample_input']) : '';
$samp_out=isset($_POST['sample_output']) ? mysql_real_escape_string($_POST['sample_output']) : '';
$hint=isset($_POST['hint']) ? mysql_real_escape_string($_POST['hint']) : '';
$source=isset($_POST['source']) ? mysql_real_escape_string($_POST['source']) : '';

require 'inc/problem_flags.php';
$has_tex=0;
if(isset($_POST['option_open_source'])){
	switch(intval($_POST['option_open_source'])){
		case 0:
			break;
		case 1:
			$has_tex|=PROB_SOLVED_OPENSOURCE;
			break;
		case 2:
			$has_tex|=PROB_DISABLE_OPENSOURCE;
			break;
	}
}
if(isset($_POST['option_level'])){
	$l=intval($_POST['option_level']);
	$level_max=(PROB_LEVEL_MASK>>PROB_LEVEL_SHIFT);
	if($l>=0 && $l<=$level_max){
		$has_tex|=($l<<PROB_LEVEL_SHIFT);
	}
}
foreach ($_POST as $value) {
	if(strstr($value,'[tex]') || strstr($value,'[inline]')) {
		$has_tex|=PROB_HAS_TEX;
		//echo $value;
		break;
	}
}


if($_POST['op']=='edit'){
	if(!isset($_POST['problem_id']))
		die('No such problem');
	$id=intval($_POST['problem_id']);

	$result=mysql_query("update problem set title='$title',case_time_limit=$time,memory_limit=$memory,case_score=$score,description='$des',input='$input',output='$output',sample_input='$samp_in',sample_output='$samp_out',hint='$hint',source='$source',has_tex=$has_tex,compare_way=$compare_way where problem_id=$id");
	if(!$result)
		die('Operation failed');
	else
		header("location: problempage.php?problem_id=$id");
}else if($_POST['op']=='add'){
	$id=1000;
	$result=mysql_query('select max(problem_id) from problem');
	if( ($row=mysql_fetch_row($result)) && intval($row[0]))
		$id=intval($row[0])+1;
	$result=mysql_query("insert into problem (problem_id,title,description,input,output,sample_input,sample_output,hint,source,in_date,memory_limit,case_time_limit,case_score,has_tex,compare_way) values ($id,'$title','$des','$input','$output','$samp_in','$samp_out','$hint','$source',NOW(),$memory,$time,$score,$has_tex,$compare_way)");
	if(!$result)
		die('Operation failed');
	else
		header("location: problempage.php?problem_id=$id");
}
?>