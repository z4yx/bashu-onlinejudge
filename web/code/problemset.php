<?php
require('inc/checklogin.php');

if(isset($_GET['page_id']))
  $page_id=intval($_GET['page_id']);
else if(isset($_SESSION['view']))
  $page_id=intval($_SESSION['view']/100);
else
  $page_id=10;

require('inc/database.php');
$row=mysql_fetch_row(mysql_query('select max(problem_id) from problem'));
$maxpage=intval($row[0]/100);
if($page_id<10 || $page_id>$maxpage)
  die('Argument out of range.');

if(isset($_SESSION['administrator']))
  $addt_cond='';
else
  $addt_cond=" defunct='N' and ";
$range="between $page_id"."00 and $page_id".'99';
if(isset($_SESSION['user'])){
  $user_id=$_SESSION['user'];
  $result=mysql_query("SELECT problem_id,title,accepted,submit,in_date,defunct,res,saved.pid from problem LEFT JOIN (select problem_id as pid,MIN(result) as res from solution where user_id='$user_id' and problem_id $range group by problem_id) as solved on(solved.pid=problem_id) left join (select problem_id as pid from saved_problem where user_id='$user_id') as saved on(saved.pid=problem_id) where $addt_cond problem_id $range order by problem_id");
}else{
  $result=mysql_query("select problem_id,title,accepted,submit,in_date,defunct from problem where $addt_cond problem_id $range  order by problem_id");
}
$Title="Problemset $page_id";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php') ?>
    <div class="container-fluid">
      <div class="row-fluid">
      <div class="pagination pagination-centered">
      <ul>
      <?php
      if($maxpage>10){

        for($i=10;$i<=$maxpage;++$i)
          if($i!=$page_id)
            echo '<li><a href="problemset.php?page_id=',$i,'">',$i,'</a></li>';
          else
            echo '<li class="active"><a href="problemset.php?page_id=',$i,'">',$i,'</a></li>';
      }
      ?>
        <li><a href="level.php?level=1">Levels &raquo;</a></li>
      </ul>
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
        var cur_page=<?php echo $page_id ?>;
        $('#nav_set').parent().addClass('active');
        $('#ret_url').val("problemset.php?page_id="+cur_page);

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
          <?php
            if($page_id<$maxpage){
              $nextpage=$page_id+1;
              echo "location.href='problemset.php?page_id=$nextpage';";
            }
          ?>
          return false;
        });
        $('#btn-pre').click(function(){
          if(cur_page>10)
            location.href='problemset.php?page_id='+(cur_page-1);
          return false;
        });
      });
    </script>
  </body>
</html>
