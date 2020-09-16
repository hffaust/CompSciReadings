The Web Application Hackers Handbook Chapters 1-3

# Chapter 1: Web Application (In)security

- Broken Authentication
	- This category of vulnerability encom- passes various defects within the application’s login mechanism, which may enable an attacker to guess weak passwords, launch a brute-force attack, or bypass the login.
- Broken Access Controls
	- This involves cases where the application fails to properly protect access to its data and functionality, potentially enabling an attacker to view other users’ sensitive data held on the server or carry out privileged actions.
- SQL Injection 
	- This vulnerability enables an attacker to submit crafted input to interfere with the application’s interaction with back-end databases. An attacker may be able to retrieve arbitrary data from the application, interfere with its logic, or execute commands on the database server itself.
- Cross-Site Scripting
	- This vulnerability enables an attacker to target other users of the application, potentially gaining access to their data, performing unauthorized actions on their behalf, or carrying out other attacks against them.
- Information Leakage
	- This involves cases where an application divulges sensitive information that is of use to an attacker in developing an assault against the application, through defective error handling or other behavior.
- Cross-Site Request Forgery
	- This flaw means that application users can be induced to perform unintended actions on the application within their user context and privilege level. The vulnerability allows a malicious web site visited by the victim user to interact with the applica- tion to perform actions that the user did not intend.

## The Core Security Proble: Users Can Submit Arbitrary Input
- This core problem manifests itself in various ways:
	- Users can interfere with any piece of data transmitted between the client and the server, including request parameters, cookies, and HTTP head- ers. Any security controls implemented on the client side, such as input validation checks, can be easily circumvented.
	- Users can send requests in any sequence and can submit parameters at a different stage than the application expects, more than once, or not at all. Any assumption developers make about how users will interact with the application may be violated.
	- Users are not restricted to using only a web browser to access the application. Numerous widely available tools operate alongside, or independently of, a browser to help attack web applications. These tools can make requests that no browser would ordinarily make and can generate huge numbers of requests quickly to find and exploit problems.
***
# Chapter 2: Core Defense Mechanisms
	
## Handling User Access
### Authentication
- Most basic authentication methods take the form of usernames and passwords to login to an account and thus identify the user. 
	- Without this facility, the application would need to treat all users as anonymous — the lowest possible level of trust.
- In security-critical applications, such as those used by online banks, this basic model is usually supplemented by additional credentials and a multistage login process. 
- When security requirements are higher still, other authentication models may be used, based on client certificates, smartcards, or challenge-response tokens. In addition to the core login process, authentication mechanisms often employ a range of other supporting functionality, such as self-registration, account recovery, and a password change facility.


### Session Management
- The web application being accessed is receiving a series of HTTP requests from the user, as well as all other users simultaneously accessing the application. To keep track of where each request is coming from web applications establish a session with each user.
- The session itself is a set of data structures held on the server that track the state of the user’s interaction with the application. 
- The token is a unique string that the application maps to the session.
	- When a user receives a token, the browser automatically submits it back to the server in each subsequent HTTP request, enabling the application to associate the request with that user. 
	- HTTP cookies are the standard method for transmitting session tokens, although many applications use hidden form fields or the URL query string for this purpose.
	-  If a user does not make a request for a certain amount of time, the session is ideally expired.
- The majority of attacks against session management tokens seek to compromise the tokens issued to other users.
	- If this is possible, an attacker can masquerade as the victim user and use the application just as if he had actually authenticated as that user. 
	- The principal areas of vulnerability arise from: 
		1. Defects in how tokens are generated, enabling an attacker to guess the tokens issued to other users,  
		2. Defects in how tokens are subsequently handled, enabling an attacker to capture other users’ tokens.
- A small number of applications dispense with the need for session tokens by using other means of reidentifying users across multiple requests. 
	- If HTTP’s built-in authentication mechanism is used, the browser automatically resubmits the user’s credentials with each request, enabling the application to identify the user directly from these. 
	- In other cases, the application stores the state infor- mation on the client side rather than the server, usually in encrypted form to prevent tampering.

### Access Controls
- The access control mechanism usually needs to implement some fine-grained logic, with different considerations being relevant to different areas of the application and different types of functionality. It gives different levels of "access" to different users depending upon their identity. 


***
## Handling User Input
### Approaches to Input Handling

#### "Reject Know Bad"
- This approach typically employs a blacklist containing a set of literal strings or patterns that are known to be used in attacks. The validation mechanism blocks any data that matches the blacklist and allows everything else.
- In general, this is regarded as the least effective approach to validating user input, for two main reasons:
	1. First, a typical vulnerability in a web application can be exploited using a wide variety of input, which may be encoded or represented in various ways. Except in the simplest of cases, it is likely that a blacklist will omit some patterns of input that can be used to attack the application. 
	2. Second, techniques for exploitation are constantly evolving. Novel methods for exploiting existing categories of vulnerabilities are unlikely to be blocked by current blacklists.
- Examples of Bypassing with Ease:
	- If `SELECT` is blocked, try `SeLeCt`
	- If or `1=1--` is blocked, try or `2=2--`
	- If `alert(‘xss’)` is blocked, try `prompt(‘xss’)`
- Can bypass filters designed to block specific keywords by using nonstandard characters between expressions to disrupt the tokenizing performed by the application.
	- Additionally, NULL byte attacks sometimes allow for a filter bypass

