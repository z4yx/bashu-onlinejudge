<?php 
function encode_user_id($user)
{
  if(!extension_loaded('openssl'))
    return false;
  $iv='7284565820000000';
  $key=hash('sha256','my)(password_xx0',true);
  return openssl_encrypt($user,'aes-256-cbc',$key,false,$iv);
}
require('inc/checklogin.php');
require('inc/database.php');
$res=mysql_query("select content from news where news_id=0");
$index_text=($row=mysql_fetch_row($res)) ? $row[0] : '';
$res=mysql_query("select YEAR(in_date) as yr, MONTH(in_date) as mon,count(1),SUM(IF(result=0, 1, 0)) from solution GROUP BY YEAR(in_date) , MONTH(in_date) ORDER BY yr,mon");
$submit_cnt=array();
while($row=mysql_fetch_row($res))
  $submit_cnt[]=$row;
$Title="Welcome to Bashu OnlineJudge";
?>
<!DOCTYPE html>
<html>

    <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span10 offset1">
          <div id="newspad" class="well" style="font-size:16px;padding:19px 40px; background-color: #FFF">
            <div id="title" style="text-align:center; font-size:24px;cursor: pointer;">
                <h1 style="padding-bottom: 10px;">Welcome to Bashu OnlineJudge <i class="icon-double-angle-down"></i></h1>
            </div> 
            <div id="mainarea" style="display: none;">
                <?php echo $index_text?>
            </div>
          </div>
        </div>
      </div>
      <div class="row-fluid">
        <div id="chart" class="span12" style="height: 400px; margin: 0 auto"></div>
      </div>
      <hr>
      <footer>
        <p>&copy; 2012-2016 Bashu Middle School</p>
      </footer>
    </div>
    <script type="text/javascript">
      <?php
        echo 'var ws_url="ws://',$_SERVER["SERVER_ADDR"],':6844/";';
        if(isset($_SESSION['user']))
          echo 'var userid="',encode_user_id('id-'.$_SESSION['user']),'";';
      ?>
    </script>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/highcharts.js"></script>
    <script src="../assets/js/common.js"></script>
    <script src="../assets/js/chat.js"></script>
    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#ret_url').val("index.php");

        var originColor = '#E3E3E3';
        $('#newspad #title').click(function(){
            $('#newspad #mainarea').slideToggle();
            $('#title i').toggleClass('icon-double-angle-down');
            $('#title i').toggleClass('icon-double-angle-up');
            /* change color, unnecessary in this theme
            var tmp = $('#newspad').css('background-color');
            $('#newspad').css('background-color', originColor);
            originColor = tmp;
             */
        });
      });
    </script>

    <script type="text/javascript">
$(function () {
    var chart;
    $(document).ready(function() {
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'chart',
                type: 'area'
            },
            title: {
                text: 'Submission Statistics'
            },
            credits: {
              enabled: false
            },
            xAxis: {
              type: 'datetime'
            },
            yAxis: {
                title: {
                    text: 'Number'
                },
                labels: {
                    formatter: function() {
                        return this.value;
                    }
                }
            },
            tooltip: {
                formatter: function() {
                  var d=new Date();
                  d.setTime(this.x);
                    return d.getFullYear()+'/'+(d.getMonth()+1) + ': <br>' +this.y;
                }
            },
            plotOptions: {
                area: {
                    // dataLabels: {
                    //     enabled: true
                    // },
                    enableMouseTracking: true,
                    // pointStart: 200601,
                    marker: {
                        enabled: true,
                        symbol: 'circle',
                        radius: 4,
                        states: { hover: {
                                enabled: true
                            }
                        }
                    }
                }
            },
            series: [{
                name: 'Submission',
                data: [<?php foreach($submit_cnt as $row) echo "[Date.UTC(",$row[0],",",$row[1]-1,",1),",$row[2],"],"; ?>]
            },{
                name: 'Accepted',
                data: [<?php foreach($submit_cnt as $row) echo "[Date.UTC(",$row[0],",",$row[1]-1,",1),",$row[3],"],"; ?>]
            }]
        });
    });
    
});
    </script>

  </body>
</html>
