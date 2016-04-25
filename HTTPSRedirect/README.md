# HTTP Redirect for ESP8266

This library extends the *WiFiClientSecure* library, which is an amazing piece of work by Ivan Grokhotkov [Ivan-github](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h). 
*HTTPSRedirect* implements following the re-direction URL passed back to a *GET* request by the server, by making another suitable *GET*
 request automatically. In many cases, it is this re-directed URL which would present the final data that was required. 
 This is, for example, used by a number of Google services.

## Example, with reference specially to Google services
With Google Apps Script, you can publish your Google Scripts on the web. These scripts can access a variety of Google services, 
like Gmail, Google Docs, Calendar, Drive, etc. Google requires all access to their services use SSL/TLS. Hence the regular *WiFiClient* 
is not suitable and *WiFiClientSecure* must be used. Thanks to Ivan's work, we can now access Google services, using TLS 1.0/1.1 and 
with large certificates. However, *WiFiClientSecure* doesn't follow redirects.

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
  
  The main class is **HTTPSRedirect**, which has a method called **printRedir**. 
  
  Please check the GoogleDocs Arduino example on how to use this library.
  
