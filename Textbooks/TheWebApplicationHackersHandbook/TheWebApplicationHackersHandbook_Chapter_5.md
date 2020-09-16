The Web Application Hackers Handbook Chapter 5

# Chapter 5: Bypassing Client-Side Controls

## Transmitting Data Via the Client
- For a list of reasons why you would want to transmit data through the client see page 118 in the book or google that... Basically its because it takes a load off the server.
- Because everything submitted from the client to the server is within the user’s control, the assumption that data transmitted via the client will not be modified is usually false and often leaves the application vulnerable to one or more attacks.

### Hidden Form Fields
- The code behind this form is as follows:
	```
	<form method=”post” action=”Shop.aspx?prod=1”>
	Product: iPhone 5 <br/>
	Price: 449 <br/>
	Quantity: <input type=”text” name=”quantity”> (Maximum quantity is 50)
	<br/>
	<input type=”hidden” name=”price” value=”449”>
	<input type=”submit” value=”Buy”>
	</form>
	```
- Notice the form field called price, which is flagged as hidden. This field is sent to the server when the user submits the form:
	```
	POST /shop/28/Shop.aspx?prod=1 HTTP/1.1
	Host: mdsec.net
	Content-Type: application/x-www-form-urlencoded
	Content-Length: 20
	quantity=1&price=449
	```
	- The user cannot directly edit the price on the webpage, however they can indirectly edit the price field that is sent to ther server by using an intercepting proxy to modify the desired data on-the-fly.

### HTTP Cookies
- Just like Hidden Form Fields, HTTP Cookies can be easily modified with an intercepting proxy.
	- Basically, just modify the field set in the `Set-Cookie` field and the returned in the `Cookie` field.

### URL Parameters
- When a URL containing parameters is displayed in the browser’s location bar, any parameters can be modified easily by any user without the use of tools.
	- Example: `http://mdsec.net/shop/?prod=3&pricecode=32`

### The Referer Header
- Browsers include the `Referer` header within most HTTP requests. It is used to indicate the URL of the page from which the current request originated — either because the user clicked a hyperlink or submitted a form, or because the page referenced other resources such as images.

- For example, consider a mechanism that enables users to reset their password if they have forgotten it. The application requires users to proceed through several steps in a defined sequence before they actually reset their password’s value with the following request:
	```
	GET /auth/472/CreateUser.ashx HTTP/1.1
	Host: mdsec.net
	Referer: https://mdsec.net/auth/472/Admin.ashx
	```
	- The application may use the Referer header to verify that this request originated from the correct stage (Admin.ashx). 
		- If it did, the user can access the requested functionality.

### Opaque Data
- Sometimes, data transmitted via the client is not transparently intelligible because it has been encrypted or obfuscated in some way. 
- For example, instead of seeing a product’s price stored in a hidden field, you may see a cryptic value being transmitted:
	```
	<form method=”post” action=”Shop.aspx?prod=4”>
	  Product: Nokia Infinity <br/>
	  Price: 699 <br/>
	  Quantity: <input type=”text” name=”quantity”> (Maximum quantity is 50)
	  <br/>
	  <input type=”hidden” name=”price” value=”699”>
	  <input type=”hidden” name=”pricing_token”
	  value=”E76D213D291B8F216D694A34383150265C989229”>
	  <input type=”submit” value=”Buy”>
	</form>
	```

### The ASP.NET ViewState
- One commonly encountered mechanism for transmitting opaque data via the client is the ASP.NET `ViewState`. This is a hidden field that is created by default in all ASP.NET web applications. 
- It contains serialized information about the state of the current page. The ASP.NET platform employs the `ViewState` to enhance server performance. 
	- It enables the server to preserve elements within the user interface across successive requests without needing to maintain all the relevant state information on the server side. 
	- For example, the server may populate a drop-down list on the basis of parameters submitted by the user. 
		- When the user makes subsequent requests, the browser does not submit the contents of the list back to the server. 
		- However, the browser does submit the hidden `ViewState` field, which contains a serialized form of the list. 
		- The server deserializes the `ViewState` and recreates the same list that is presented to the user again.

