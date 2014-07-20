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

    // Convert data for bar chart (summing all digits)
    var timeData = {};	// type -> table
    var funcRowMap;

    for (var i = 1; i < data.length; i++) {
    	var type = data[i][0];
   		var func = data[i][1];
   		var time = data[i][3];
   		if (timeData[type] == null) {
   			timeData[type] = [["Function", "Time (ms)"/*, { role: "style" }*/]];
   			funcRowMap = {};
   		}

   		var table = timeData[type];
   		
   		if (funcRowMap[func] == null)
   			funcRowMap[func] = table.push([func, 0/*, defaultColors[table.length - 1]*/]) - 1;
   		
   		table[funcRowMap[func]][1] += time;
    }

    // Convert data for drawing line chart per digit
    var timeDigitData = {}; // type -> table
    var funcColumnMap;

    for (var i = 1; i < data.length; i++) {
    	var type = data[i][0];
   		var func = data[i][1];
		var digit = data[i][2];
   		var time = data[i][3];

   		if (timeDigitData[type] == null) {
   			timeDigitData[type] = [["Digit"]];
   			funcColumnMap = {};
   		}

   		var table = timeDigitData[type];

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

	for (var type in timeData) {
		var header = document.createElement("h2");
		header.innerHTML = type;
		$("#main").append(header);

		drawTable(type, timeData[type]);
		drawBarChart(type, timeData[type]);
		drawDigitChart(type, timeDigitData[type]);
	}
});

function drawTable(type, timeData) {
	var data = google.visualization.arrayToDataTable(timeData);
    data.addColumn('number', 'Speedup');
    data.sort([{ column: 1, desc: true }]);
    var formatter1 = new google.visualization.NumberFormat({ fractionDigits: 3 });
    formatter1.format(data, 1);

	var div = document.createElement("div");
	div.className = "tablechart";
	$("#main").append(div);
    var table = new google.visualization.Table(div);
    redrawTable(0);
    table.setSelection([{ row: 0, column: null}]);

    function redrawTable(selectedRow) {
        // Compute relative time using the first row as basis
        var basis = data.getValue(selectedRow, 1);
        for (var rowIndex = 0; rowIndex < data.getNumberOfRows(); rowIndex++)
            data.setValue(rowIndex, 2, basis / data.getValue(rowIndex, 1));

        var formatter = new google.visualization.NumberFormat({suffix: 'x'});
        formatter.format(data, 2); // Apply formatter to second column

        table.draw(data);
    }

    google.visualization.events.addListener(table, 'select',
    function() {
        var selection = table.getSelection();
        if (selection.length > 0) {
            var item = selection[0];
            if (item.row != null)
                redrawTable(item.row);
        }
    });

}

function drawBarChart(type, timeData) {
    var defaultColors = ["#3366cc","#dc3912","#ff9900","#109618","#990099","#0099c6","#dd4477","#66aa00","#b82e2e","#316395","#994499","#22aa99","#aaaa11","#6633cc","#e67300","#8b0707","#651067","#329262","#5574a6","#3b3eac","#b77322","#16d620","#b91383","#f4359e","#9c5935","#a9c413","#2a778d","#668d1c","#bea413","#0c5922","#743411"];

	var data = google.visualization.arrayToDataTable(timeData);
	data.addColumn({ type: "string", role: "style" })
	for (var rowIndex = 0; rowIndex < data.getNumberOfRows(); rowIndex++)
		data.setValue(rowIndex, 2, defaultColors[rowIndex]);

    data.sort([{ column: 1, desc: true }]);
	var options = { 
		title: type,
		chartArea: {'width': '70%', 'height': '70%'},
		width: 800,
		height: 300,
		legend: { position: "none" },
		hAxis: { title: "Time (ms)" }
	};
	var div = document.createElement("div");
	div.className = "chart";
	$("#main").append(div);
	var chart = new google.visualization.BarChart(div);
/*
	var a = document.createElement("a");
	$("#main").append(a);
	google.visualization.events.addListener(chart, 'ready', function () {
        a.href = chart.getImageURI();
        a.innerHTML = "Download PNG";
    });
*/
	chart.draw(data, options);
}

function drawDigitChart(type, timeDigitData) {
	var data = google.visualization.arrayToDataTable(timeDigitData);

	var options = { 
		title: type,
		chartArea: {'width': '70%', 'height': '80%'},
		hAxis: {
			title: "Digit",
			gridlines: { count: timeDigitData.length - 1 },
			maxAlternation: 1,
			minTextSpacing: 0
		},
		vAxis: {
			title: "Time (ms) in log scale",
			logScale: true,
			minorGridlines: { count: 10 }
		},
		width: 800,
		height: 600
	};
	var div = document.createElement("div");
	div.className = "chart";
	$("#main").append(div);
	var chart = new google.visualization.LineChart(div);
/*
	var a = document.createElement("a");
	$("#main").append(a);
	google.visualization.events.addListener(chart, 'ready', function () {
        a.href = chart.getImageURI();
        a.innerHTML = "Download PNG";
    });
*/
	chart.draw(data, options);
}
</script>
<style type="text/css">
body {
	color: #333333;
	font-family: Helvetica, arial, freesans, clean, sans-serif, 'Segoe UI Emoji', 'Segoe UI Symbol';
	font-size: 15px;
	font-style: normal;
	font-variant: normal;
	font-weight: normal;
	line-height: 25.5px;
}
h1 {
	font-size: 2em;
	line-height: 1.7;
	margin: 1em 0 15px;
	padding: 0;
	overflow: hidden;
}

h2 {
	border-top: 1px solid #eee;
	font-size: 1.5em;
	line-height: 1.7;
	margin: 1em 0 15px;
	padding: 0;
}
textarea {
	width: 800px;	
	font-family: Consolas, 'Liberation Mono', Menlo, Courier, monospace;
	font-size: 13px;
	font-style: normal;
	font-variant: normal;
	font-weight: normal;
	line-height: 19px;
}
#container {
	margin: 0 auto;
	width: 800px;
}
.tablechart {
	width: 560px;
	margin: auto;
	padding-top: 20px;
	padding-bottom: 20px;
}
.chart {
	padding-top: 20px;
	padding-bottom: 20px;
}
</style>
</head>
<body>
<div id="container">
<h1><?=basename($argv[1], '.'.pathinfo($argv[1], PATHINFO_EXTENSION))?></h1>
<div id="main"></div>
<h2>Source CSV</h2>
<textarea id="textInput" rows="5" readonly>
<?php include $argv[1] ?>
</textarea>
</div>
</body>
</html>