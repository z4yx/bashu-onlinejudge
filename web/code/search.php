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
require('inc/database.php');
check_problemid($req);
$keyword=mysql_real_escape_string(trim($req));
$result=mysql_query("select problem_id,title,source from problem where title like '%$keyword%' or source like '%$keyword%' order by problem_id limit $page_id,100");
session_start();//for login
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Search result <?php echo $page_id;?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <link href="../assets/css/bootstrap.css" rel="stylesheet">
    <link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">
    <link href="../assets/css/docs.css" rel="stylesheet">
    <!--[if IE 6]>
    <link href="ie6.min.css" rel="stylesheet">
    <![endif]-->
    <!--[if lt IE 9]>
      <script src="../assets/js/html5.js"></script>
    <![endif]-->
    <style type="text/css">
      .table td{
        text-align: left;
      }
      .table td:first-child{
        text-align: center;
      }
    </style>

  </head>

  <body>
    <?php require('page_header.php'); ?>      
    <div class="container-fluid" style="font-size:14px">
      <div class="row-fluid">
        <div class="span10 offset1" style="text-align: center;">
          <h2>Search Result</h2>
        </div>
      </div>
      <div class="row-fluid">
        <div class="span10 offset1">

            <table class="table table-hover table-bordered" style="margin-bottom:0">
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
            <a href="#" id="btn-pre">&larr; Previous</a>
          </li>
          <li>
            <a href="#" id="btn-next">Next &rarr;</a>
          </li>
        </ul>
      </div> 
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
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