- In addition to this core purpose of the ViewState, developers can use it to store arbitrary information across successive requests. 
	- For example, instead of saving the product’s price in a hidden form field, an application may save it in the ViewState as follows:
	```
	string price = getPrice(prodno);
	ViewState.Add(“price”, price);
	```
	- The form returned to the user now looks something like this:
	```
	<form method=”post” action=”Shop.aspx?prod=3”>
	<input type=”hidden” name=”__VIEWSTATE” id=”__VIEWSTATE”
	value=”/wEPDwULLTE1ODcxNjkwNjIPFgIeBXByaWNlBQMzOTlkZA==” />
	Product: HTC Avalanche <br/>
	Price: 399 <br/>
	Quantity: <input type=”text” name=”quantity”> (Maximum quantity is 50)
	<br/>
	<input type=”submit” value=”Buy”>
	</form>
	```
	- When the user submits the form, the browser sends the following:
	```
	POST /shop/76/Shop.aspx?prod=3 HTTP/1.1
	Host: mdsec.net
	Content-Type: application/x-www-form-urlencoded
	Content-Length: 77
	__VIEWSTATE=%2FwEPDwULLTE1ODcxNjkwNjIPFgIeBXByaWNlBQMzOTlkZA%3D%3D&
	quantity=1
	```
	- The `ViewState` ( or `__VIEWSTATE` as seen in the request) can be decoded from its URL format to reveal a Base64 Encoded message, which can then be decoded to reveal the price parameter:
	```
	// URL ENCODED
	%2FwEPDwULLTE1ODcxNjkwNjIPFgIeBXByaWNlBQMzOTlkZA%3D%3D
	
	// Base64 ENCODED
	/wEPDwULLTE1ODcxNjkwNjIPFgIeBXByaWNlBQMzOTlkZA==
	
	// DECODED 
	.....-1587169062.....price..399dd
	```
- **PRO TIP:** When you attempt to decode what appears to be a Base64-encoded string, a common mistake is to begin decoding at the wrong position within the string. Because of how Base64 encoding works, if you start at the wrong position, the decoded string will contain gibberish. 
	- Base64 is a block-based format in which every 4 bytes of encoded data translates into 3 bytes of decoded data. Hence, if your attempts to decode a Base64 string do not uncover anything meaningful, try starting from four adjacent offsets into the encoded string.

- By default, the ASP.NET platform protects the `ViewState` from tampering by adding a keyed hash to it (known as MAC protection). 
	- However, some applications disable this default protection, meaning that you can modify the `ViewState`’s value to determine whether it has an effect on the application’s server-side processing.
	- Burp Suite includes a `ViewState` parser that indicates whether the ViewState is MAC protected

### Bypassing Cached-Requests
- When you attempt to intercept and modify server responses, you may find that the relevant message displayed in your proxy looks like this:
	```
	HTTP/1.1 304 Not Modified
	Date: Wed, 6 Jul 2011 22:40:20 GMT
	Etag: “6c7-5fcc0900”
	Expires: Thu, 7 Jul 2011 00:40:20 GMT
	Cache-Control: max-age=7200
	```
	- This response arises because the browser already possesses a cached copy of the resource it requested. 

	- When the browser requests a cached resource, it typically adds two headers to the request — `If-Modified-Since` and `If-None-Match`:
	```
	GET /scripts/validate.js HTTP/1.1
	Host: wahh-app.com
	If-Modified-Since: Sat, 7 Jul 2011 19:48:20 GMT
	If-None-Match: “6c7-5fcc0900”
	```
	- These headers tell the server when the browser last updated its cached copy.
	- The `Etag` string, which the server provided with that copy of the resource, is a kind of serial number that the server assigns to each cacheable resource.
		- It updates each time the resource is modified. If the server possesses a newer version of the resource than the date specified in the If-Modified-Since header, or if the Etag of the current version matches the one specified in the If-None-Match header, the server responds with the latest version of the resource. 
		- Otherwise, it returns a 304 response, as shown here, informing the browser that the resource has not been modified and that the browser should use its cached copy.
	- When this occurs, and you need to intercept and modify the resource that the browser has cached, you can intercept the relevant request and remove the If-Modified-Since and If-None-Match headers. This causes the server to respond with the full version of the requested resource. 


