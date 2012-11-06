<?php
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
$keyword=mysql_real_escape_string(trim($req));
$result=mysql_query("select user_id,nick,solved,submit from users where user_id like '%$keyword%' or nick like '%$keyword%' order by solved desc limit $page_id,100");
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
                <th style="width:19%">Name</th>
                <th style="width:65%">Nick</th>
                <th style="width:8%">AC</th>
                <th style="width:8%">Submit</th>
              </tr></thead>

              <tbody id="userlist">
                <?php 
                  while($row=mysql_fetch_row($result)){
                echo '<tr>';
                echo '<td><a href="#">',$row[0],'</a></td>';
                echo '<td>',htmlspecialchars($row[1]),'</td>';
                echo '<td>',$row[2],'</td>';
                echo '<td>',$row[3],'</td>';
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
      <div class="modal hide" id="UserModal">
        <div class="modal-header">
          <a class="close" data-dismiss="modal">×</a>
          <h4>User Infomation</h4>
        </div>
        <div class="modal-body" id="user_status" style="max-height:350px">
          <p>Information isn't available.</p>
        </div>
        <div class="modal-footer">
          <a href="#" class="btn" data-dismiss="modal">Close</a>
        </div>
      </div> 
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>

    </div>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap-modal.js"></script>
    <script src="../assets/js/bootstrap-dropdown.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        var thispage=<?php echo $page_id?>;
        var u=new String("searchuser.php?q=<?php echo urlencode($_GET['q']);?>");
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
        $('#userlist a').click(function(Event){
          //if($.browser.msie)
          //  $('#user_status').html('本功能不开放给IE使用!');
          //else
            $('#user_status').load("ajax_user.php?user_id="+Event.target.text);
          $('#UserModal').modal('show');
          return false;
        });
      });
    </script>
  </body>
</html>
