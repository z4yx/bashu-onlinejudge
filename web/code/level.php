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
  $result=mysql_query("SELECT problem_id,title,accepted,submit,in_date,defunct,res,saved.pid from problem LEFT JOIN (select problem_id as pid,MIN(result) as res from solution where user_id='$user_id' group by problem_id) as solved on(solved.pid=problem_id) left join (select problem_id as pid from saved_problem where user_id='$user_id') as saved on(saved.pid=problem_id) where $cond order by problem_id $range");
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
            <?php require 'problemset_table.php';?>
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
        $('#problemset_table').click(function(E){
          var $target = $(E.target);
          if($target.is('i.save_problem')){
            var pid = $target.attr('data-pid');
            var op;
            if($target.hasClass('icon-star'))
              op='rm_saved';
            else
              op='add_saved';
            $.get('ajax_saveproblem.php?prob='+pid+'&op='+op,function(result){
              if(/__ok__/.test(result)){
                $target.toggleClass('icon-star-empty')
                $target.toggleClass('icon-star')
              }
            });
          }
        });
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