### Script-Based Validation
```
<form method=”post” action=”Shop.aspx?prod=2” onsubmit=”return validateForm(this)”>
  Product: Samsung Multiverse <br/>
  Price: 399 <br/>
  Quantity: <input type=”text” name=”quantity”> (Maximum quantity is 50)
  <br/>
  <input type=”submit” value=”Buy”>
</form>
<script>function validateForm(theForm)
{
  var isInteger = /^\d+$/;
  var valid = isInteger.test(quantity) &&
      quantity > 0 && quantity <= 50;
  if (!valid)
      alert(’Please enter a valid quantity’);
  return valid;
} 
</script>
```
- The `onsubmit` attribute of the form tag instructs the browser to execute the `ValidateForm` function when the user clicks the Submit button, and to submit the form only if this function returns true.
- In the preceding example, the validation is simple; it checks whether the data entered in the `amount` field is an integer and is between 1 and 50.
- 2 basic methods to defeat this script validation:
	1. Disable JavaScript on the page entirely. This sometimes works but most of the time this will break the appliaction if the client-side depends on JavaScript for normal operation (constructing the user interface for example).
	2. A better apporach is to enter in a known good value into the input field in the browser, then intercept the validated submission with your proxy and modify the data to your desired value. 
- Note that if multiple input fields are subjected to client-side validation prior to form submission, you need to test each field individually with invalid data while leaving valid values in all the other fields. If you submit invalid data in multiple fields simultaneously, the server might stop processing the form when it identifies the first invalid field. Therefore, your testing won’t reach all possible code paths within the application.
- In Summary, if client-side controls are not replicated on the server, this can often be exploited to accept unintended values. 

*** 
- **HACK STEPS: Disabled Elements**
1. Look for disabled elements within each form of the application. Whenever you find one, try submitting it to the server along with the form’s other parameters to determine whether it has any effect.
2. Often, submit elements are flagged as disabled so that buttons appear as grayed out in contexts when the relevant action is unavailable. You should always try to submit the names of these elements to determine whether the application performs a server-side check before attempting to carry out the requested action.
3. Note that browsers do not include disabled form elements when forms are submitted. Therefore, you will not identify these if you simply walk through the application’s functionality, monitoring the requests issued by the browser. To identify disabled elements, you need to monitor the server’s responses or view the page source in your browser.
4. You can use the HTML modification feature in Burp Proxy to automatically re-enable any disabled fields used within the application.
***

## Capturing User Data: browser Extensions
### Common Browser Extension Technologies
- Properties of extension architecture that are relevant to security:
	- They are compiled to an intermediate bytecode.
	- They execute within a virtual machine that provides a sandbox environment for execution.
	- They may use remoting frameworks employing serialization to transmit complex data structures or objects over HTTP.
- Java, Flash, and Silverlight are the primary technologies used. 
- Java:
	- ava applets run in the Java Virtual Machine (JVM) and are subject to the sand- boxing applied by the Java Security Policy. Because Java has existed since early in the web’s history, and because its core concepts have remained relatively unchanged, a large body of knowledge and tools are available for attacking and defending Java applets.
- Flash:
	- Flash objects run in the Flash virtual machine, and, like Java applets, are sandboxed from the host computer. Once used largely as a method of delivering animated content, Flash has moved on. With newer versions of ActionScript, Flash is now squarely billed as capable of delivering full-blown desktop applica- tions. A key recent change in Flash is ActionScript 3 and its remoting capability with Action Message Format (AMF) serialization.
- Silverlight
	- Silverlight is Microsoft’s alternative to Flash. It is designed with the similar goal of enabling rich, desktop-like applications, allowing web applications to provide a scaled-down .NET experience within the browser, in a sandboxed environment. Technically, Silverlight applications can be developed in any .NET-compliant language from C# to Python, although C# is by far the most common.

- There are 2 primary approaches to analzing and modify browser extensions:
	1. You can intercept and modify the requests made by the component and the responses received from the server. 
		- This is certainly the easier of the two methods, however, the data being transmitted may be obfuscated or encrypted, or may be serialized using schemes that are specific to the technology being used.
	2. You can target the component itself directly and attempt to decom- pile its bytecode to view the original source, or interact dynamically with the component using a debugger. 
		- This is a much more thorough approach which will always yield more information about the target, however, it is much more difficult and time-consuming. 

## Handling Serialized Data
### Java Serialization
- Messages containing serialized Java objects usually can be identified because they have the following Content-Type header: `Content-Type: application/x-java-serialized-object`
	- Having intercepted the raw serialized data using your proxy, you can deserialize it using Java itself to gain access to the primitive data items it contains.
	- DSer is a handy plug-in to Burp Suite that provides a framework for viewing and manipulating serialized Java objects that have been intercepted within Burp.
		- This tool converts the primitive data within the intercepted object into XML format for easy editing. When you have modified the relevant data, DSer then reserializes the object and updates the HTTP request accordingly.
		- Link to DSer: http://blog.andlabs.org/2010/09/re-visiting-java-de-serialization-it.html

