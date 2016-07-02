// Created by Sujay S. Phadke, 2016
// All Rights Reserved.
// Github: https://github.com/electronicsguy
//
// Read/Write to Google Sheets using a HTTPS GET/POST API.
// Can be used to with ESP8266 embedded IoT device.
// 
// doPost needs the spreadsheet ID, it has no concept of "active spreadsheet".

var ss = SpreadsheetApp.openById('1j5UX_r9JBG_qLsKYpLnlgqdZgXSkF1VC8L_mt7iAhgI');
var sheet = ss.getSheetByName('Sheet1');

function doPost(e) {
  var val = e.parameter.value;
  
  if (e.parameter.value !== undefined){
    var range = sheet.getRange('A1');
    range.setValue(val);
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
