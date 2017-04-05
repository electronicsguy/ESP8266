// Created by Sujay S. Phadke, 2017
// All Rights Reserved.
// Github: https://github.com/electronicsguy
//
// Read/Write to Google Sheets using REST API.
// Can be used with ESP8266 & other embedded IoT devices.
// 
// Use this file with the ESP8266 library HTTPSRedirect
// 
// doGet() and doPost() need the spreadsheet ID. Cannot use "active spreadsheet" here since
// the device can operate without the spreadsheet even being open.
// http://stackoverflow.com/questions/4024271/rest-api-best-practices-where-to-put-parameters
// http://trevorfox.com/2015/03/rest-api-with-google-apps-script

// Similar API docs:
// https://gspread.readthedocs.org/en/latest/
// https://smartsheet-platform.github.io/api-docs/#versioning-and-changes
// http://search.cpan.org/~jmcnamara/Excel-Writer-XLSX/lib/Excel/Writer/XLSX.pm

// http://forum.espruino.com/conversations/269510/
// http://stackoverflow.com/questions/34691425/difference-between-getvalue-and-getdisplayvalue-on-google-app-script
// http://ramblings.mcpher.com/Home/excelquirks/gooscript/optimize

// Things to remember with getValue() object format:
// 1. Partial dates or times-only will be replaced with a full date + time, probably in the
//    year 1989. Like this: Sat Dec 30 1899 08:09:00 GMT-0500 (EST)
// 2. Dollar ($) currency symbol will be absent if cell contains currency.
//    This may be locale-dependent.
// 3. Scientific notation will be replaced by decimal numbers like this: 0.0000055

// Script examples
// https://developers.google.com/adwords/scripts/docs/examples/spreadsheetapp

var SS = SpreadsheetApp.openById('1j5UX_r9JBG_qLsKYpLnlgqdZgXSkF1VC8L_mt7iAhgI');
var sheet = SS.getSheetByName('Sheet1');
var str = "";

function onOpen(){
  var ui = SpreadsheetApp.getUi();
  ui.createMenu('ESP8266 Logging')
  .addItem('Clear', 'Clear')
  .addToUi();
}

function Clear(){
  sheet.deleteRows(4, sheet.getLastRow());
  SS.toast('Chart cleared', 'ESP8266 logging', 5);
}

function doPost(e) {

  var parsedData;
  var result = {};
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }
   
  if (parsedData !== undefined){
    // Common items first
    // data format: 0 = display value(literal), 1 = object value
    var flag = parsedData.format;
    
    if (flag === undefined){
      flag = 0;
    }
    
    switch (parsedData.command) {
      case "appendRow":
         var tmp = SS.getSheetByName(parsedData.sheet_name);
         var nextFreeRow = tmp.getLastRow() + 1;
         var dataArr = parsedData.values.split(",");
         
         tmp.appendRow(dataArr);
         
         str = "Success";
         SpreadsheetApp.flush();
         break;     
       
       
    }
    
    return ContentService.createTextOutput(str);
  } // endif (parsedData !== undefined)
  
  else{
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
  
  
}


function doGet(e){
  
  var val = e.parameter.value;
  var cal = e.parameter.cal;
  var read = e.parameter.read;
  
  if (cal !== undefined){
    return ContentService.createTextOutput(GetEventsOneWeek());
  }
  
  if (read !== undefined){
    var now = Utilities.formatDate(new Date(), "EST", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11,19);
    sheet.getRange('D1').setValue(now);
    var count = (sheet.getRange('C1').getValue()) + 1;
    sheet.getRange('C1').setValue(count);
    return ContentService.createTextOutput(sheet.getRange('A1').getValue());
  }
  
  if (e.parameter.value === undefined)
    return ContentService.createTextOutput("No value passed as argument to script Url.");
    
  var range = sheet.getRange('A1');
  var retval = range.setValue(val).getValue();
  var now = Utilities.formatDate(new Date(), "EST", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11,19);
  sheet.getRange('B1').setValue(now);
  sheet.getRange('C1').setValue('0');
  
  if (retval == e.parameter.value)
    return ContentService.createTextOutput("Successfully wrote: " + e.parameter.value + "\ninto spreadsheet.");
  else
    return ContentService.createTextOutput("Unable to write into spreadsheet.\nCheck authentication and make sure the cursor is not on cell 'A1'." + retval + ' ' + e.parameter.value);
}

function GetEventsOneWeek(){
  var Cal = CalendarApp.getCalendarsByName('Test REST API')[0];
  // Need to create 2 separate Date() objects. Cannot do 'OneWeekFromNow = Nowjs' to 
  // simply get it's value and use that later without modifying 'Now'
  // since in JS, an object is automatically passed by reference
  var Now = new Date();
  var OneWeekFromNow = new Date();
  OneWeekFromNow.setDate(Now.getDate() + 7);
  //Logger.log(Now);
  //Logger.log(OneWeekFromNow);
  var events = Cal.getEvents(Now, OneWeekFromNow);
  //Logger.log(events.length);
  var str = '\nEvent Title,\tDescription,\tRecurring?,\tAll-day?,\tFirst Reminder (in minutes before event)\n';
  for (var i = 0; i < events.length; i++){
    str += events[i].getTitle() + ',\t' + events[i].getDescription() + ',\t' + events[i].isRecurringEvent() +  ',\t' + events[i].isAllDayEvent() + ',\t' + events[i].getPopupReminders()[0];
    str += '\n';
  }
  //Logger.log(str);
  return str;
}