### Flash Serialization
- Flash uses its own serialization format that can be used to transmit complex data structures between server and client components. Action Message Format (AMF) normally can be identified via the following Content-Type header: `Content-Type: application/x-amf`
- Burp natively supports AMF format. When it identifies an HTTP request or response containing serialized AMF data, it unpacks the content and presents this in tree form for viewing and editing.

### Silverlight Serialization
- Silverlight applications can make use of the Windows Communication Foundation (WCF) remoting framework that is built in to the .NET platform. 
- Silverlight client components using WCF typically employ Microsoft’s .NET Binary Format for SOAP (NBFS), which can be identified via the following Content-Type header: `Content-Type: application/soap+msbin1`
	- The WCF binary SOAP plug-in for Burp was produced by Brian Holyfield and is available to download here: www.gdssecurity.com/l/b/2009/11/19/wcf-binary-soap-plug-in-for-burp/


- **Note:** To learn about "Obstacles to Intercepting Traffic from Browser Extensions" see page 138, chapter 20, or google it. Basically your settings are probably wrong or you have an SSL certificate problem. 
	- One tool that can help is called Echo Mirage.

## Decompiling Browser Extensions
- Browser extensions are compiled into bytecode. 
- Bytecode is a high-level platform-independent binary representation that can be executed by the relevant interpreter (such as the Java Virtual Machine or Flash Player), and each browser extension technology uses its own bytecode format. 
	- As a result, the application can run on any platform that the interpreter itself can run on.

### Downloading the Bytecode
- Java applets generally are loaded using the `<applet>` tag, and other components generally are loaded using the `<object>` tag. For example:
	```
	<applet code=”CheckQuantity.class” codebase=”/scripts”
	id=”CheckQuantityApplet”>
	</applet>
	```
	- When you have identified the URL for the browser extension’s bytecode, usu- ally you can just paste this URL into your browser’s address bar. Your browser then prompts you to save the bytecode file on your local filesystem.
		- For more tips see page 140

### Decompiling the Bytecode
- Bytecode usually is distributed in a single-file package, which may need to be unpacked to obtain the individual bytecode files for decompilation into source code.
- Java applets normally are packaged as `.jar` (Java archive) files
	- The Java bytecode is contained in `.class` files.

- Silverlight objects are packaged as `.xap` files.
	- Silverlight bytecode is contained in `.dll` files.

- Both of these file types use the zip archive format, so you can easily unpack them by renaming the files with the .zip extension and then using any zip reader to unpack them into the individual files they contain. 
	- After unpacking the relevant file package, you need to decompile these files to obtain source code.

- Flash objects are packaged as `.swf` files and don’t require any unpacking before you use a decompiler.

- To actually decompule you will need specific tools:
	- Java Tools:
		- Java bytecode can be decompiled to into Java source code using a tool called Jad (the Java decompiler), which is available from: www.varaneckas.com/jad
		- The Jad program saves its decompiled source code with the `.jad` extension. However, if you want to modify and recompile the source code, you need to rename each source file with the .java extension.
	- Flash Tools:
		- Flash bytecode can be decompiled into ActionScript source code.
		- An alternative approach, which is often more effective, is to disassemble the bytecode into a human-readable form, without actually fully decompiling it into source code.
		- To decompile and disassemble Flash, you can use the following tools:
			- Flasm — www.nowrap.de/flasm
			- Flare — www.nowrap.de/flare
			- SWFScan — www.hp.com/go/swfscan (this works for Actionscript 2 and 3)
	- Silverlight Tools
		- Silverlight bytecode can be decompiled into source code using a tool called .NET Reflector, which is available from: www.red-gate.com/products/dotnet-development/reflector/

### Recompiling and Executing Within the Browser
- You can modify the decompiled source code to change the component’s behavior, recompile it to bytecode, and execute the modified component within your browser. 
	- This method is preferred to modify client-side events. 
- To perform the recompilation, you need to use the developer tools that are relevant to the technology you are using:
	- For Java, use the javac program in the JDK to recompile your modified source code.
	- For Flash, you can use flasm to reassemble your modified bytecode or one of the Flash development studios from Adobe to recompile modified ActionScript source code.
	- For Silverlight, use Visual Studio to recompile your modified source code.

