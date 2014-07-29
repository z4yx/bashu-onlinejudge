<?php
require('inc/checklogin.php');

if(!isset($_SESSION['user'],$_SESSION['administrator'])){
  die('You are not administrator.');
}else if(!isset($_SESSION['admin_tfa']) || !$_SESSION['admin_tfa']){
  $_SESSION['admin_retpage'] = 'admin.php';
  header("Location: admin_auth.php");
  exit;
}else{
  require('inc/database.php');

  $res=mysql_query('select content from news where news_id=0');
  $index_text=($res && ($row=mysql_fetch_row($res))) ? str_replace('<br>', "\n", $row[0]) : '';
}
$Title="Admin panel";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>
  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid admin-page">
      <div class="row-fluid">

        <div class="span12">
          <div class="tabbable tabs-left">
            <ul class="nav nav-tabs" id="nav_tab">
              <li class="active"><a href="#tab_A" data-toggle="tab">Main Function</a></li>
              <li class=""><a href="#tab_B" data-toggle="tab">News</a></li>
              <li class=""><a href="#tab_C" data-toggle="tab">Experience</a></li>
              <li class=""><a href="#tab_D" data-toggle="tab">Privilege</a></li>
              <li class=""><a href="#tab_E" data-toggle="tab">Users</a></li>
            </ul>
            <div class="tab-content">
              <div class="tab-pane active" id="tab_A">
                <div class="row-fluid">
                  <div class="span3 operations">
                    <h3 class="center">Operation</h3>
                    <a href="newproblem.php" class="btn btn-primary">Add Problem</a>
                    <a href="#" id="btn_rejudge" class="btn btn-info">Rejudge...</a>
                    <div class="alert hide" id="rejudge_res" style="margin-top:20px"></div>
                  </div>
                  <div class="span5">
                    <h3 class="center">Home Page</h3>
                    <form action="#" method="post" id="form_index">
                      <input type="hidden" name="op" value="update_index">
                      <textarea name="text" rows="10" class="border-box" style="width:100%"><?php echo htmlspecialchars($index_text)?></textarea>
                      <div class="alert hide" id="alert_result">Updated successfully.</div>
                      <div class="pull-right">
                        <input type="submit" class="btn btn-small btn-primary" value="Update">
                      </div>
                    </form>
                  </div>
                  <div class="span4">
                    <h3 class="center" id="meter_title">System Info</h3>
                    <div id="cpumeter" class="meter"></div>
                    <div id="memmeter" class="meter"></div>
                  </div>
                </div>
              </div>
              <div class="tab-pane" id="tab_B">
                <div style="margin-left:50px;margin-right:50px">
                  <div id="table_news">
                    <div class="row-fluid">
                      <div class="alert span4">Loading news...</div>
                    </div>
                  </div>
                  <form action="admin.php" method="post" class="form-inline" id="form_news">
                    <label for="input_news" style="display:block">Add News</label>
                    <input type="text" id="input_news" name="news" class="input-xlarge" placeholder="Entering something...">
                    <input type="submit" class="btn" value="Add">
                    <input type="hidden" name="op" value="add_news">
                  </form>
                </div>
              </div>
              <div class="tab-pane" id="tab_C">
                <div style="margin-left:50px;margin-right:50px">
                  <div id="table_experience_title"> 
                    
                  </div>
                  <form action="admin.php" method="post" class="form-inline" id="form_experience_title">
                    <input type="text" id="input_experience" name="experience" class="input-small" placeholder="Experience&nbsp;&ge;">
                    <input type="text" id="input_experience_title" name="title" class="input-small" placeholder="Title">
                    <input type="submit" class="btn" value="Add">
                    <input type="hidden" name="op" value="add_experience_title">
                  </form>
                  <hr>
                  <form action="admin.php" method="post" id="form_level_experience">
                    <div id="table_level_experience"> 
                    </div>
                    <input type="submit" class="btn" value="Update">
                    <input type="hidden" name="op" value="update_level_experience">
                  </form>
                </div>
              </div>
              <div class="tab-pane" id="tab_D">
                <div style="margin-left:50px">
                  <div id="table_priv"></div>
                  <form action="admin.php" method="post" class="form-inline" id="form_priv">
                    <label for="input_user_id" style="display:block">Add Privilege</label>
                    <input type="text" id="input_user_id" name="user_id" class="input-small" placeholder="User ID">
                    <select name="right" id="slt_right">
                      <option value="administrator">administrator</option>
                      <option value="source_browser">source_browser</option>
                    </select>
                    <input type="submit" class="btn" value="Add">
                    <input type="hidden" name="op" value="add_priv">
                  </form>
                </div>
              </div>
              <div class="tab-pane" id="tab_E">
                <div style="margin-left:50px">
                  <div id="table_usr"></div>
                  <form action="admin.php" method="post" class="form-inline" id="form_usr">
                    <label for="input_dis_usr" style="display:block">Disable a user</label>
                    <input type="text" id="input_dis_usr" name="user_id" class="input-small" placeholder="User ID">
                    <input type="submit" class="btn" value="Disable">
                    <input type="hidden" name="op" value="disable_usr">
                  </form>
                  <hr>

                  <form action="admin.php" method="post" class="form-inline" id="form_resetpwd">
                    <label for="input_reset_usr" style="display:block">Reset Password</label>
                    <input type="text" id="input_reset_usr" name="user_id" class="input-small" placeholder="User ID">
                    <input type="submit" class="btn" value="Reset">
                    <input type="hidden" name="op" value="reset_usr">
                  </form>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
      <hr>
      <footer>
        <p>&copy; 2012-2014 Bashu Middle School</p>
      </footer>

    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/common.js"></script>
    <script src="../assets/js/highcharts.js"></script>
    <script src="../assets/js/highcharts-more.js"></script>

    <script type="text/javascript">
      $(document).ready(function(){
        $('#ret_url').val("admin.php");
        $('#btn_rejudge').click(function(){
          var obj=$('#rejudge_res').hide();
          var id=prompt("Enter problem ID","");
          if(id!=null){
            id=$.trim(id);
            if(id){
              $.get("rejudge.php?problem_id="+id,function(msg){
                if(/start/.test(msg))obj.addClass('alert-success');
                else obj.addClass('alert-error');
                obj.html(msg).slideDown();
              });
            }
          }
        });
        var getlevellist=function(){$('#table_level_experience').load('ajax_admin.php',{op:'list_level_experience'});};
        var gettitlelist=function(){$('#table_experience_title').load('ajax_admin.php',{op:'list_experience_title'});};
        var getprivlist=function(){$('#table_priv').load('ajax_admin.php',{op:'list_priv'});};
        var getnewslist=function(){$('#table_news').load('ajax_admin.php',{op:'list_news'});};
        var getusrlist=function(){$('#table_usr').load('ajax_admin.php',{op:'list_usr'});};
        $('#nav_tab').click(function(E){
          var jq=$(E.target);
          if(jq.is('a')){
            if(E.target.innerHTML.search(/Privilege/i)!=-1)
              getprivlist();
            else if(E.target.innerHTML.search(/News/i)!=-1)
              getnewslist();
            else if(E.target.innerHTML.search(/User/i)!=-1)
              getusrlist();
            else if(E.target.innerHTML.search(/Experience/i)!=-1){
              getlevellist();
              gettitlelist();
            }
          }
        });
        $('#table_experience_title').click(function(E){
          E.preventDefault()
          var $i=$(E.target);
          if($i.is('i.icon-remove')){
            var id=$i.data('id');
            $.post('ajax_admin.php',{'op':'del_title','id':id},function(){
              gettitlelist();
            })
          }
        });
        $('#form_experience_title').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$(this).serialize(),
            success:gettitlelist
          });
        });
        $('#form_level_experience').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$(this).serialize(),
            success:getlevellist
          });
        });
        $('#table_usr').click(function(E){
          E.preventDefault();
          var jq=$(E.target);
          if(jq.is('i')){
            var oper;
            var str_id=jq.parents('tr').first().children().first().contents()
              .filter(function(){return this.nodeType == 3;})
              .text();
            if(jq.hasClass('icon-remove')){
              if(!window.confirm("Are you sure to delete "+str_id))
                return false;
              oper='del_usr';
            }else{
              oper='en_usr';
            }
            $.ajax({
              type:"POST",
              url:"ajax_admin.php",
              data:{
                op:oper,
                user_id:str_id
              },
              success:getusrlist
            });
          }
          return false;
        });
        $('#table_priv').click(function(E){
          E.preventDefault();
          var jq=$(E.target);
          if(jq.is('i')){
            var jq_pri=jq.parent().parent().prev();
            var jq_uid=jq_pri.prev();
            $.ajax({
              type:"POST",
              url:"ajax_admin.php",
              data:{
                op:'del_priv',
                user_id:jq_uid.html(),
                right:jq_pri.html()
              },
              success:getprivlist
            });
          }
          return false;
        });
        $('#form_usr').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_usr').serialize(),
            success:getusrlist
          });
          return false;
        });
        $('#form_resetpwd').submit(function(E){
          E.preventDefault();
          if(!window.confirm("Are you sure to reset password?"))
            return false;
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_resetpwd').serialize(),
            success:function(info){alert(info)}
          });
          return false;
        });
        $('#form_priv').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_priv').serialize(),
            success:getprivlist
          });
          return false;
        });
        $('#form_news').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_news').serialize(),
            success:getnewslist
          });
          return false;
        });
        $('#table_news').click(function(E){
          E.preventDefault();
          var jq=$(E.target);
          if(jq.is('i')){
            var jq_id=jq.parent().parent().prev().prev().prev();
            $.ajax({
              type:"POST",
              url:"ajax_admin.php",
              data:{
                op:'del_news',
                news_id:jq_id.html()
              },
              success:function(){jq_id.parent().remove();}
            });
          }
          return false;
        });
        $('#form_index').submit(function(E){
          E.preventDefault();
          $('#alert_result').hide();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_index').serialize(),
            success:function(msg){
              if(/success/.test(msg))
                $('#alert_result').show();
            }
          });
          return false;
        });
        $('#input_adminpass').focus();
      });

      function update_chart(){
        $.getJSON('ajax_usage.php',function(data){
          // console.log(data);
          if(data&&"number"==typeof(data.cpu)){
            if(!window.cpuChart){
              window.cpuChart = new Highcharts.Chart({
                chart: {
                  renderTo: 'cpumeter'
                },        
                yAxis: [{
                  title: {
                    text: 'CPU'
                  }
                }],
                series: [{
                  data: [0],
                  yAxis: 0
                }]
              });
            }
            cpuChart.series[0].points[0].update(data.cpu,true);
          }
          if(data&&"number"==typeof(data.mem)){
            if(!window.memChart){
              window.memChart = new Highcharts.Chart({
                chart: {
                  renderTo: 'memmeter'
                },        
                yAxis: [{
                  title: {
                    text: 'Memory'
                  }
                }],
                series: [{
                  data: [0],
                  yAxis: 0
                }]
              });

              $('#meter_title').show();
            }
            memChart.series[0].points[0].update(data.mem,true);
          }

          setTimeout('update_chart()',3000);
        });
      }
      $(function () {
        Highcharts.setOptions({
          chart: {
            type: 'gauge',
            plotBorderWidth: 1,
            plotBackgroundColor: {
              linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
              stops: [
                [0, '#FFF9D9'],
                [0.2, '#FFFFFF'],
                [1, '#FFF4C6']
              ]
            },
            plotBackgroundImage: null,
            height: 150
          },
          credits: {
            enabled: false
          },

          title: {
            text: null//'VU meter'
          },
          
          pane: [{
            startAngle: -45,
            endAngle: 45,
            background: null,
            center: ['50%', '145%'],
            size: 260
          }],                 
        
          yAxis: [{
            min: 0,
            max: 100,
            tickInterval: 25,
            minorTickPosition: 'outside',
            tickPosition: 'outside',
            labels: {
              rotation: 'auto',
              distance: 20,
              formatter: function() {
                return this.value + '%';
              }
            },
            plotBands: [{
              from: 70,
              to: 100,
              color: '#C02316',
              innerRadius: '100%',
              outerRadius: '105%'
            }],
            pane: 0,
            title: {
              // text: 'Memory',
              y: -40
            }
          }],
          plotOptions: {
            gauge: {
              animation: false,
              dataLabels: {
                enabled: false
              },
              dial: {
                radius: '100%'
              }
            }
          },
          series: [{
            data: [0],
            yAxis: 0
          }]
        });

        update_chart();
      });
    </script>
  </body>
</html>