<?php
$idle_jiffies=0;
$total_jiffies=0;

$free_memory=0;
$total_memory=0;
function read_proc_stat()
{
  global $idle_jiffies, $total_jiffies;
  $file = fopen('/proc/stat','r');
  if(FALSE === $file)
    return -1;

  $line = fgets($file);
  $numbers = explode(' ', $line);
  fclose($file);

  $total_jiffies = 0;
  for($i=2;$i<=5;$i++)
    $total_jiffies += intval($numbers[$i]);

  $idle_jiffies = intval($numbers[5]);
}
function get_mem_usage()
{
  global $free_memory, $total_memory;
  $file = fopen('/proc/meminfo','r');
  if(FALSE === $file)
    return -1;

  $type='';
  $val=0;
  $unit='';
  while(!feof($file)) {
    $line = fgets($file);
    sscanf($line, "%s%d%s", $type, $val, $unit);

    if(stristr($type, 'MemTotal') !== false){
      $total_memory=$val;
    }else if(stristr($type, 'MemFree') !== false){
      $free_memory+=$val;
    }else if(stristr($type, 'Buffers') !== false){
      $free_memory+=$val;
    }else if(stristr($type, 'Cached') !== false){
      $free_memory+=$val;
    }
  }

  fclose($file);
}
header('content-type: text/plain');
session_start();

if(!isset($_SESSION['administrator']))
  exit;

if(-1==read_proc_stat())
  exit;
// echo $idle_jiffies,' ',$total_jiffies,"\n";
$old_idle_jiffies=$idle_jiffies;
$old_total_jiffies=$total_jiffies;

if(-1==get_mem_usage())
  exit;
// echo $free_memory,' ',$total_memory,"\n";
usleep(100000);

if(-1==read_proc_stat())
  exit;
// echo $idle_jiffies,' ',$total_jiffies,"\n";

echo '{"cpu":',100*(1-($idle_jiffies-$old_idle_jiffies)/($total_jiffies-$old_total_jiffies)),",";
echo '"mem":',100*(1-$free_memory/$total_memory),"}";
