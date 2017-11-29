<?php
require('inc/result_type.php');
require('inc/lang_conf.php');
require('inc/problem_flags.php');
require('inc/checklogin.php');
require('inc/database.php');
function get_testcase_dir()
{
  if(!isset($_SESSION['testcase_dir'])||$_SESSION['testcase_dir']==''){
    $content = file_get_contents("http://127.0.0.1:8881/get_datapath");
    if(FALSE === $content)
      die("failed to get dir");
    $_SESSION['testcase_dir']=$content.DIRECTORY_SEPARATOR;
  }
  return $_SESSION['testcase_dir'];
}
function getcase($prob_id){
  $datadir=get_testcase_dir();
  $datadir=$datadir.'/'.$prob_id.'/';
  $arr = scandir($datadir);
  if(FALSE === $arr)
    $arr = array();
  $tot = 0;
  foreach($arr as $file_name)
    if(pathinfo($file_name,PATHINFO_EXTENSION)=='in')
      $tot++;
  return $tot;
}
if(!isset($_GET['op']))exit;
if(isset($_GET['problem_id']))
  $prob_id=intval($_GET['problem_id']);
else
  die('Wrong Problem ID.');
if(isset($_GET['op']))
  $op=$_GET['op'];
else
  die('Wrong argument.');
$row=mysql_fetch_row(mysql_query("select defunct,has_tex,compare_way from problem where problem_id=$prob_id"));
if(!$row)
  die('Wrong Problem ID.');
if($row[0]=='Y'&&!isset($_SESSION['administrator']))
  die('Permission Denied');
if($row[1]&PROB_IS_HIDE && !isset($_SESSION['insider']))
  die('Permission Denied');
if(intval($row[2])>>20==0)die('invaild problem id.');
$datadir=get_testcase_dir();
if($op=='download'){
  $dir=$datadir.$prob_id.'/'.$prob_id.'.zip';
  if(!is_file($dir))
    die('Can\'t open '.$prob_id.'.zip');
  $file=fopen($dir,"r");
  Header("Content-type: application/octet-stream" );
  Header("Accept-Ranges: bytes" );
  Header("Accept-Length: ".filesize($dir));
  Header("Content-Disposition: attachment; filename=".$prob_id.".zip");
  echo fread ($file,filesize($dir));
}else if($op=='update'){
  if(!isset($_SESSION['administrator']))
    die('Permission Denied');
  if(!isset($_SESSION['admin_tfa']) || !$_SESSION['admin_tfa']){
    $_SESSION['admin_retpage'] = "download.php?problem_id=$prob_id&op=$op";
    header("Location: admin_auth.php");
    exit;
  }
  $datadir=$datadir.$prob_id.'/';
  if(!is_file($datadir."addition.zip"))$command="zip ".$datadir.$prob_id.".zip ".$datadir."*.in -j";
  else $command="zip ".$datadir.$prob_id.".zip ".$datadir."*.in ".$datadir."/addition.zip -j";
  echo "system(".$command.")<br>";
  system($command,$callback);
  if($callback!=0){
    echo "<br>Something went wrong<br>Please Contact to Administrator<br>perhaps you haven't installed software \"zip\"";
  }else{
    echo "<br>Success";
  }
}else if($op=='query'){
  echo getcase($prob_id);
}else die('Wrong operation.');
?>