#### "Accept Known Good"
- This approach employs a whitelist containing a set of literal strings or patterns, or a set of criteria, that is known to match only benign input. The validation mechanism allows data that matches the whitelist and blocks everything else.
- In cases where this approach is feasible, it is regarded as the most effective way to handle potentially malicious input.

#### Sanitization
- This approach recognizes the need to sometimes accept data that cannot be guaranteed as safe. Instead of rejecting this input, the application sanitizes it in various ways to prevent it from having any adverse effects. 
	- Potentially malicious characters may be removed from the data, leaving only what is known to be safe, or they may be suitably encoded or “escaped” before further processing is performed.

#### Safe Data Handling
- Many web application vulnerabilities arise because user-supplied data is processed in unsafe ways. 
	- Vulnerabilities often can be avoided, not by validating the input itself, but by ensuring that the processing that is performed on it is inherently safe. 
	- In some situations, safe programming methods are available that avoid common problems. 
		- For example, SQL injection attacks can be pre- vented through the correct use of parameterized queries for database access.
- This approach cannot be applied to every kind of task that web applications need to perform. But where it is available, it is an effective general approach to handling potentially malicious input.


#### Semantic Checks
- With some vulnerabilities the input supplied by the attacker is identical to the input that an ordinary, nonmalicious user may submit. What makes it malicious is the different circumstances under which it is submitted. 
	- For example, an attacker might seek to gain access to another user’s bank account by changing an account number transmitted in a hidden form field. 
- Semantic Checks make sure that items submitted "belongs" to the user who submits them.

#### Boundary Validation
- Each individual component or functional unit of the server-side application treats its inputs as coming from a potentially malicious source. 
	- Data validation is performed at each of these trust boundaries, in addition to the external frontier between the client and server.

