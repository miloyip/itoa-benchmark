<html>
<head>
<script src="http://code.jquery.com/jquery-1.8.2.js"></script>
<script src="http://www.google.com/jsapi?autoload={'modules':[{'name':'visualization','version':'1','packages':['controls', 'charteditor']}]}"></script>
<script src="http://jquery-csv.googlecode.com/git/src/jquery.csv.js"></script>
<link rel="stylesheet" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.2.0/css/bootstrap.min.css">
<link rel="stylesheet" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.2.0/css/bootstrap-theme.min.css">
<script src="http://maxcdn.bootstrapcdn.com/bootstrap/3.2.0/js/bootstrap.min.js"></script>
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

	$(".chart").each(function() {
		var chart = $(this);
		var d = $("#downloadDD").clone().css("display", "");
		$('li a', d).each(function() {
	        $(this).click(function() {
	            var svg = chart[0].getElementsByTagName('svg')[0].parentNode.innerHTML;
	            svg=sanitize(svg);
	            $('#imageFilename').val($("#title").html() + "_" + chart.data("filename"));
	            $('#imageGetFormTYPE').val($(this).attr('dltype'));
	            $('#imageGetFormSVG').val(svg);
	            $('#imageGetForm').submit();
	        });
	    });		
		$(this).after(d);
	});
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
	$(div).data("filename", type + "_time");
	$("#main").append(div);
	var chart = new google.visualization.BarChart(div);

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
	$(div).data("filename", type + "_timedigit");
	$("#main").append(div);
	var chart = new google.visualization.LineChart(div);

	chart.draw(data, options);
}

// http://jsfiddle.net/P6XXM/
function sanitize(svg) {
    svg = svg
        .replace(/\<svg/,'<svg xmlns="http://www.w3.org/2000/svg" version="1.1"')
        .replace(/zIndex="[^"]+"/g, '')
        .replace(/isShadow="[^"]+"/g, '')
        .replace(/symbolName="[^"]+"/g, '')
        .replace(/jQuery[0-9]+="[^"]+"/g, '')
        .replace(/isTracker="[^"]+"/g, '')
        .replace(/url\([^#]+#/g, 'url(#')
        .replace('<svg xmlns:xlink="http://www.w3.org/1999/xlink" ', '<svg ')
        .replace(/ href=/g, ' xlink:href=')
    /*.replace(/preserveAspectRatio="none">/g, 'preserveAspectRatio="none"/>')*/
    /* This fails in IE < 8
    .replace(/([0-9]+)\.([0-9]+)/g, function(s1, s2, s3) { // round off to save weight
    return s2 +'.'+ s3[0];
    })*/

    // IE specific
        .replace(/id=([^" >]+)/g, 'id="$1"')
        .replace(/class=([^" ]+)/g, 'class="$1"')
        .replace(/ transform /g, ' ')
        .replace(/:(path|rect)/g, '$1')
        .replace(/<img ([^>]*)>/gi, '<image $1 />')
        .replace(/<\/image>/g, '') // remove closing tags for images as they'll never have any content
        .replace(/<image ([^>]*)([^\/])>/gi, '<image $1$2 />') // closes image tags for firefox
        .replace(/width=(\d+)/g, 'width="$1"')
        .replace(/height=(\d+)/g, 'height="$1"')
        .replace(/hc-svg-href="/g, 'xlink:href="')
        .replace(/style="([^"]+)"/g, function (s) {
            return s.toLowerCase();
        });

    // IE9 beta bugs with innerHTML. Test again with final IE9.
    svg = svg.replace(/(url\(#highcharts-[0-9]+)&quot;/g, '$1')
        .replace(/&quot;/g, "'");
    if (svg.match(/ xmlns="/g).length == 2) {
        svg = svg.replace(/xmlns="[^"]+"/, '');
    }

    return svg;
}
</script>
<style type="text/css">
@media (min-width: 800px) {
  .container {
    max-width: 800px;
  }
}
textarea {
	font-family: Consolas, 'Liberation Mono', Menlo, Courier, monospace;
}
.tablechart {
	width: 700px;
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
<div class="container">
<div class="page-header">
<h1 id="title"><?=basename($argv[1], '.'.pathinfo($argv[1], PATHINFO_EXTENSION))?></h1>
</div>
<div id="main"></div>
<h2>Source CSV</h2>
<textarea id="textInput" class="form-control" rows="5" readonly>
<?php include $argv[1] ?>
</textarea>
</div>
<div class="row" id="downloadDD" style="display: none">
<div class="btn-group pull-right" >
    <button class="btn dropdown-toggle" data-toggle="dropdown"><span class="glyphicon glyphicon-picture"></span></button>
    <ul class="dropdown-menu">
        <li><a tabindex="-1" href="#" dltype="image/jpeg">JPEG</a></li>
        <li><a tabindex="-1" href="#" dltype="image/png">PNG</a></li>
        <li><a tabindex="-1" href="#" dltype="application/pdf">PDF</a></li>
        <li><a tabindex="-1" href="#" dltype="image/svg+xml">SVG</a></li>
    </ul>
</div>
</div>
<form method="post" action="http://export.highcharts.com/" id="imageGetForm">
    <input type="hidden" name="filename" id="imageFilename" value="" />
    <input type="hidden" name="type" id="imageGetFormTYPE" value="" />
    <input type="hidden" name="width" value="900" />
    <input type="hidden" name="svg" value="" id="imageGetFormSVG" />
</form>
</div>
</body>
</html>