- Having recompiled your source code into one or more bytecode files, you may need to repackage the distributable file if required for the technology being used.
	- For Java and Silverlight, replace the modified bytecode files in your unpacked archive, repackage using a zip utility, and then change the extension back to `.jar` or `.xap` as appropriate.

- The final step is to load your modified component into your browser so that your changes can take effect within the application you are testing. You can achieve this in various ways:
	- If you can find the physical file within your browser’s on-disk cache that contains the original executable, you can replace this with your modified version and restart your browser. 
		- This approach may be difficult if your browser does not use a different individual file for each cached resource or if caching of browser extension components is implemented only in memory.
	- Using your intercepting proxy, you can modify the source code of the page that loads the component and specify a different URL, pointing to either the local filesystem or a web server that you control. 
		- This approach normally is difficult because changing the domain from which the component is loaded may violate the browser’s same origin policy and may require reconfiguring your browser or other methods to weaken this policy.
	- You can cause your browser to reload the component from the original server (as described in the earlier section “Downloading the Bytecode”), use your proxy to intercept the response containing the executable, and replace the body of the message with your modified version. In Burp Proxy, you can use the Paste from File context menu option to achieve this. 
		- This approach usually is the easiest and least likely to run into the problems described previously.

### Recompiling and Executing Ouside the Browser
- In some cases, it is not necessary to modify the component’s behavior while it is being executed.
	- To carry out this attack, you need to change the original executable, which is designed to run within the relevant browser extension, into a standalone program that can be run on the command line.
	-  The way this is done depends on the programming language being used. 
	- For example, in Java you simply need to implement a main method. The section “Java Applets: A Worked Example” gives an example of how to do this.

- The reason you don't always need a browser is because some browser extension components validate user-supplied input and then obfuscate or encrypt the result before sending it to the server. In this situation, you may be able to modify the component to perform the required obfuscation or encryption on arbitrary unvalidated input and simply output the result locally. You can then use your proxy to intercept the relevant request when the original component submits the validated input, and you can replace this with the value that was output by your modified component.


### Manipulating the Original Component Using JavaScript
- In some cases, it is not necessary to modify the component’s bytecode. 
	- Instead, you may be able to achieve your objectives by modifying the JavaScript within the HTML page that interacts with the component.
		- Having reviewed the component’s source code, you can identify all its public methods that can be invoked directly from JavaScript, and the way in which parameters to those methods are handled.	

### Attaching A Debugger
- Because the debugger is working at the bytecode level, it can be easily used to control and understand the flow of execution.
- Efficient debuggers are not fully matured for all the browser extension technologies, but debugging is well supported for Java applets. 
	- JavaSnoop, is a Java debugger that can integrate Jad to decompile source code, trace variables through an application, and set breakpoints on methods to view and modify parameters.
	- An alternative tool for debugging Java is JSwat.
		- To use JSwat, you need to launch an applet using the `appletviewer` tool included in the JDK and then connect JSwat to it. 
		- For example, you could use this command:
			```
			appletviewer -J-Xdebug -J-Djava.compiler=NONE -J-
			Xrunjdwp:transport=dt_socket,
			server=y,suspend=n,address=5000 appletpage.htm 
			```
- When you’re working on Silverlight objects, you can use the Silverlight Spy tool to monitor the component’s execution at runtime.
	- Silverlight Spy is available from the following URL: http://firstfloorsoftware.com/SilverlightSpy/


### Native Client Components
- Some applications need to perform actions within the user’s computer that cannot be conducted from inside a browser-based VM sandbox. 
- In terms of client-side security controls, here are some examples of this functionality:
	- Verifying that a user has an up-to-date virus scanner
	- Verifying that proxy settings and other corporate configuration are in force
	- Integrating with a smartcard reader
- **Typically, these kinds of actions require the use of native code components, which integrate local application functionality with web application functionality.** 
	- Native client components are often delivered via **ActiveX controls**, which are custom browser extensions that run outside the browser sandbox.

- Native client components may be significantly harder to decipher than other browser extensions, because there is no equivalent to intermediate bytecode.
	- You can use IDA Pro or OllyDbg to step through the native executables

- Final Takeaway: **In every instance where you identify data being transmitted via the client, or validation of user-supplied input being implemented on the client, you should test how the server responds to unexpected data that bypasses those controls.**


