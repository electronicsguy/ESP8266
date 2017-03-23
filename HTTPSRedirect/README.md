# HTTPS Redirect for ESP8266 (Version 2.0)

This library extends the *WiFiClientSecure* library, which is an amazing piece of work by Ivan Grokhotkov ([Ivan-github](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h)). 
*HTTPSRedirect* uses the header information in the server's reply to a HTTP *GET* or *POST* request, and follows the re-direction URL by making another suitable *GET* request. In many cases, it is this re-directed URL which would present the final data that was required. 
This is, for example, used by a number of Google services.

Version 2 of *HTTPSRedirect* has been completely rewritten for increased functionality, seamless library calls and maximizing performance.

## Major features in Version 2
[enhancements]
* Implements a detailed HTTP Client
* Implements *GET* and *PUSH* requests according to HTTP/1.1 specificaton
* Handles raw and chunked encoding in the response body
* Generic enough to follow as many number of redirections as required
* Generic enough to connect to any server, not just Google Docs
* Various performance improvements
[bug fixes]
* Correctly parses HTTP status codes and calculates chunk sizes (bug from V1)

## *HTTPSRedirect* Library API
![redirection logic](https://github.com/electronicsguy/ESP8266/blob/master/HTTPSRedirect/Extra/redirection.jpg)

*HTTPSRedirect* is generic enough to use it as a standard HTTP/SSL client, even when the server has no redirection. Redirection logic is explained in the figure above. In case of a server using redirection (ie: 'Location' field in the first response header), the library will automatically follow the target URL(s) till it hits the final endpoint for the final response.

*HTTPSRedirect* Initialization:

Constructor:
```C++
HTTPSRedirect(const int& p)
```
*HTTPSRedirect* methods:
A *GET* request can be made by calling the following methods:

```C++
bool GET(const String& url, const char* host);
bool GET(const String& url, const char* host, const bool& disp);
```

Arguments:
*host* specifies the target server hostname and *url* specifies the specific URL for that host. 
(Optional) *disp* (boolean) temporarily overrides the value set by ```printResponseBody()```.

The return value specifies if the request was made successfully or no. If an error occured, the final HTTP Status code and Reason phrase can be obtained via the methods:
```C++
int getStatusCode(void);
String getReasonPhrase(void);
```

If the request was successful, the endpoint response can be obtained as a string using the method:
```C++
String getResponseBody(void);
```

## Optimize Speed

## Extra functions

## Working Example (Using Google Docs)
With Google Apps Script, you can publish your Google Scripts on the web. These scripts can access a variety of Google services, 
like Gmail, Google Docs, Calendar, Drive, etc. Google requires all access to their services use SSL/TLS. Hence the regular *WiFiClient* is not suitable and *WiFiClientSecure* must be used. Thanks to Ivan's work, we can now access Google services, using TLS 1.0/1.1 and 
with large certificates. However, *WiFiClientSecure* does not follow redirects.

When you publish your Google scripts, it'll be in the domain: **https://script.google.com/**. When you make a **GET** request to this
URL, the data will be processed by your Google script, as long as you have a *doGet()* function in there. This is what a **GET** request to
Google script will look like:

```
GET https://script.google.com/macros/s/<script-id>/exec  
```

However, any data returned by *doGet()* is not passed on in the body of the response. This is what the initial response header
looks like:
```
Access-Control-Allow-Origin: *
Alt-Svc: quic=":443"; ma=2592000; v="32,31,30,29,28,27,26,25"
Alternate-Protocol: 443:quic
Cache-Control: no-cache, no-store, max-age=0, must-revalidate
Content-Encoding: gzip
Content-Type: text/html; charset=UTF-8
Date: Mon, 25 Apr 2016 22:15:44 GMT
Expires: Mon, 01 Jan 1990 00:00:00 GMT
Location: https://script.googleusercontent.com/macros/<random url>
Pragma: no-cache
Transfer-Encoding: chunked
```

The HTTP error code will be **302 Moved Temporarily**.
Notice the field called **Location** in the response header. Even though we hit the script URL correctly (and passed on any parameters
 through the **GET** request), we will now need to follow this second link in order to get the return data. The second **GET** 
 request has to be made to the domain: **https://script.googleusercontent.com/**.
 
 *HTTPSRedirect* does precisely this, in a seamless fashion. It'll make both the **GET** requests and return the final data from the server
  in the response body. 
  
  The main class is **HTTPSRedirect**, which has a method called **printRedir**, which does most of the stuff.
  
  Please check the **GoogleDocs** Arduino example included above, on how to use this library. The *Extra* folder contains the Google Apps script that you can use for your own spreadsheet. It also has an image of the test calendar whose entries are fetched by the above example. The spreadsheet can be found at: [spreadsheet](http://bit.ly/1Ql4qrN).
  
  In order to view the return values from Google Docs, you need to open up a serial port terminal program (like CoolTerm) and connect to ESP8266 from there. An easier option is to use the in-built "Serial Monitor" within Arduino itself. The advantage of this (over say coolterm) is that it automatically disconnects when you recompile and re-flash, and then automatically reconnects to ESP8266. This saves a lot of effort in manually disconnecting and freeing up the serial port everytime we want to re-flash.
  
  The Arduino example does 3 things:
  1. Makes a request to the script attached to the Google Spreadsheet, and write a value in the cell 'A1'.
  
  2. Fetches entries for the next 1 week from my Google calendar's test calendar.
  
  3. Make repeated requests to read the cell 'A1' of the spreadsheet. In this way, if you manually type something in the cell, you can *chat* with the ESP8266 :smile:
  
  Comments and suggestions welcome.
  
