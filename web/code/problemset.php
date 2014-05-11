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
  $result=mysql_query("select problem_id,title,accepted,submit,in_date,defunct,res from problem LEFT JOIN (select problem_id as pid,MIN(result) as res from solution where user_id='$user_id' and problem_id $range group by problem_id) as temp on(pid=problem_id) where $addt_cond problem_id $range order by problem_id");
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
            <a class="pager-pre-link" title="Alt+A" href="#" id="btn-pre">&larr; Previous</a>
          </li>
          <li>
            <a class="pager-next-link" title="Alt+D" href="#" id="btn-next">Next &rarr;</a>
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
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        var cur_page=<?php echo $page_id ?>;
        $('#nav_set').parent().addClass('active');
        $('#ret_url').val("problemset.php?page_id="+cur_page);

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
