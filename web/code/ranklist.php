<?php
require('inc/checklogin.php');
header('Content-Type: text/html; charset=utf-8');

if(isset($_GET['start_id']))
  $page_id=intval($_GET['start_id']);
else
  $page_id=0;

require('inc/database.php');
$row=mysql_fetch_row(mysql_query('select count(*) from users'));
$total=($row[0]);
if($page_id<0 || $page_id>=$total)
  die('Argument out of range.');
$rank=$page_id;
$result=mysql_query('select user_id,nick,solved,submit,score from users order by solved desc,submit desc limit '.$page_id.',50');
$Title="Ranklist";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>       
    <div class="container-fluid">
      <div class="form-inline" style="margin-bottom:10px">
        <label for="user_page">Page: </label>
        <select class="input-small" id="user_page"></select>
        <div class="pull-right">
          <form id="searchuser_form" action="searchuser.php" method="get" style="margin: 0 0">
            <div class="input-append"><input id="searchuser_input" autofocus="autofocus" type="text" name="q" class="input-medium" placeholder="Search User"><span id="search_addon" class="add-on"><i class="icon-search"></i></span></div>
          </form>
        </div>
        <div class="btn-group pull-right" style="margin-right:9px">
          <button class="btn btn-info dropdown-toggle" id="btn_usrcmp_menu">User Compare<span class="caret"></span></button>
          <ul class="dropdown-menu" id="usrcmp_menu">
            <li><input type="text" id="ipt_user1" placeholder="User 1"></li>
            <li><input type="text" id="ipt_user2" placeholder="User 2"></li>
            <li class="divider"></li>
            <li>
              <button id="btn_usrcmp" class="btn btn-small btn-primary pull-right" style="margin-right:9px;">Compare</button>
            </li>
          </ul>
        </div>
        <div class="pull-right" style="margin-right:9px">
          <a href="solved.php" class="btn btn-success">Recent Solved...</a>
        </div>
        <div class="clearfix"></div>
      </div>
      <div class="row-fluid">
        <div class="span12">
            <table class="table table-hover table-bordered " style="margin-bottom:0">
              <thead><tr>
                <th style="width:4%">No.</th>
                <th style="width:15%">Name</th>
                <th style="width:57%">Nick</th>
                <th style="width:7%">Score</th>
                <th style="width:6%">AC</th>
                <th style="width:6%">Submit</th>
                <th style="width:5%">Ratio</th>
              </tr></thead>
              <tbody id="userlist">
                <?php 
                  while($row=mysql_fetch_row($result)){
                echo '<tr><td>',(++$rank),'</td>';
                echo '<td><a href="#linkU">',$row[0],'</a></td>';
                echo '<td>',htmlspecialchars($row[1]),'</td>';
                echo '<td>',$row[4],'</td>';
                echo '<td><a href="record.php?user_id=',$row[0],'&amp;result=0">',$row[2],'</a></td>';
                echo '<td><a href="record.php?user_id=',$row[0],'">',$row[3],'</a></td>';
                echo '<td>',$row[3] ? intval($row[2]/$row[3]*100) : 0,'%</td>';
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
            <a class="pager-pre-link" title="Alt+A" href="ranklist.php?start_id=<?php echo $page_id-50 ?>" id="btn-pre">&larr; Previous</a>
          </li>
          <li>
            <a class="pager-next-link" title="Alt+D" href="ranklist.php?start_id=<?php echo $page_id+50 ?>" id="btn-next">Next &rarr;</a>
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
      <footer>
        <p>&copy; 2012-2014 Bashu Middle School</p>
      </footer>

    </div><!--/.container-->
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript">
      function intersection(obj1,obj2,arr1,arr2,ist){
        for(var k in obj1){
          if(obj2.hasOwnProperty(k)){
            ist.push(parseInt(k));
            delete obj2[k];
          }else{
            arr1.push(parseInt(k));
          }
        }
        for(var k in obj2)
          arr2.push(parseInt(k));
      }
      var content='';
      function output(arr){
        arr.sort(function(a,b){return a-b;});
        for(var i in arr){
          content+='<a target="_blank" href="problempage.php?problem_id=';
          content+=arr[i]+'">'+arr[i]+'</a> ';
        }
      }
      function user_diff(id1,info1,id2,info2)
      {
        var arr1=[],arr2=[],ist=[];
        content='<table class="table table-condensed" style="margin-bottom:0px;">';
        content+='<caption>'+id1+' vs '+id2+'</caption>';
        intersection(info1.solved,info2.solved,arr1,arr2,ist);
        content+='<tr class="success"><td>Problems only '+id1;
        content+=' accepted:</td></tr><tr><td><samp>';output(arr1);
        content+='</samp></td></tr><tr class="success"><td>Problems only '+id2;
        content+=' accepted:</td></tr><tr><td><samp>';output(arr2);
        content+='</samp></td></tr><tr class="success"><td>Problems both '+id1+' and '+id2;
        content+=' accepted:</td></tr><tr><td><samp>';output(ist);
        content+='</samp></td></tr>';
        arr1=[];arr2=[];ist=[];
        intersection(info1.failed,info2.failed,arr1,arr2,ist);
        content+='<tr class="error"><td>Problems only '+id1;
        content+=' tried but failed:</td></tr><tr><td><samp>';output(arr1);
        content+='</samp></td></tr><tr class="error"><td>Problems only '+id2;
        content+=' tried but failed:</td></tr><tr><td><samp>';output(arr2);
        content+='</samp></td></tr><tr class="error"><td>Problems both '+id1+' and '+id2;
        content+=' tried but failed:</td></tr><tr><td><samp>';output(ist);
        content+='</samp></td></tr></table>';
      }
      $(document).ready(function(){
        var i,o=$('#user_page'),cur=<?php echo $page_id?>;
        for(i=1;i<=<?php echo $total?>;i+=50){
          if(i-50<=cur && cur<i)
            o.append('<option id="page_selected" selected="selected">'+i+'</option>');
          else
            o.append('<option>'+i+'</option>');
        }
        $('#user_page').change(function(){
          var num=parseInt($(this).find("option:selected").text())-1;
          location.href='ranklist.php?start_id='+num;
        });
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
        $('#btn-next').click(function(){
          if(cur+1+50<=<?php echo $total?>)
            return true;
          return false;
        });
        $('#btn-pre').click(function(){
          if(cur+1-50>=1)
            return true;
          return false;
        });
        $('#searchuser_form').submit(function(){
          if($.trim($('#searchuser_input').val()).length==0)
            return false;
          return true;
        });
        $('#search_addon').click(function(){$('#searchuser_form').submit();});
        $('#btn_usrcmp_menu').click(function(E){
          $(E.target).parent().toggleClass('open');
        });
        $('#btn_usrcmp').click(function(){
          var user1=$.trim($('#ipt_user1').val());
          var user2=$.trim($('#ipt_user2').val());
          if(!user1||!user2)
            return;
          $.getJSON("ajax_user.php?type=json&user_id="+user1, function(info1){
            if(info1.hasOwnProperty('nobody')){
              alert('"'+user1+'" doesn\'t exist');
              return;
            }
            $.getJSON("ajax_user.php?type=json&user_id="+user2, function(info2){
              if(info2.hasOwnProperty('nobody')){
                alert('"'+user2+'" doesn\'t exist');
                return;
              }
              $('#usrcmp_menu').parent().removeClass('open');
              user_diff(user1,info1,user2,info2);
              $('#user_status').html(content).scrollTop(0);
              var win=$('#UserModal');
              win.children('.modal-header').children('h4').html('User Compare');
              win.modal('show');
              return false;
            });
          });
        });
        $('#nav_rank').parent().addClass('active');
        $('#ret_url').val("ranklist.php?start_id=<?php echo $page_id?>");
      }); 
    </script>
  </body>
</html>