### Multistep Validation and Canonicalization
- A common problem encountered by input-handling mechanisms arises when user-supplied input is manipulated across several steps as part of the validation logic. 
	- If this process is not handled carefully, an attacker may be able to construct crafted input that succeeds in smuggling malicious data through the validation mechanism.
	- Example:
		- The defense mechanisms may strip the `<script>` tag from user inputs, however, an input such as `<scr<script>ipt>` would be stripped and contracted to restore the malicious payload **because the filter is not eing applied recursively**.
		- Similarly, if more than one validation step is performed on user input, an attacker may be able to exploit the ordering of these steps to bypass the filter. For example, if the application first removes `../` recursively and then removes `..\` recursively, the following input can be used to defeat the validation: `....\/`
- *Canonicalization* is the process of converting or decoding data into a common character set. 
	- If any canonicalization is carried out after input filters have been applied, an attacker may be able to use a suitable encoding scheme to bypass the validation mechanism.
- In addition to the standard encoding schemes that are intended for use in web applications, canonicalization issues can arise in other situations where a component employed by the application converts data from one character set to another. 
	- For example, some technologies perform a “best fit” mapping of characters based on similarities in their printed glyphs. Here, the characters « and » may be converted into < and >, respectively, and Ÿ and Â are converted into Y and A. This behavior can often be leveraged to smuggle blocked characters or keywords past an application’s input filters.
***
## Handling Attacks
### Handling Errors
- A key defense mechanism is for the application to handle unexpected errors gracefully, and either recover from them or present a suitable error message to the user. 
- **In a production context, the application should never return any system-generated messages or other debug information in its responses.** 
	- In some situations, an attacker can leverage defective error handling to retrieve sensitive information within the error messages themselves, providing a valuable channel for stealing data from the application.
- Effective error handling is often integrated with the application’s logging mechanisms, which record as much debug information as possible about unanticipated errors.

### Maintaining Audit Logs
- Following such an incident, effective audit logs should enable the application’s owners to understand exactly what has taken place, which vulnerabilities (if any) were exploited, whether the attacker gained unauthorized access to data or performed any unauthorized actions, and, as far as possible, provide evidence of the intruder’s identity.
- In any application for which security is important, key events should be logged as a matter of course. At a minimum, these typically include the following:
	- All events relating to the authentication functionality, such as successful and failed login, and change of password
	- Key transactions, such as credit card payments and funds transfers
	- Access attempts that are blocked by the access control mechanisms
	- Any requests containing known attack strings that indicate overtly malicious intentions
- Effective audit logs typically record the time of each event, the IP address from which the request was received, and the user’s account (if authenticated). 
	- Such logs need to be strongly protected against unauthorized read or write access. 
	- An effective approach is to store audit logs on an autonomous system that accepts only update messages from the main application.
	- In some situations, logs may be flushed to write-once media to ensure their integrity in the event of a successful attack.
- In terms of attack surface, poorly protected audit logs can provide a gold mine of information to an attacker, disclosing a host of sensitive information such as session tokens and request parameters. 

### Alerting Administration
- Anomalous events monitored by alerting mechanisms often include the following:
	- Usage anomalies, such as large numbers of requests being received from a single IP address or user, indicating a scripted attack
	- Business anomalies, such as an unusual number of funds transfers being made to or from a single bank account
	- Requests containing known attack strings
	- Requests where data that is hidden from ordinary users has been modified
- In any security-critical application, the most effective way to implement real-time alerting is to integrate this tightly with the application’s input validation mechanisms and other controls.

### Reacting to Attacks
- In addition to alerting administrators, many security-critical applications con- tain built-in mechanisms to react defensively to users who are identified as potentially malicious.

* * *
# Chapter 3: Web Application Technologies
## The HTTP Protocol
- HTTP uses a message-based model in which a client sends a request message and the server returns a response message. 
- The protocol is essentially connectionless: although HTTP uses the stateful TCP protocol as its transport mechanism, each exchange of request and response is an autonomous transaction and may use a different TCP connection.

### HTTP Requests
- All HTTP messages (requests and responses) consist of one or more headers, each on a separate line, followed by a mandatory blank line, followed by an optional message body. 
- Typical HTTP Request:
	```
	GET /auth/488/YourDetails.ashx?uid=129 HTTP/1.1
	Accept: application/x-ms-application, image/jpeg, application/xaml+xml,
	image/gif, image/pjpeg, application/x-ms-xbap, application/x-shockwave-
	flash, */*
	Referer: https://mdsec.net/auth/488/Home.ashx
	Accept-Language: en-GB
	User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64;
	Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR
	3.0.30729; .NET4.0C; InfoPath.3; .NET4.0E; FDM; .NET CLR 1.1.4322)
	Accept-Encoding: gzip, deflate
	Host: mdsec.net
	Connection: Keep-Alive
	Cookie: SessionId=5B70C71F3FD4968935CDB6682E545476
	```
	- The first line of every HTTP request consists of three items, separated by spaces:
		1. A verb indicating the HTTP method. The most commonly used method is GET, whose function is to retrieve a resource from the web server. GET requests do not have a message body, so no further data follows the blank line after the message headers.
		2. The requested URL. The URL typically functions as a name for the resource being requested, together with an optional query string containing param- eters that the client is passing to that resource. The query string is indicated by the ? character in the URL. The example contains a single parameter with the name uid and the value 129.
		3. The HTTP version being used. The only HTTP versions in common use on the Internet are 1.0 and 1.1, and most browsers use version 1.1 by default. There are a few differences between the specifications of these two versions; however, the only difference you are likely to encounter when attacking web applications is that in version 1.1 the Host request header is mandatory.
	- Other points of interest:
		- The `Referer` header is used to indicate the URL from which the request originated (for example, because the user clicked a link on that page). Note that this header was misspelled in the original HTTP specification, and the misspelled version has been retained ever since.
		- The `User-Agent` header is used to provide information about the browser or other client software that generated the request. Note that most browsers include the Mozilla prefix for historical reasons. This was the User-Agent string used by the originally dominant Netscape browser, and other browsers wanted to assert to websites that they were compatible with this standard. As with many quirks from computing history, it has become so established that it is still retained, even on the current version of Internet Explorer, which made the request shown in the example.
		- The `Host` header specifies the hostname that appeared in the full URL being accessed. This is necessary when multiple websites are hosted on the same server, because the URL sent in the first line of the request usu- ally does not contain a hostname.
		- The `Cookie` header is used to submit additional parameters that the server has issued to the client.

### HTTP Responses
- Typical HTTP Response:
	```
	  HTTP/1.1 200 OK
	  Date: Tue, 19 Apr 2011 09:23:32 GMT
	  Server: Microsoft-IIS/6.0
	  X-Powered-By: ASP.NET
	  Set-Cookie: tracking=tI8rk7joMx44S2Uu85nSWc
	  X-AspNet-Version: 2.0.50727
	  Cache-Control: no-cache
	  Pragma: no-cache
	  Expires: Thu, 01 Jan 1970 00:00:00 GMT
	  Content-Type: text/html; charset=utf-8
	  Content-Length: 1067
	  <!DOCTYPE html PUBLIC “-//W3C//DTD XHTML 1.0 Transitional//EN” “http://
	  www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd”><html xmlns=”http://
	  www.w3.org/1999/xhtml” ><head><title>Your details</title>
	  ...
	```
	- The first line of every HTTP response consists of three items, separated by spaces:
		1. The HTTP version being used.
		2. A numeric status code indicating the result of the request. 200 is the most common status code; it means that the request was successful and that the requested resource is being returned.
		3. A textual “reason phrase” further describing the status of the response. This can have any value and is not used for any purpose by current browsers.
	- Other points of interest:
		- The `Server` header contains a banner indicating the web server software being used, and sometimes other details such as installed modules and the server operating system. The information contained may or may not be accurate.
		- The `Set-Cookie` header issues the browser a further cookie; this is submitted back in the `Cookie` header of subsequent requests to this server.
		- The `Pragma` header instructs the browser not to store the response in its cache. 
			- The `Expires` header indicates that the response content expired in the past and therefore should not be cached. These instructions are frequently issued when dynamic content is being returned to ensure that browsers obtain a fresh version of this content on subsequent occasions.
		- Almost all HTTP responses contain a message body following the blank line after the headers. The `Content-Type` header indicates that the body of this message contains an HTML document.
		- The `Content-Length` header indicates the length of the message body in bytes.

### HTTP Methods

- `HEAD` functions in the same way as a GET request, except that the server should not return a message body in its response. The server should return the same headers that it would have returned to the corresponding GET request. Hence, this method can be used to check whether a resource is present before making a GET request for it.
- `TRACE` is designed for diagnostic purposes. The server should return in the response body the exact contents of the request message it received. This can be used to detect the effect of any proxy servers between the client and server that may manipulate the request.
- `OPTIONS` asks the server to report the HTTP methods that are available for a particular resource. The server typically returns a response containing an `Allow` header that lists the available methods.
- `PUT` attempts to upload the specified resource to the server, using the content contained in the body of the request. If this method is enabled, you may be able to leverage it to attack the application, such as by uploading an arbitrary script and executing it on the server.

### URLs
- A uniform resource locator (URL) is a unique identifier for a web resource through which that resource can be retrieved. 
- The format of most URLs is as follows: `protocol://hostname[:port]/[path/]file[?param=value]`

### REST
- Representational state transfer (REST) is a style of architecture for distributed systems in which requests and responses contain representations of the current state of the system’s resources. 
	- The core technologies employed in the World Wide Web, including the HTTP protocol and the format of URLs, conform to the REST architectural style.
- Although URLs containing parameters within the query string do themselves conform to REST constraints, the term “REST-style URL” is often used to signify a URL that contains its parameters within the URL file path, rather than the query string. 
- For example: 
	- The following URL containing a query string:
	`http://wahh-app.com/search?make=ford&model=pinto`
	- The above URL can be represented in the same way but while containing “REST-style” parameters like so:
	`http://wahh-app.com/search/ford/pinto`

## HTTP Headers
- HTTP supports a large number of headers, some of which are designed for specific unusual purposes. Some headers can be used for both requests and responses, and others are specific to one of these message types. The following sections describe the headers you are likely to encounter when attacking web applications.

### General Headers
- `Connection` tells the other end of the communication whether it should close the TCP connection after the HTTP transmission has completed or keep it open for further messages.
- `Content-Encoding` specifies what kind of encoding is being used for the content contained in the message body, such as `gzip`, which is used by some applications to compress responses for faster transmission.
- `Content-Length` specifies the length of the message body, in bytes (except in the case of responses to HEAD requests, when it indicates the length of the body in the response to the corresponding GET request).
- `Content-Type` specifies the type of content contained in the message body, such as `text/html` for HTML documents.
- `Transfer-Encoding` specifies any encoding that was performed on the message body to facilitate its transfer over HTTP. It is normally used to specify chunked encoding when this is employed.

### Request Headers
- `Accept` tells the server what kinds of content the client is willing to accept, such as image types, office document formats, and so on.
- `Accept-Encoding` tells the server what kinds of content encoding the client is willing to accept.
- `Authorization` submits credentials to the server for one of the built-in HTTP authentication types.
- `Cookie` submits cookies to the server that the server previously issued. 
- `Host` specifies the hostname that appeared in the full URL being requested.
- `If-Modified-Since` specifies when the browser last received the requested resource. If the resource has not changed since that time, the server may instruct the client to use its cached copy, using a response with status code 304.
- `If-None-Match` specifies an **entity tag**, which is an identifier denoting the contents of the message body. The browser submits the entity tag that the server issued with the requested resource when it was last received. The server can use the entity tag to determine whether the browser may use its cached copy of the resource.
- `Origin` is used in cross-domain Ajax requests to indicate the domain from which the request originated (see Chapter 13).
- `Referer` specifies the URL from which the current request originated.
- `User-Agent` provides information about the browser or other client software that generated the request.

### Response Headers
- `Access-Control-Allow-Origin` indicates whether the resource can be retrieved via cross-domain Ajax requests (see Chapter 13).
- `Cache-Control` passes caching directives to the browser (for example, `no-cache`).
- `ETag` specifies an entity tag. Clients can submit this identifier in future requests for the same resource in the `If-None-Match` header to notify the server which version of the resource the browser currently holds in its cache.
- `Expires` tells the browser for how long the contents of the message body are valid. The browser may use the cached copy of this resource until this time.
- `Location` is used in redirection responses (those that have a status code starting with 3) to specify the target of the redirect.
- `Pragma` passes caching directives to the browser (for example, `no-cache`).
- `Server` provides information about the web server software being used.
- `Set-Cookie` issues cookies to the browser that it will submit back to the server in subsequent requests.
- `WWW-Authenticate` is used in responses that have a 401 status code to provide details on the type(s) of authentication that the server supports.
- `X-Frame-Options` indicates whether and how the current response may be loaded within a browser frame (see Chapter 13).


### Cookies
- The cookie mechanism enables the server to send items of data to the client, which the client stores and resubmits to the server. 
	- Unlike the other types of request parameters (those within the URL query string or the message body), cookies continue to be resubmitted in each subsequent request without any particular action required by the application or the user.
- A server issues a cookie using the Set-Cookie response header, as you have seen: `Set-Cookie: tracking=tI8rk7joMx44S2Uu85nSWc`
	- The user’s browser then automatically adds the following header to subsequent requests back to the same server: `Cookie: tracking=tI8rk7joMx44S2Uu85nSWc`
- Cookies normally consist of a name/value pair, as shown, but they may consist of any string that does not contain a space. 
- Multiple cookies can be issued by using multiple `Set-Cookie` headers in the server’s response. 
	These are submitted back to the server in the same `Cookie` header, with a semicolon separating different individual cookies.
	
#### `Set-Cookie` additional options:
- `expires` sets a date until which the cookie is valid. This causes the browser to save the cookie to persistent storage, and it is reused in subsequent browser sessions until the expiration date is reached. If this attribute is not set, the cookie is used only in the current browser session.
- `domain` specifies the domain for which the cookie is valid. This must be the same or a parent of the domain from which the cookie is received.
- `path` specifies the URL path for which the cookie is valid.
- `secure` — If this attribute is set, the cookie will be submitted only in HTTPS
requests.
- `HttpOnly` — If this attribute is set, the cookie cannot be directly accessed via client-side JavaScript.


### Status Codes
- 1xx - Informational.
	- `100 Continue` is sent in some circumstances when a client submits a request containing a body. The response indicates that the request headers were received and that the client should continue sending the body. The server returns a second response when the request has been completed.
- 2xx - The request was successful.
	- `200 OK` indicates that the request was successful and that the response body contains the result of the request.
	- `201 Created` is returned in response tto a `PUT` request to indicate that the request was successful.
- 3xx - The client is redirected to a different resource.
	- `301 Moved Permanetly` redirects the browser permanently to a different URL, which is specified in the `Location` header. The client should use the new URL in the future rather than the original.
	- `302 Found` redirects the browser temporarily to a different URL, which is specified in the `Location` header. The client should revert to the original URL in subsequent requests.
	- `304 Not Modified` instructs the browser to use its cached copy of the requested resource. The server uses the `If-Modified-Since` and `If-None-Match` request headers to determine whether the client has the latest version of the resource.
- 4xx - The request contains an error of some kind.
	- `400 Bad Request` indicates that the client submitted an invalid HTTP request. You will probably encounter this when you have modified a request in certain invalid ways, such as by placing a space character into the URL.
	- `401 Unauthorized` indicates that the server requires HTTP authentication before the request will be granted. The `WWW-Authenticate` header contains details on the type(s) of authentication supported.
	- `403 Forbidden` indicates that no one is allowed to access the requested resource, regardless of authentication.
	- `404 Not Found` indicates that the requested resource does not exist.
	- `405 Method Not Allowed` indicates that the method used in the request is not supported for the specified URL. For example, you may receive this status code if you attempt to use the `PUT` method where it is not supported.
	- `413 Request Entity Too Large` — If you are probing for buffer overflow vulnerabilities in native code, and therefore are submitting long strings of data, this indicates that the body of your request is too large for the server to handle.
	- `414 Request URI Too Long` is similar to the 413 response. It indicates that the URL used in the request is too large for the server to handle.
- 5xx - The server encountered an error fulfilling the request.
	- `500 Internal Server Error` indicates that the server encountered an error fulfilling the request. This normally occurs when you have submitted unexpected input that caused an unhandled error somewhere within the application’s processing. You should closely review the full contents of the server’s response for any details indicating the nature of the error.
	- `503 Service Unavailable` normally indicates that, although the web server itself is functioning and can respond to requests, the application accessed via the server is not responding. You should verify whether this is the result of any action you have performed.

### HTTPS
- The HTTP protocol uses plain TCP as its transport mechanism, which is unencrypted and therefore can be intercepted by an attacker who is suitably positioned on the network. 
- HTTPS is essentially the same application-layer protocol as HTTP but is tunneled over the secure transport mechanism, Secure Sockets Layer (SSL).
	- **Note:** SSL has strictly been superseded by transport layer security (TLS), but the latter usually still is referred to using the older name.

### HTTP Proxies
- An HTTP proxy is a server that mediates access between the client browser and the destination web server. 
- When a browser has been configured to use a proxy server, it makes all its requests to that server. The proxy relays the requests to the relevant web servers and forwards their responses back to the browser. 
	- Most proxies also provide additional services, including caching, authentication, and access control. 
- *2 differences in how HTTP works when a proxy server is being used:*
	1. When a browser issues an unencrypted HTTP request to a proxy server, it places the full URL into the request, including the protocol prefix `http://`, the server’s hostname, and the port number if this is nonstandard. The proxy server extracts the hostname and port and uses these to direct the request to the correct destination web server.
	2. When HTTPS is being used, the browser cannot perform the SSL handshake with the proxy server, because this would break the secure tunnel and leave the communications vulnerable to interception attacks. Hence, the browser must use the proxy as a pure TCP-level relay, which passes all network data in both directions between the browser and the destination web server, with which the browser performs an SSL handshake as normal. 
		- To establish this relay, the browser makes an HTTP request to the proxy server using the `CONNECT` method and specifying the destination hostname and port number as the URL. If the proxy allows the request, it returns an HTTP response with a 200 status, keeps the TCP connection open, and from that point onward acts as a pure TCP-level relay to the destination web server.

### HTTP Authentication
- The HTTP protocol includes its own mechanisms for authenticating users using various authentication schemes, including the following:
	- **Basic** is a simple authentication mechanism that sends user credentials as a Base64-encoded string in a request header with each message.
	- **NTLM** is a challenge-response mechanism and uses a version of the Windows NTLM protocol.
	- **Digest** is a challenge-response mechanism and uses MD5 checksums of a nonce with the user’s credentials.
- **Note:** It is relatively rare to encounter these authentication protocols being used by web applications deployed on the Internet. They are more commonly used within organizations to access intranet-based services.


## Web Functionality
### Server-Side Functionality
- Dynamic content is generated by scripts or other code executing on the server. These scripts are akin to computer programs in their own right. They have various inputs, perform processing on these, and return their outputs to the user.
- Generally, when a user's browser requests a dynamic resource, it also submits various parameters along with its request to generate content tailored to the individual user. 
	- HTTP Requests can be used to send parameter to the application (mainly) in the following ways:
		- In the URL query string
		- In the file path of REST-style URLs
		- in HTTP cookies
		- In the body of requests using the `POST` method
	- In addition to these primary sources of input, the server-side application may in principle use any part of the HTTP request as an input to its processing. 
		- For example, an application may process the `User-Agent` header to generate content that is optimized for the type of browser being used.

### Web Application Platforms
#### The Java Platform
- For many years, the Java Platform, Enterprise Edition (formerly known as J2EE) was a de facto standard for large-scale enterprise applications. 
- Originally developed by Sun Microsystems and now owned by Oracle, it lends itself to multitiered and load-balanced architectures and is well suited to modular development and code reuse. 
- Descriptions of Java-based web applications often employ a number of potentially confusing terms that you may need to be aware of:
	- An Enterprise Java Bean (EJB) is a relatively heavyweight software component that encapsulates the logic of a specific business function within the application. EJBs are intended to take care of various technical challenges that application developers must address, such as transactional integrity.
	- A Plain Old Java Object (POJO) is an ordinary Java object, as distinct from a special object such as an EJB. A POJO normally is used to denote objects that are user-defined and are much simpler and more lightweight than EJBs and those used in other frameworks.
	- A Java Servlet is an object that resides on an application server and receives HTTP requests from clients and returns HTTP responses. Servlet implementations can use numerous interfaces to facilitate the development of useful applications.
	- A Java web container is a platform or engine that provides a runtime environment for Java-based web applications. Examples of Java web con- tainers are Apache Tomcat, BEA WebLogic, and JBoss.
- Many Java web applications employ third-party and open source components alongside custom-built code. This is an attractive option because it reduces development effort, and Java is well suited to this modular approach. Here are some examples of components commonly used for key application functions:
	- Authentication — JAAS, ACEGI
	- Presentation layer — SiteMesh, Tapestry
	- Database object relational mapping — Hibernate
	- Logging — Log4J

#### ASP.NET
- ASP.NET uses Microsoft’s .NET Framework, which provides a virtual machine (the Common Language Runtime) and a set of powerful APIs. Hence, ASP.NET applications can be written in any .NET language, such as C# or VB.NET.
	- ASP.NET lends itself to the event-driven programming paradigm that is normally used in conventional desktop software, rather than the script-based approach used in most earlier web application frameworks.
- The ASP.NET framework helps protect against some common web application vulnerabilities such as cross-site scripting, without requiring any effort from the developer.

#### PHP
- The PHP language emerged from a hobby project (the acronym originally stood for “personal home page”). It has since evolved almost unrecognizably into a highly powerful and rich framework for developing web applications. 
- It is often used in conjunction with other free technologies in what is known as the LAMP stack (composed of Linux as the operating system, Apache as the web server, MySQL as the database server, and PHP as the programming language for the web application).
- Numerous open source applications and components have been developed using PHP. Many of these provide off-the-shelf solutions for common application functions, which are often incorporated into wider custom-built applications:
	- Bulletin boards — PHPBB, PHP-Nuke
	- Administrative front ends — PHPMyAdmin
	- Web mail — SquirrelMail, IlohaMail
	- Photo galleries — Gallery
	- Shopping carts — osCommerce, ECW-Shop
	- Wikis — MediaWiki, WakkaWikki

#### Ruby on Rails
- Rails 1.0 was released in 2005, with strong emphasis on Model-View-Controller architecture. A key strength of Rails is the breakneck speed with which fully fledged data-driven applications can be created.
- If a developer follows the Rails coding style and naming conventions, Rails can autogenerate a model for database content, controller actions for modifying it, and default views for the application user.

#### SQL 
- Structured Query Language (SQL) is used to access data in relational databases, such as Oracle, MS-SQL server and MySQL. The vast majority of today’s web applications employ SQL-based databases as their back-end data store, and nearly all application functions involve interaction with these data stores in some way.


#### XML
- Extensible Markup Language (XML) is a specification for encoding data in a machine-readable form. Like any markup language, the XML format sepa- rates a document into content (which is data) and markup (which annotates the data).


#### Simple Object Access Protocol (SOAP)
- Web services use Simple Object Access Protocol (SOAP) to exchange data. 
- SOAP typically uses the HTTP protocol to transmit messages and represents data using the XML format.
- A typical SOAP request is as follows:
	```
	POST /doTransfer.asp HTTP/1.0
	Host: mdsec-mgr.int.mdsec.net
	Content-Type: application/soap+xml; charset=utf-8
	Content-Length: 891
	<?xml version=”1.0”?>
	<soap:Envelope xmlns:soap=”http://www.w3.org/2001/12/soap-envelope”>
	  <soap:Body>
	    <pre:Add xmlns:pre=http://target/lists soap:encodingStyle=
	“http://www.w3.org/2001/12/soap-encoding”>
	      <Account>
	        <FromAccount>18281008</FromAccount>
	        <Amount>1430</Amount>
	        <ClearedFunds>False</ClearedFunds>
	        <ToAccount>08447656</ToAccount>
	      </Account>
	    </pre:Add>
	  </soap:Body>
	</soap:Envelope>
	```
- In the context of web applications accessed using a browser, you are most likely to encounter SOAP being used by the server-side application to com- municate with various back-end systems.

### Client-Side Functionality 
#### HTML 
- The core technology used to build web interfaces is hypertext markup language (HTML).
- XHTML is a development of HTML that is based on XML and that has a stricter specification than older versions of HTML. Part of the motivation for XHTML was the need to move toward a more rigid standard for HTML markup to avoid the various compromises and security issues that can arise when browsers are obligated to tolerate less-strict forms of HTML.

#### Hyperlinks
- In web applications, hyperlinks frequently contain preset request parameters. These are items of data that the user never enters; they are submitted because the server places them into the target URL of the hyperlink that the user clicks. 
	- For example, a web application might present a series of links to news stories, each having the following form:
	`<a href=”?redir=/updates/update29.html”>What’s happening?</a>`
	- When a user clicks this link, the browser makes the following request:
	```
	GET /news/8/?redir=/updates/update29.html HTTP/1.1
	Host: mdsec.net
	```
	- The server receives the `redir` parameter in the query string and uses its value to determine what content should be presented to the user.

#### Forms
- HTML forms are the usual mechanism for allowing users to enter arbitraty input via their browser.
- A typical form is as follows:
	```
	<form action=”/secure/login.php?app=quotations” method=”post”>
	  username: <input type=”text” name=”username”><br>
	  password: <input type=”password” name=”password”>
	  <input type=”hidden” name=”redir” value=”/secure/home.php”>
	  <input type=”submit” name=”submit” value=”log in”>
	</form>
	```
	- When the user enters values into the form and clicks the Submit button, the browser makes a request like the following:
	```
	POST /secure/login.php?app=quotations HTTP/1.1
	Host: wahh-app.com
	Content-Type: application/x-www-form-urlencoded
	Content-Length: 39
	Cookie: SESS=GTnrpx2ss2tSWSnhXJGyG0LJ47MXRsjcFM6Bd
	username=daf&password=foo&redir=/secure/home.php&submit=log+in
	```
	- The preceding request contains a header specifying that the type of content in the message body is `x-www-form-urlencoded`. This means that parameters are represented in the message body as name/value pairs in the same way as they are in the URL query string. 
	- The other content type you are likely to encoun- ter when form data is submitted is `multipart/form-data`. An application can request that browsers use multipart encoding by specifying this in an `enctype` attribute in the form tag. With this form of encoding, the `Content-Type` header in the request also specifies a random string that is used as a separator for the parameters contained in the request body.
	- For example, if the form specified multipart encoding, the resulting request would look like the following:
	```
	POST /secure/login.php?app=quotations HTTP/1.1
	Host: wahh-app.com
	Content-Type: multipart/form-data; boundary=------------7d71385d0a1a
	Content-Length: 369
	Cookie: SESS=GTnrpx2ss2tSWSnhXJGyG0LJ47MXRsjcFM6Bd
	------------7d71385d0a1a
	Content-Disposition: form-data; name=”username”
	daf
	------------7d71385d0a1a
	Content-Disposition: form-data; name=”password”
	foo
	------------7d71385d0a1a
	Content-Disposition: form-data; name=”redir”
	/secure/home.php
	------------7d71385d0a1a
	Content-Disposition: form-data; name=”submit”
	log in
	------------7d71385d0a1a--
	```
	
#### CSS
- Cascading Style Sheets (CSS) is a language used to describe the presentation of a document written in a markup language. Within web applications, it is used to specify how HTML content should be rendered on-screen (and in other media, such as the printed page).

#### JavaScript
- JavaScript is a relatively simple but powerful programming language that can be easily used to extend web interfaces in ways that are not possible using HTML alone. 
- It is commonly used to perform the following tasks:
	- Validating user-entered data before it is submitted to the server to avoid unnecessary requests if the data contains errors
	- Dynamically modifying the user interface in response to user actions — for example, to implement drop-down menus and other controls familiar from non-web interfaces
	- Querying and updating the document object model (DOM) within the browser to control the browser’s behavior (the browser DOM is described in a moment)

#### VBScript
- VBScript is an alternative to JavaScript that is supported only in the Internet Explorer browser. 
- It is modeled on Visual Basic and allows interaction with the browser DOM. But in general it is somewhat less powerful and developed than JavaScript.
- Due to its browser-specific nature, VBScript is scarcely used in today’s web applications. 
- Its main interest from a security perspective is as a means of delivering exploits for vulnerabilities such as cross-site scripting in occasional situations where an exploit using JavaScript is not feasible.

#### Document Object Model
- The Document Object Model (DOM) is an abstract representation of an HTML document that can be queried and manipulated through its API.
- The DOM allows client-side scripts to access individual HTML elements by their `id` and to traverse the structure of elements programmatically. Data such as the current URL and cookies can also be read and updated. The DOM also includes an event model, allowing code to hook events such as form submission, navigation via links, and keystrokes.

#### Ajax
- Ajax is a collection of programming techniques used on the client side to create user interfaces that aim to mimic the smooth interaction and dynamic behavior of traditional desktop applications.
- The name originally was an acronym for “Asynchronous JavaScript and XML,” although in today’s web Ajax requests need not be asynchronous and need not employ XML.
- With Ajax, some user actions are handled within client-side script code and do not cause a full reload of the page. Instead, the script performs a request “in the background” and typically receives a much smaller response that is used to dynamically update only part of the user interface. 
- The core technology used in Ajax is `XMLHttpRequest`. After a certain consolida- tion of standards, this is now a native JavaScript object that client-side scripts can use to make “background” requests without requiring a window-level navigation event. Despite its name, `XMLHttpRequest` allows arbitrary content to be sent in requests and received in responses.

#### JSON
- JavaScript Object Notation (JSON) is a simple data transfer format that can be used to serialize arbitrary data. It can be processed directly by JavaScript interpreters. 
	- It is commonly employed in Ajax applications as an alternative to the XML format originally used for data transmission.
- In a typical situation, when a user performs an action, client-side JavaScript uses `XMLHttpRequest` to communicate the action to the server. The server returns a lightweight response containing data in JSON format. The client-side script then processes this data and updates the user interface accordingly.
- A location where you may encounter JSON data in today’s applications is as a means of encapsulating data within conventional request parameters. 
- For example, when the user updates the details of a contact, the new information might be communicated to the server using the following request:
	```
	POST /contacts HTTP/1.0
	Content-Type: application/x-www-form-urlencoded
	Content-Length: 89
	Contact={“name”:”Mike Kemp”,”id”:”8041148671”,”email”:”pikey@
	clappymonkey.com”}
	&submit=update
	```

#### Same-Origin Policy 
- The same-origin policy is a key mechanism implemented within browsers that is designed to keep content that came from different origins from interfering with each other. 
	- Basically, content received from one website is allowed to read and modify other content received from the same site but is not allowed to access content received from other sites.
- Here are some key features of the same-origin policy that you need to be aware of:
	- A page residing on one domain can cause an arbitrary request to be made to another domain (for example, by submitting a form or loading an image). But it cannot itself process the data returned from that request.
	- A page residing on one domain can load a script from another domain and execute this within its own context. This is because scripts are assumed to contain code, rather than data, so cross-domain access should not lead to disclosure of any sensitive information.
	- A page residing on one domain cannot read or modify the cookies or other DOM data belonging to another domain.

#### HTML5
- HTML5 is a major update to the HTML standard.
- From a security perspective, HTML5 is primarily of interest for the follow- ing reasons:
	- It introduces various new tags, attributes, and APIs that can be lever- aged to deliver cross-site scripting and other attacks.
	- It modifies the core Ajax technology, `XMLHttpRequest`, to enable two-way cross-domain interaction in certain situations. This can lead to new cross- domain attacks, as described in Chapter 13.
	- It introduces new mechanisms for client-side data storage, which can lead to user privacy issues, and new categories of attack such as client-side SQL injection.

#### Browser Extension Technologies
- Some web applications employ browser extension technologies that use custom code to extend the browser’s built-in capabilities in arbitrary ways. 
- These components may be deployed as bytecode that is executed by a suitable browser plug-in or may involve installing native executables onto the client computer itself. 
- The thick-client technologies you are likely to encounter when attacking web applications are:
	- Java applets
	- ActiveX controls
	- Flash objects
	- Silverlight objects

#### State and Sessions
- Because the HTTP protocol is itself stateless, most applications need a way to reidentify individual users across multiple requests for the correct set of state data to be used to process each request. Normally this is achieved by issuing each user a token that uniquely identifies that user’s session. 
	- These tokens may be transmitted using any type of request parameter, but most applications use HTTP cookies.


## Encoding Schemes
### URL Encoding
- URLs are permitted to contain only the printable characters in the US-ASCII character set — that is, those whose ASCII code is in the range 0x20 to 0x7e, inclusive.
	- Several characters within this range are restricted because they have special meaning within the URL scheme itself or within the HTTP protocol.
- The URL-encoding scheme is used to encode any problematic characters within the extended ASCII character set so that they can be safely transported over HTTP. 
- The URL-encoded form of any character is the % prefix followed by the character’s two-digit ASCII code expressed in hexadecimal. 
- Here are some characters that are commonly URL-encoded:
	- %3d — =
	- %25 — %
	- %20 — Space
		- Also represeneted by a `+` character
	- %0a — New line
	- %00 — Null byte
- **Note:** For the purpose of attacking web applications, you should URL-Encode any of the following characters when you insert them as data into an HTTP request: `space % ? & = ; + #`

### Unicode Encoding
- Unicode is a character encoding standard that is designed to support all of the world’s writing systems.
	- For transmission over HTTP, the 16-bit Unicode-encoded form of a character is the %u prefix followed by the character’s Unicode code point expressed in
	hexadecimal:
		- %u2215 — /
		- %u00e9 — é
- UTF-8 is a variable-length encoding standard that employs one or more bytes to express each character. 
	- For transmission over HTTP, the UTF-8-encoded form of a multibyte character simply uses each byte expressed in hexadecimal and preceded by the % prefix:
		- %c2%a9 — ©
		- %e2%89%a0 — ≠

- For the purpose of attacking web applications, Unicode encoding is primarily of interest because it can sometimes be used to defeat input validation mechanisms. 
	- If an input filter blocks certain malicious expressions, but the component that subsequently processes the input understands Unicode encoding, it may be possible to bypass the filter using various standard and malformed Unicode encodings.


### HTML Encoding
- HTML encoding defines numerous HTML entities to represent specific literal characters:
	- `&quot;` — " 
	- `&apos;` — ' 
	- `&amp;` — & 
	- `&lt;` — <
	- `&gt;` — >
- In addition, any character can be HTML-encoded using its ASCII code in deci-
mal form:
	- `&#34;` — "
	- `&#39;` — '
- Or by using its ASCII code in hexadecimal form (prefixed by an x):
	- `&#x22;` — "
	- `&#x27;` — '

### Base64
- Base64 encoding employs the following character set, which contains only printable ASCII characters:
`ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/`
- If the final block of input data results in fewer than three chunks of output data (not perfectly divisble by 3), the output is padded with one or two = characters.

### Hex Encoding
- Many applications use straightforward hexadecimal encoding when transmitting binary data, using ASCII characters to represent the hexadecimal block. 
- For example, hex-encoding the username “daf” within a cookie would result in this: `646166`


### Remoting and Serialization Frameworks
- Basically, these include software packages that utilize APIs which are implemented on the server side and remotely accessed (from the client side) so the proper serialization of data passed to the API functions can occur.
- Examples:
	- Flex and AMF
	- Silverlight and WCF
	- Java serialized objects.
