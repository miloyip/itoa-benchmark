<html>
<head>
<script src="http://code.jquery.com/jquery-1.8.2.js"></script>
<script src="http://www.google.com/jsapi?autoload={'modules':[{'name':'visualization','version':'1','packages':['controls', 'charteditor']}]}"></script>
<script src="http://jquery-csv.googlecode.com/git/src/jquery.csv.js"></script>
<script>
$(function() {
	google.load("visualization", "1", {packages:["corechart"]});

    var csv = $('#textInput').val();
    var data = $.csv.toArrays(csv, {
        onParseValue: $.csv.hooks.castToScalar
    });
    //var s = JSON.stringify(data);
    //$("#parsed").html(s);

    var chartData = {};
    var funcColumnMap;

    for (var i = 1; i < data.length; i++) {
    	var type = data[i][0];
   		var func = data[i][1];
		var digit = data[i][2];
   		var time = data[i][3];

   		if (chartData[type] == null) {
   			chartData[type] = [["Digit"]];
   			funcColumnMap = {};
   		}

   		var table = chartData[type];

   		if (funcColumnMap[func] == null)
   			funcColumnMap[func] = table[0].push(func) - 1;

   		var row;
   		for (row = 1; row < table.length; row++)
   			if (table[row][0] == digit)
   				break;

    	if (row == table.length)
    		table.push([digit]);

		table[row][funcColumnMap[func]] = time;
	}

	for (var type in chartData)
		drawChart(type, chartData[type]);
});

function drawChart(type, chartData) {
	var data = google.visualization.arrayToDataTable(chartData);
	var options = { 
		title: type,
		chartArea: {'width': '70%', 'height': '80%'},
		hAxis: {
			title: "Digit",
			gridlines: { count: chartData.length - 1 },
			maxAlternation: 1,
			minTextSpacing: 0
		},
		vAxis: {
			title: "Time (ms)",
			logScale: true,
			minorGridlines: { count: 10 }
		},
		width: 800,
		height: 600
	};
	var div = document.createElement("div");
	$("#chart_div").append(div);
	var chart = new google.visualization.LineChart(div);
/*
	var a = document.createElement("a");
	$("#chart_div").append(a);
	google.visualization.events.addListener(chart, 'ready', function () {
        a.href = chart.getImageURI();
        a.innerHTML = "Download PNG";
    });
*/
	chart.draw(data, options);
}
</script>
</head>
<body>
<h1><?=basename($argv[1], '.'.pathinfo($argv[1], PATHINFO_EXTENSION))?></h1>
<div id="chart_div"></div>
<textarea id="textInput">
<?php include $argv[1] ?>
</textarea>
</body>
</html>