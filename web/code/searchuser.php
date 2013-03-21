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
require('inc/checklogin.php');
require('inc/database.php');
$keyword=mysql_real_escape_string(trim($req));
$result=mysql_query("select user_id,nick,solved,submit from users where user_id like '%$keyword%' or nick like '%$keyword%' order by solved desc limit $page_id,100");
$Title="Search result $page_id";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

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
                echo '<td><a href="#linkU">',$row[0],'</a></td>';
                echo '<td>',htmlspecialchars($row[1]),'</td>';
                echo '<td><a href="record.php?user_id=',$row[0],'&amp;result=0">',$row[2],'</a></td>';
                echo '<td><a href="record.php?user_id=',$row[0],'">',$row[3],'</a></td>';
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
    <script src="../assets/js/bootstrap.min.js"></script>
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
        $('#userlist').click(function(Event){
          var $target=$(Event.target);
          if($target.is('a') && $target.attr('href')=='#linkU'){
            $('#user_status').html("<p>Loading...</p>").load("ajax_user.php?user_id="+Event.target.innerHTML).scrollTop(0);
            var win=$('#UserModal');
            win.children('.modal-header').children('h4').html('User Infomation');
            win.modal('show');
            return false;
          }
        });
      });
    </script>
  </body>
</html>
