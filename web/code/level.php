<?php

require 'inc/problem_flags.php';
$level_max=(PROB_LEVEL_MASK>>PROB_LEVEL_SHIFT);
if(!isset($_GET['level'])){
  header('location: problemset.php');
  exit();
}else{
  $level=intval($_GET['level']);
  if($level<=0 || $level>$level_max)
    die('Wrong argument.');
}

if(isset($_GET['page_id']))
  $page_id=intval($_GET['page_id']);
else
  $page_id=0;

require('inc/checklogin.php');
require('inc/database.php');

$cond=" (has_tex&".PROB_LEVEL_MASK.")=".($level<<PROB_LEVEL_SHIFT);
if(!isset($_SESSION['administrator']))
  $cond.=" and defunct='N' ";
$range="limit $page_id,100";
if(isset($_SESSION['user'])){
  $user_id=$_SESSION['user'];
  $result=mysql_query("select problem_id,title,accepted,submit,in_date,defunct,res from problem LEFT JOIN (select problem_id as pid,MIN(result) as res from solution where user_id='$user_id' group by problem_id) as temp on(pid=problem_id) where $cond order by problem_id $range");
}else{
  $result=mysql_query("select problem_id,title,accepted,submit,in_date,defunct from problem where $cond order by problem_id $range");
}

$Title="Level $level Problems";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>      
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span8 offset1" style="margin-bottom: 20px;margin-top: 20px;">
          <div style="display: table-cell;vertical-align: middle;">
            Level:&nbsp;&nbsp;
          </div>
          <div style="display: table-cell;" class="pagination">
            <ul>
            <?php
              for($i=1;$i<=$level_max;++$i){
                if($i!=$level)
                  echo '<li>';
                else
                  echo '<li class="active">';
                echo '<a href="level.php?level=',$i,'">',$i,'</a></li>';
              }
            ?>
              <li><a href="problemset.php">All &raquo;</a></li>
            </ul>
          </div>
        </div>
      </div>
      <div class="row-fluid">
        <div class="span10 offset1">
          <table class="table table-striped table-bordered">
            <thead><tr>
              <th style="width:6%">ID</th>
              <?php 
                if(isset($_SESSION['user']))
                  echo '<th colspan="2">Title</th>';
                else
                  echo '<th>Title</th>';
              ?>
              <th style="width:10%">AC/Submit</th>
              <th style="width:6%">Ratio</th>
              <th style="width:25%">Last Submit</th>
            </tr></thead>
            <tbody>
              <?php 
                while($row=mysql_fetch_row($result)){
                  echo '<tr>';
                  echo '<td>',$row[0],'</td>';
                  if(isset($_SESSION['user'])){
                    echo '<td><i class=', is_null($row[6]) ? '"icon-remove icon-2x" style="visibility:hidden"' : ($row[6]? '"icon-remove icon-2x" style="color:red"' : '"icon-2x icon-ok" style="color:green"'), '></i>', '</td>';
                    echo '<td style="text-align:left;border-left:0;">';
                  }else{
                    echo '<td style="text-align:left">';
                  }
                  echo '<a href="problempage.php?problem_id=',$row[0],'">',$row[1];
                  if($row[5]=='Y')echo '<span class="label label-important">Deleted</span>';
                  echo '</a></td>';
                  echo '<td><a href="record.php?result=0&amp;problem_id=',$row[0],'">',$row[2],'</a>/';
                  echo '<a href="record.php?problem_id=',$row[0],'">',$row[3],'</a></td>';
                  echo '<td>',$row[3] ? intval($row[2]/$row[3]*100) : 0,'%</td>';
                  echo '<td>',$row[4],'</td>';
                  echo "</tr>\n";
                }
              ?>
            </tbody>
          </table>
  
        </div>  
      </div>
      <div class="row-fluid">
        <ul class="pager">
          <li>
            <a class="pager-pre-link shortcut-hint" title="Alt+A" href="#" id="btn-pre">&larr; Previous</a>
          </li>
          <li>
            <a class="pager-next-link shortcut-hint" title="Alt+D" href="#" id="btn-next">Next &rarr;</a>
          </li>
        </ul>
      </div> 
      <hr>
      <footer>
        <p>&copy; 2012-2014 Bashu Middle School</p>
      </footer>

    </div>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        var thispage=<?php echo $page_id?>;
        var u=new String("level.php?level=<?php echo $level?>");
        $('#ret_url').val(u+"&page_id="+thispage);
        $('#btn-next').click(function(){
          location.href=u+"&page_id="+(thispage+100);
          return false;
        })
        $('#btn-pre').click(function(){
          if(thispage-100>=0)
            location.href=u+"&page_id="+(thispage-100);
          return false;
        })

      });
    </script>
  </body>
</html>
