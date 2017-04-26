# HTTPS Redirect (Version 2.0)
## A library for seamless data logging, communication and control for Internet of Things (focus: ESP8266, ESP32)
This library extends the *WiFiClientSecure* library ([Ivan-github](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h)). 
*HTTPSRedirect* uses the header information in the server's reply to a HTTP *GET* or *POST* request, and follows the re-direction URL by making another suitable *GET* request. In many cases, it is this re-directed URL which would present the final data that was required. 

**HTTPSRedirect implements a novel HTTP client with *follow-redirects* which can directly work with many web services (including Google Sheets, Calendar, Drive) without the need for 3rd party services like IFTTT, Temboo, Pushingbox, Thingspeak, Xively, etc.**

Version 2 of *HTTPSRedirect* has been completely rewritten for increased functionality, seamless library calls and maximizing performance.

## Sec. I Major features in Version 2
[enhancements]
  * Implements a detailed HTTP Client
  * Implements *GET* and *POST* requests according to HTTP/1.1 specificaton
  * Handles identity, chunked and raw transfer encoding in the response body
  * Generic enough to follow as many number of redirections as required
  * Generic enough to connect to any server, not just Google Docs
  * Various performance improvements
  
[bug fixes]
  * Correctly parses HTTP status codes and calculates transfer chunk sizes

## Sec. II *HTTPSRedirect* Library API
<p align="center"><img src="https://cdn.rawgit.com/electronicsguy/ESP8266/7d457c35/HTTPSRedirect/Extra/redirection.svg" width="400" height="200"></p>

*HTTPSRedirect* is generic enough to be used as a standard HTTP/SSL client, even when the server has no redirection. Redirection logic is explained in the figure above. In case of a server using redirection (ie: 'Location' field in the first response header), the library will automatically follow the target URL(s) till it hits the final endpoint for the final response.

### Sec. II(a) *HTTPSRedirect* Initialization:
Initialize a new *HTTPSRedirect* variable or object using either of these constructors:
```C++
HTTPSRedirect(void);
HTTPSRedirect(const int& p);
```
where *p* denotes the HTTPS port to be used (default is 443).

After initialization, use the method:
```C++
void setPrintResponseBody(bool);
```
to decide if you want the endpoint response body to be printed or not on the Serial output. If not printed to Serial output, it can be read using the method `getResponseBody()` described below.

### Sec. II(b) *HTTPSRedirect* methods:
A *GET* request can be made by calling the following methods:

```C++
bool GET(const String& url, const char* host);
bool GET(const String& url, const char* host, const bool& disp);
```
Arguments:
*host* specifies the target server hostname and *url* specifies the specific URL for that host.

A *POST* request can be made by calling the following methods:
```C++
bool POST(const String& url, const char* host, const String& payload);
bool POST(const String& url, const char* host, const String& payload, const bool& disp);
```
Arguments:
*host* specifies the target server hostname and *url* specifies the specific URL for that host. *payload* is a string containing the body of the POST request.

(Optional) *disp* (boolean) temporarily overrides the value set by `printResponseBody()`.

In both cases, the return value specifies if the request was made successfully or no. If an error occured, the final HTTP Status code and Reason phrase can be obtained via the methods:
```C++
int getStatusCode(void);
String getReasonPhrase(void);
```

If the request was successful, the endpoint response can be obtained as a string using the method:
```C++
String getResponseBody(void);
```
## Sec. III Features and Notes:
### Sec. III(a) Optimize Speed
Most of the string arguments are passed by reference (or char \*) to avoid inefficient calls to the copy constructor. In case of redirection, it is possible that a repeated operation needs to be made, which results in the same request being made to the endpoint everytime. In such cases, a special method is provided to speed up the process:
```C++
bool HTTPSRedirect::reConnectFinalEndpoint(void);
```
*reConnectFinalEndpoint*, as the name implies, will directly reconnect to the last endpoint stored from previous requests. It'll return the response as per the data provided by the server. Hence, all the steps to compute the endpoint are avoided, speeding up the response rate. You can disable this by commenting out the `#define OPTIMIZE_SPEED` declaration within HTTPSRedirect.h.

