
function loadWeatherQueryScript() {
	$.ajax({
        type          : 'get',
        async         : false,
        url           : 'http://api.k780.com:88/?app=weather.today&weaid=1&appkey=23699&sign=267483f52f112d0794756d68035b1f7a&format=json&jsoncallback=data',
        dataType      : 'jsonp',
        jsonp         : 'callback',
        jsonpCallback : 'data',
        success       : function(data){
            if(data.success != '1'){
                alert(data.msgid + ' ' + data.msg);
                return;
            }
			console.log(data);
			var cur_tempe = "Temp:"
			cur_tempe += data.result.temperature_curr.replace(/\D/g, '');
			$("#utility-weather").text(cur_tempe);
        },
        error:function(){
            alert('fail');
        }
    });
	
	var t3 = setTimeout("loadWeatherQueryScript();", 600 * 1000);
}

function loadStockQueryScript() {
	$.ajax({
        type          : 'get',
        async         : true,
        url           : 'http://api.k780.com:88/?app=finance.stock_realtime&symbol=gb_tal&appkey=23699&sign=267483f52f112d0794756d68035b1f7a&format=json&jsoncallback=data',
        dataType      : 'jsonp',
        jsonp         : 'callback',
        jsonpCallback : 'data',
        success       : function(data){
			if (!data) {
				console.log(data);
				return;
			}
            if(data.success != '1'){
                alert(data.msgid + ' ' + data.msg);
                return;
            }
			console.log(data);
			var scode = data.result.lists.gb_tal.scode;
			var price = data.result.lists.gb_tal.last_price;
			$("#utility-stock").text(scode + ":$" + price);
			return;
        },
        error:function(e){
			console.log("failed");
			console.log(e);
			return;
        }
    });
	
	var t3 = setTimeout("loadStockQueryScript();", 600 * 1000);
}


