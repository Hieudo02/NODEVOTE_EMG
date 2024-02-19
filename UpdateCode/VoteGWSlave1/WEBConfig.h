//=========Chứa mã HTLM Website==//
#ifndef _HTML_H_
#define _HTML_H_
#include <Arduino.h>

const char MainpageHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta name="viewport" content="width=device-width,user-scalable=0" charset="UTF-8">		
    <script async src="code.js"></script>    
    <link rel="stylesheet" href="style.css">
		<title>IoTVision</title>
	</head>
	<body>
    <div style="text-align: center">
      <header>
                        <img src="data:image/png;base64,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" 
                        alt="Logo">
					</header>
    </div>
    <table style="width: 100%">
        <tr>
            <th colspan="2" style="text-align: center; padding: 5px; border-bottom: 0.5px solid #c8c8c8">Cấu hình MAC</th>
        </tr>
        <tr>
            <td style="text-align: right; padding: 5px; width: 35%">MAC của Master:</td>
            <td><input id="tbxMACMaster"/></td>
        </tr>
    </table>
    <div style="text-align: right; padding-right: 10px">   
        <button class="NutNhan" id="btnMACMaster" onclick="LuuMACMasterVaoEEPROM()">LƯU MACs</button>
    </div>   
    <div id="ThongBao"></div>
	</body>
<html>
)rawliteral";


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== Begin: STYLE CSS ===============================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
const char style_css[] PROGMEM = R"rawliteral(
body {
  background-color: #FFF;
  border: 0;
  margin: 0;
  font: 1em tahoma, arial, sans-serif;
}  
header {
  border-bottom: 1px solid #888;
  padding: 10px;
}
section {
  padding-top: 5px;
  border-bottom: 1px solid #888;
  border-top: 1px solid #888;
}
#ThongBao {
  border-top: 1px solid #888;
  text-align: left; 
  padding: 5px 0 5px 15px;
  font: 0.9em tahoma, arial, sans-serif;
  font-style: italic;
}
h1 {
color: #F07057;
}      
.NutNhan {
  height: 30px; 
  width: 130px; 
  margin: 10px 0;
  background-color: #F07057;
  border-radius: 3px;
  border: 1px solid #EEE;
}
.ape {
  margin-left: 15px;
  padding: 3px 0px;
  font: 0.9em tahoma, arial, sans-serif;
}
.ape:hover {
  cursor: pointer;
}
.w0 {
  background: url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBibGFjazsiLz4KPHBhdGggZD0iTTUsMTNMNywxNUM5Ljc2LDEyLjI0IDE0LjI0LDEyLjI0IDE3LDE1TDE5LDEzQzE1LjE0LDkuMTQgOC44Nyw5LjE0IDUsMTNaIiBzdHlsZT0iZmlsbDogYmxhY2s7Ii8+CjxwYXRoIGQ9Ik05LDE3TDEyLDIwTDE1LDE3QzEzLjM1LDE1LjM0IDEwLjY2LDE1LjM0IDksMTdaIiBzdHlsZT0iZmlsbDogYmxhY2s7Ii8+Cjwvc3ZnPgo=') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
.w1 {
  background:  url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNNSwxM0w3LDE1QzkuNzYsMTIuMjQgMTQuMjQsMTIuMjQgMTcsMTVMMTksMTNDMTUuMTQsOS4xNCA4Ljg3LDkuMTQgNSwxM1oiIHN0eWxlPSJmaWxsOiBibGFjazsiLz4KPHBhdGggZD0iTTksMTdMMTIsMjBMMTUsMTdDMTMuMzUsMTUuMzQgMTAuNjYsMTUuMzQgOSwxN1oiIHN0eWxlPSJmaWxsOiBibGFjazsiLz4KPC9zdmc+Cg==') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
.w2 {
  background:  url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNNSwxM0w3LDE1QzkuNzYsMTIuMjQgMTQuMjQsMTIuMjQgMTcsMTVMMTksMTNDMTUuMTQsOS4xNCA4Ljg3LDkuMTQgNSwxM1oiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNOSwxN0wxMiwyMEwxNSwxN0MxMy4zNSwxNS4zNCAxMC42NiwxNS4zNCA5LDE3WiIgc3R5bGU9ImZpbGw6IGJsYWNrOyIvPgo8L3N2Zz4K') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
.w3 {
  background:  url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNNSwxM0w3LDE1QzkuNzYsMTIuMjQgMTQuMjQsMTIuMjQgMTcsMTVMMTksMTNDMTUuMTQsOS4xNCA4Ljg3LDkuMTQgNSwxM1oiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNOSwxN0wxMiwyMEwxNSwxN0MxMy4zNSwxNS4zNCAxMC42NiwxNS4zNCA5LDE3WiIgc3R5bGU9ImZpbGw6IGdyYXk7Ii8+Cjwvc3ZnPgo=') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
)rawliteral";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== Begin: STYLE CSS ===============================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== Begin: CODE JAVASCRIPT =========================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
const char code_js[] PROGMEM = R"rawliteral(

const gel = (e) => document.getElementById(e);

function create_obj() {
    td = navigator.appName;
    if(td == "Microsoft Internet Explorer") {
        obj = new ActiveXObject("Microsoft.XMLHTTP");
    }
    else {
        obj = new XMLHttpRequest();
    }
    
    return obj;
}
var xhttp = create_obj();
//-----------------------------------------------------------------------------------------------------
async function LuuMACMasterVaoEEPROM() {
    var MACMaster = document.getElementById("tbxMACMaster").value;
    xhttp.open("GET","/LuuMACMasterVaoEEPROM?MACMaster=" + MACMaster, true);
    xhttp.onreadystatechange = KetQuaMACMaster;//nhận reponse 
    xhttp.send();
}
//--------------------------------Kiểm tra response -------------------------------------------------//
function KetQuaMACMaster(){
  if(xhttp.readyState == 4 && xhttp.status == 200){        
      //---------- Update data sử dụng javascript ----------//
      var res = xhttp.responseText;    
      gel("ThongBao").innerHTML = res;     
  }
}
//-----------------------------------------------------------------------------------------------------
)rawliteral";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== End: CODE JAVASCRIPT ===========================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

#endif