### Sec. III(b) Extra functions
These methods are not included by default to minimize the memory footprint:
```C++
void fetchBodyRaw(void);
void printHeaderFields(void);
```
To enable them, uncomment this declaration within HTTPSRedirect.h `#define EXTRA_FNS`.
Note: `fetchBodyRaw()` is still unimplemented. `printHeaderFields()` works in debugging mode (described below) to print some of the relevant header fields.

### Sec. III(c) Debugging
*HTTPSRedirect* supports a debugging mode for developers. This prints out extra debugging information to the Serial output when enabled. To enable, uncomment this line within DebugMacros.h: `#define DEBUG`.

### Sec. III(d) SSL Certificates
WiFiClientSecure supports TLS v1.2 and X.509 certificate fingerprint verification for remote servers. In most cases, you could get the remote server fingerprint using online services like [grc](https://www.grc.com/fingerprints.htm). However, in case of Google Docs, this will not work. Google serves HTTP client requests from different servers based on geographical location and time of day. Hence, any fingerprint obtained from say GRC, will be from GRC's client location. You won't get the right fingerprint since it needs to be fetched locally from your location, at this point in time. To get around this problem, you need to run *openssl* locally (on your computer) in Linux, to get the remote server's fingerprint. The command is as follows: (ref: [openssl s_client](http://askubuntu.com/questions/156620/how-to-verify-the-ssl-fingerprint-by-command-line-wget-curl/))
```bash
echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
```
 
Windows users can either use it from within a Linux virtual machine or Cygwin. Another alternative is to use pre-compiled binaries of openssl for Windows. These are available at: [gnuwin32-openssl](http://gnuwin32.sourceforge.net/packages/openssl.htm) or [fulgan-openssl](https://indy.fulgan.com/SSL/). Open up *cmd* within Windows, goto the directory containing the binary and run the above command. I've uploaded a Windows batch file(fingerprint.bat) that you can use to automate getting the fingerprint in the future.

(Note: For both Windows and Mac OS, remove replace the "|&" with just the pipe "|".)
Command syntax explanation here: [grep-pipe-ampersand](http://askubuntu.com/questions/24953/using-grep-with-pipe-and-ampersand-to-filter-errors-from-find)

This will fetch a fingerprint string. Put this string in the example main file *GoogleDocs.ino* (`const char* fingerprint`).
You can call the method *verify()* as shown in the example to perform fingerprint verification. However, this is not mandatory. HTTPSRedirect will function correctly even in case of a certificate mismatch (Unless there is some real MiTM attack going on). Also remember that in case of Google's servers, this fingerprint will only be valid from your physical location for a few hours/days. In case you want to use fingerprint verification with Google Docs, you will have to keep generating a new fingerprint every few hours/days.

---

# Sec. IV Working Example (Using Google Docs)
## Sec. IV(a) GoogleDocs.ino Arduino Script
With Google Apps Script, you can publish your Google Scripts on the web. These scripts can access a variety of Google services, 
like Gmail, Google Docs, Calendar, Drive, etc. Google requires all access to their services use HTTP over SSL/TLS. Hence the regular *WiFiClient* is not suitable and *WiFiClientSecure* must be used. However, *WiFiClientSecure* does not follow redirects and hence doesn't work directly with Google Docs.

When you publish your Google scripts, it'll be in the domain: **https://script.google.com/**. When you make a **GET** request to this
URL, the data will be processed by your Google script, as long as you have a *doGet()* function in there. This is what a **GET** request to Google script will look like:

```
GET https://script.google.com/macros/s/<script-id>/exec  
```

However, any data returned by *doGet()* is not passed on in the body of the response. This is what the initial response header
looks like:
```http
Access-Control-Allow-Origin: *
Alt-Svc: quic=":443"; ma=2592000; v="32,31,30,29,28,27,26,25"
Alternate-Protocol: 443:quic
Cache-Control: no-cache, no-store, max-age=0, must-revalidate
Content-Encoding: gzip
Content-Type: text/html; charset=UTF-8
Date: Mon, 25 Apr 2016 22:15:44 GMT
Expires: Mon, 01 Jan 1990 00:00:00 GMT
Location: https://script.googleusercontent.com/macros/echo?<random-data>
Pragma: no-cache
Transfer-Encoding: chunked
```

The HTTP status code will be **302 Moved Temporarily**.

Notice the field called **Location** in the response header. Even though we hit the script URL correctly (and passed on any parameters through the **GET** request), we will now need to follow this second link in order to get the return data. The second **GET** request has to be made to the domain: **https://script.googleusercontent.com/**. This URL is of the form:
```http
https://script.googleusercontent.com 
/macros/echo?
lib=<encrypted-library-key>
user_content_key=<long-random-data-key>
```
The value of *lib* will be the same for a given published script. *user_content_key* will change for every content served up by the doGet() or doPost() within apps script.
 
*HTTPSRedirect* follows this URL in a seamless fashion. It'll make both the **GET** requests and return the final data from the server in the response body.

Please check the **GoogleDocs** Arduino example included above, on how to use this library. The server-side code is in the file **GoogleScript.gs**, that you can use for your own spreadsheet. It also has an image of the test calendar whose entries are fetched by the above example. 

My copy of the spreadsheet can be found at: [spreadsheet](http://bit.ly/1Ql4qrN).

You can view the contents of cell 'A1', the data rows and the chart here. Any new data from your device will be appended at the end.

In order to view the return values from Google Docs, you need to open up a serial port terminal program (like CoolTerm) and connect to your IoT device (like ESP8266) from there. An easier option is to use the in-built "Serial Monitor" within Arduino itself. The advantage of this (over say Coolterm) is that it automatically disconnects when you recompile and re-flash, and then automatically reconnects to ESP8266. This saves a lot of effort in manually disconnecting and freeing up the serial port everytime we want to re-flash.
  
The Arduino example does the following:
  1. Makes a GET request to the script attached to the Google Spreadsheet, and write a value in the cell 'A1'.
  
  2. Make a GET request and fetches entries for the next 1 week from my Google calendar's test calendar.
  
  3. Make repeated GET requests to read the cell 'A1' of the spreadsheet. In this way, if you manually type something in the cell (within your own copy of the spreadsheet), you can *chat* with the ESP8266 :smile:

  4. Makes repeated POST requests and records the current value of the free heap and free stack sizes into the sheet. These are plotted in the chart. 
  
Following is a representative image of the spreadsheet. (Yours may not look like this if the data has been cleared. To generate a chart similar to this, create your own spreadsheet as explained in the next section, and let the device record around 200 rows of data.)
<p align="center"><img src="https://github.com/electronicsguy/ESP8266/blob/master/HTTPSRedirect/Extra/spreadsheet.jpg" width="600" height="300"></p>

The test calendar that I have in my Google Calendar account looks like this:
<p align="center"><img src="https://github.com/electronicsguy/ESP8266/blob/master/HTTPSRedirect/Extra/Test-REST-API-calendar.JPG" width="600" height="300"></p>

The data is fetched from my Spreadsheet and Google calendar. This is the Serial output:
<p align="center"><img src="https://github.com/electronicsguy/ESP8266/blob/master/HTTPSRedirect/Extra/output.jpg" width="600" height="300"></p>

---

## Sec. IV(b) Steps to get it working
You need to be familiar with using Arduino IDE in general and how to use the IDE to create a project and flash the ESP8266. Refer this to link for guidance: [programming-esp8266-arduino](https://create.arduino.cc/projecthub/Metavix/programming-the-esp8266-with-the-arduino-ide-601c16)

I tested the code on an ESP8266 model ESP-01 and ESP-01E. It should work on the other higher flash memeory versions as well (like ESP-12).

### Client side
1. Download the project files. Create a new project called 'GoogleDocs' within Arduino and load all the relevant files above.
2. Set the correct configuration parameters in the IDE: Board type: 'generic ESP8266',  Flash size, Upload speed etc. based on your board.
3. In GoogleDocs.ino, set the following: (a) Your wifi SSID and password  (b) Terminal UART speed in the line `Serial.begin(115200);`.
4. Compile and flash it onto the board. If all goes well and it is able to connect through your router to Google Docs, you should see the correct output in the terminal. Note: Some people have reported that it fails to connect randomly. This may be a problem with your router, DNS or firewall settings. In that case, please switch to another router/wireless access point.
5. As it stands, it will work *as-is with my copy* of the Spreadsheet and Google calendar. It will write to it and read from it. But you won't be able to modify it yourself. In order to create a copy of the sheet for yourself so that you can modify it, follow the steps for server side as described below.

### Server side
You need to be familiar with the basics of Javascript (on which Google-script is based) as well as the basics of using Google Apps Script editor to publish your own scripts. Refer to the guide here: [apps-script-tutorial](https://developers.google.com/apps-script/articles).

The spreadsheet (copyable) is here: [sample-spreadsheet](http://bit.ly/2og5Ldt). Create a copy of it in order to edit the contents. What you type in cell 'A1' will be reflected in the device Serial output.

You could also create your own calendar entries. Just make sure they repeat per week and change the calendar name appropriately in the `function GetEventsOneWeek()` within GoogleScript.gs.

The key steps for creating your own copy of the Spreadsheet and activating the script:
1. Create a new Google Spreadsheet in your account.
2. Goto Tools->Script editor... and copy all the code from *GoogleScript.gs* into the editor.
3. You can goto the menu item "ESP8266 Logging -> Clear" to clear all the row contents from the sheet.
4. Get your *spreadsheet-id*. If you look at the URL in the browser tab where the spreadsheet is open, it'll be of the form: 
`https://docs.google.com/spreadsheets/d/<random-string>/edit#gid=0`. The *\<random string>* is your *spreadsheet-id*. Put this value in *GoogleScript.gs* in the relevant line.
5. Publish your script by deploying it as a web app. The permissions should be set to "Execute the app as: Me (your email)" and "who has access to the app: Anyone, even anonymous". Once you publish your script, the script editor popup will say: "This project is now deployed as a web app.". It'll also display the URL for the webapp. Grab the string of random characters between "/macros/s/" and /"exec".

Example: If your URL looks like this: `https://script.google.com/macros/s/XXXYYY/exec`, then "XXXYYY" is your **GScriptId**. Put this value in GoogleDocs.ino for it to hit your script instead of mine.

**Important Note:** You need to re-publish your web-app (with a new version number) **every time** any change any made to your script. Google Apps script serves requests only with a published version of your script, not necessarily the *latest* one. This is an unfortunate Google Apps Script limitation. However, your *GScriptId* will remain the same.

6. Once these steps are completed, re-flash your ESP82666 with the new spreadsheet-id code. It should then read and write to your copy of the spreadsheet.

---
Note: HTTPSRedirect is free for personal, non-commercial use only. In case of use for profit or more than 10 devices per person, please contact the author for purchasing a licence.

I hope you enjoy using this library. Please try it out for yourself. Your comments and suggestions are most welcome.
If you like my work and would like to buy me some :coffee: :beer:, you can send your contributions here:
<p align="center"><a href="https://paypal.me/electronicsguy" target="_blank"><img src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif" /></a></p>

Users in India can also send using Paytm to my wallet: [paytm](http://bit.ly/2o1GDrC)
