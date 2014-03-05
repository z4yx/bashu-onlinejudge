<?php
function check_problemid(&$str)
{
  if(preg_match('/\D/',$str))
    return;
  $num=intval($str);
  if(mysql_num_rows(mysql_query('select problem_id from problem where problem_id='.$num))){
    header('location: problempage.php?problem_id='.$num);
    exit();
  }
}

if(!isset($_GET['q']))
  die('Nothing to search.');
else
  $req=($_GET['q']);
if(isset($_GET['page_id']))
  $page_id=intval($_GET['page_id']);
else
  $page_id=0;
if(strlen($req)>600)
  die('Keyword is too long');
require('inc/checklogin.php');
require('inc/database.php');
check_problemid($req);
$keyword=mysql_real_escape_string(trim($req));
$result=mysql_query("select problem_id,title,source from problem where title like '%$keyword%' or source like '%$keyword%' order by problem_id limit $page_id,100");
if(mysql_num_rows($result)==1){
  $row=mysql_fetch_row($result);
  header('location: problempage.php?problem_id='.$row[0]);
  exit();
}
$Title="Search result $page_id";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>      
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span10 offset1" style="text-align: center;">
          <h2>Search Result</h2>
        </div>
      </div>
      <div class="row-fluid">
        <div class="span10 offset1">

            <table class="table table-hover table-bordered table-left-aligned table-first-center-aligned" style="margin-bottom:0">
              <thead><tr>
                <th style="width:7%">ID</th>
                <th style="width:63%">Title</th>
                <th style="width:30%">Source</th>
              </tr></thead>
              <tbody>
                <?php 
                  while($row=mysql_fetch_row($result)){
                echo '<tr>';
                echo '<td>',$row[0],'</td>';
                echo '<td><a href="problempage.php?problem_id=',$row[0],'">',$row[1],'</a></td>';
                echo '<td>',$row[2],'</td>';
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
        var thispage=<?php echo $page_id?>;
        var u=new String("search.php?q=<?php echo urlencode($_GET['q']);?>");
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
