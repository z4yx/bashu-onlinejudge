<?php 
require('inc/result_type.php');
require('inc/lang_conf.php');
if(!isset($_GET['solution_id']))
    die('Wrong argument.');
$sol_id=intval($_GET['solution_id']);
session_start();

require('inc/database.php');
$result=mysql_query("select user_id,time,memory,result,language,code_length,problem_id,public_code from solution where solution_id=$sol_id");
$row=mysql_fetch_row($result);
if(!$row)
  die('No such solution.');
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Source <?php echo $sol_id;?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <link href="../assets/css/bootstrap.css" rel="stylesheet">
    <link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">
    <link href="../assets/js/google-code-prettify/prettify.css" rel="stylesheet">
    <link href="../assets/css/docs.css" rel="stylesheet">
    <!--[if IE 6]>
    <link href="ie6.min.css" rel="stylesheet">
    <![endif]-->
    <!--[if lt IE 9]>
      <script src="../assets/js/html5.js"></script>
    <![endif]-->
  </head>

  <body onload="prettyPrint()">
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid" style="font-size:13px">
<?php
if(!isset($_SESSION['user']) || !$row[7] && strcmp($row[0],$_SESSION['user'])!=0 && !isset($_SESSION['source_browser'])){
  echo '<div class="row-fluid" style="text-align: center;">You cannot view the code.</div>';
}else{?>
      <div class="row-fluid" style="text-align: center">
          User:<?php echo $row[0];?>
      </div>
      <div class="row-fluid" style="text-align: center">
          Problem:<?php echo $row[6];?>&nbsp;&nbsp;
          Result:<?php echo $RESULT_TYPE[$row[3]];?>
      </div>
      <div class="row-fluid" style="text-align: center">
          Length:<?php echo $row[5];?>&nbsp;&nbsp;
          Language:<?php echo $LANG_NAME[$row[4]];?>
      </div>
      <div class="row-fluid" style="text-align: center">
          Time:<?php echo $row[1];?>&nbsp;&nbsp;
          Memory:<?php echo $row[2];?>
      </div>
<?php
  $result=mysql_query('select source from source_code where solution_id='.$sol_id);
  if($row=mysql_fetch_row($result)){
?>
      <div class="row-fluid">
        <div class="span10 offset1">
            <pre class="prettyprint linenums"><?php echo htmlspecialchars($row[0]);?></pre>
        </div>
      </div>
<?php
  }else{
    echo '<div class="row-fluid" style="text-align: center">Source code is not available!</div>';
  }
}
?>
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>

    </div>

    <script src="../assets/js/google-code-prettify/prettify.js"></script>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>
    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#ret_url').val("sourcecode.php?solution_id=<?php echo $sol_id ?>");
      });
    </script>
  </body>
</html>