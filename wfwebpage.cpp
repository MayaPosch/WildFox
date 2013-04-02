/*
  wfwebpage.cpp - implementation of WildFox's WFWebPage class.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2013/03/26, Maya Posch
  (c) Nyanko.ws
*/


#include "wfwebpage.h"

#include "QNetworkReply"


// --- CONSTRUCTOR ---
WFWebPage::WFWebPage(QWidget *parent) : QWebPage(parent) {
    //
}


// --- SUPPORTS EXTENSION ---
// Returns whether a certain extension is supported or not.
bool WFWebPage::supportsExtension(Extension extension) const {
    if (extension == ErrorPageExtension) {
        return true;
    }
    
    return false;
}


// --- EXTENSION ---
bool WFWebPage::extension(Extension extension, const ExtensionOption* option, ExtensionReturn* output) {
    if (extension == QWebPage::ErrorPageExtension) {
        // handle the received error.
        ErrorPageExtensionOption* extOption = (ErrorPageExtensionOption*) option;
        int errDomain = extOption->domain;
        int errValue = extOption->error;
        QWebPage::ErrorPageExtensionReturn* errorPage = static_cast<QWebPage::ErrorPageExtensionReturn*>(output);
        QString errorHeader = "<html><head><title>Failed loading page</title></head><body>";
        QString errorFooter = "</body></html>";
       //errorPage->content = QString("<html><head><title>Failed loading page</title></head><body>%1</body></html>").arg(info->errorString).toUtf8();
       //return true;
        //int httpResponse = 
        switch (errDomain) {
        case QtNetwork:
            // The error occurred in the QtNetwork layer; 
            // the error code is of type QNetworkReply::NetworkError.
            switch (errValue) {
            case QNetworkReply::NoError:
                // 0    no error condition. Note: When the HTTP protocol returns 
                //      a redirect no error will be reported. You can check if  
                //      there is a redirect with the 
                //      QNetworkRequest::RedirectionTargetAttribute attribute.
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ConnectionRefusedError:
                // 1	the remote server refused the connection 
                //      (the server is not accepting requests)
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::RemoteHostClosedError:
                // 2	the remote server closed the connection prematurely, 
                //      before the entire reply was received and processed
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::HostNotFoundError:
                // 3	the remote host name was not found (invalid hostname)
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::TimeoutError:
                // 4	the connection to the remote server timed out
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::OperationCanceledError:
                // 5	the operation was canceled via calls to abort() or 
                //      close() before it was finished.
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::SslHandshakeFailedError:
                // 6	the SSL/TLS handshake failed and the encrypted channel 
                //      could not be established. The sslErrors() signal should 
                //      have been emitted.
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::TemporaryNetworkFailureError:
                // 7	the connection was broken due to disconnection from the 
                //      network, however the system has initiated roaming to 
                //      another access point. The request should be resubmitted 
                //      and will be processed as soon as the connection is 
                //      re-established.
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProxyConnectionRefusedError:
                // 101	the connection to the proxy server was refused 
                //      (the proxy server is not accepting requests)
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProxyConnectionClosedError:
                // 102	the proxy server closed the connection prematurely, 
                //      before the entire reply was received and processed
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProxyNotFoundError:
                // 103	the proxy host name was not found (invalid proxy hostname)
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProxyTimeoutError:
                // 104	the connection to the proxy timed out or the proxy did 
                //      not reply in time to the request sent
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProxyAuthenticationRequiredError:
                // 105	the proxy requires authentication in order to honour the 
                //      request but did not accept any credentials offered (if any)
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ContentAccessDenied:
                // 201	the access to the remote content was denied (similar to 
                //      HTTP error 401)
                //errorPage->content(errorHeader + extOption->errorString + errorFooter);
                //return true;
                break;
            case QNetworkReply::ContentOperationNotPermittedError:
                // 202	the operation requested on the remote content is not permitted
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::ContentNotFoundError:
                // 203	the remote content was not found at the server (similar 
                //      to HTTP error 404)
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::AuthenticationRequiredError:
                // 204	the remote server requires authentication to serve the 
                //      content but the credentials provided were not accepted (if any)
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::ContentReSendError:
                // 205	the request needed to be sent again, but this failed for 
                //      example because the upload data could not be read a second time.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProtocolUnknownError:
                // 301	the Network Access API cannot honor the request because 
                //      the protocol is not known
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProtocolInvalidOperationError:
                // 302	the requested operation is invalid for this protocol
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::UnknownNetworkError:
                // 99	an unknown network-related error was detected
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::UnknownProxyError:
                // 199	an unknown proxy-related error was detected
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::UnknownContentError:
                // 299	an unknown error related to the remote content was detected
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case QNetworkReply::ProtocolFailure:
                // 399	a breakdown in protocol was detected (parsing error, 
                //      invalid or unexpected responses, etc.)
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            default:
                //
                break;
            }
            
            break;
        case Http:
            // The error occurred in the HTTP layer; 
            // the error code is a HTTP status code (see QNetworkRequest::HttpStatusCodeAttribute).
            //int httpResponse = 
            switch (errValue) {
            case 400:
                // Bad Request
                // The request cannot be fulfilled due to bad syntax.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 401:
                // Unauthorized
                // Similar to 403 Forbidden, but specifically for use when 
                // authentication is required and has failed or has not yet been 
                // provided.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 402:
                // Payment Required
                // Reserved for future use.[2] The original intention was that 
                // this code might be used as part of some form of digital cash 
                // or micropayment scheme, but that has not happened, and this 
                // code is not usually used. As an example of its use, however, 
                // Apple's defunct MobileMe service generated a 402 error if the 
                // MobileMe account was delinquent. In addition, YouTube uses 
                // this status if a particular IP address has made excessive 
                // requests, and requires the person to enter a CAPTCHA.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 403:
                // Forbidden
                // The request was a valid request, but the server is refusing 
                // to respond to it. Unlike a 401 Unauthorized response, 
                // authenticating will make no difference. On servers where 
                // authentication is required, this commonly means that the 
                // provided credentials were successfully authenticated but that 
                // the credentials still do not grant the client permission 
                // to access the resource.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 404:
                // Not Found
                // The requested resource could not be found but may be 
                // available again in the future. Subsequent requests by the 
                // client are permissible.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 405:
                // Method Not Allowed
                // A request was made of a resource using a request method not 
                // supported by that resource; for example, using GET on a form 
                // which requires data to be presented via POST, or using PUT on 
                // a read-only resource.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 406:
                // Not Acceptable
                // The requested resource is only capable of generating content 
                // not acceptable according to the Accept headers sent in the 
                // request.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 407:
                // Proxy Authentication Required
                // The client must first authenticate itself with the proxy.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 408:
                // Request Timeout
                // The server timed out waiting for the request. According to W3 
                // HTTP specifications: "The client did not produce a request 
                // within the time that the server was prepared to wait. The 
                // client MAY repeat the request without modifications at any 
                // later time."
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 409:
                // Conflict
                // Indicates that the request could not be processed because of 
                // conflict in the request, such as an edit conflict.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 410:
                // Gone
                // Indicates that the resource requested is no longer available 
                // and will not be available again. This should be used when a 
                // resource has been intentionally removed and the resource 
                // should be purged. Upon receiving a 410 status code, the 
                // client should not request the resource again in the future. 
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 411:
                // Length Required
                // The request did not specify the length of its content, which 
                // is required by the requested resource.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 412:
                // Precondition Failed
                // The server does not meet one of the preconditions that the 
                // requester put on the request.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 413:
                // Request Entity Too Large
                // The request is larger than the server is willing or able to 
                // process.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 414:
                // Request-URI Too Long
                // The URI provided was too long for the server to process.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 415:
                // Unsupported Media Type
                // The request entity has a media type which the server or 
                // resource does not support. For example, the client uploads an 
                // image as image/svg+xml, but the server requires that images 
                // use a different format.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 416:
                // Requested Range Not Satisfiable
                // The client has asked for a portion of the file, but the server 
                // cannot supply that portion. For example, if the client asked 
                // for a part of the file that lies beyond the end of the file.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 417:
                // Expectation Failed
                // The server cannot meet the requirements of the Expect 
                // request-header field.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 418:
                // I'm a teapot (RFC 2324)
                // This code was defined in 1998 as one of the traditional IETF 
                // April Fools' jokes, in RFC 2324, Hyper Text Coffee Pot Control 
                // Protocol, and is not expected to be implemented by actual 
                // HTTP servers.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 420:
                // Enhance Your Calm (Twitter)
                // Not part of the HTTP standard, but returned by the Twitter 
                // Search and Trends API when the client is being rate limited.
                // Other services may wish to implement the 429 Too Many 
                // Requests response code instead.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 422:
                // Unprocessable Entity (WebDAV; RFC 4918)
                // The request was well-formed but was unable to be followed due to semantic errors.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 423:
                // Locked (WebDAV; RFC 4918)
                // The resource that is being accessed is locked.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 424:
                // Failed Dependency (WebDAV; RFC 4918)
                // The request failed due to failure of a previous request 
                // (e.g. a PROPPATCH).
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 425:
                // Unordered Collection (Internet draft)
                // Defined in drafts of "WebDAV Advanced Collections Protocol",
                // but not present in "Web Distributed Authoring and Versioning 
                // (WebDAV) Ordered Collections Protocol".
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 426:
                // Upgrade Required (RFC 2817)
                // The client should switch to a different protocol such as 
                // TLS/1.0.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 428:
                // Precondition Required (RFC 6585)
                // The origin server requires the request to be conditional. 
                // Intended to prevent "the 'lost update' problem, where a 
                // client GETs a resource's state, modifies it, and PUTs it back 
                // to the server, when meanwhile a third party has modified the 
                // state on the server, leading to a conflict."
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 429:
                // Too Many Requests (RFC 6585)
                // The user has sent too many requests in a given amount of time. 
                // Intended for use with rate limiting schemes.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 431:
                // Request Header Fields Too Large (RFC 6585)
                // The server is unwilling to process the request because either 
                // an individual header field, or all the header fields 
                // collectively, are too large.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 449:
                // Retry With (Microsoft)
                // A Microsoft extension. The request should be retried after 
                // performing the appropriate action.
                // Often search-engines or custom applications will ignore 
                // required parameters. Where no default action is appropriate, 
                // the Aviongoo website sends a "HTTP/1.1 449 Retry with valid 
                // parameters: param1, param2, . . ." response. The applications 
                // may choose to learn, or not.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 450:
                // Blocked by Windows Parental Controls (Microsoft)
                // A Microsoft extension. This error is given when Windows 
                // Parental Controls are turned on and are blocking access to 
                // the given webpage.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 451:
                // Parameter Not Understood (RTSP)
                // The recipient of the request does not support one or more 
                // parameters contained in the request.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 452:
                // Conference Not Found (RTSP)
                // The conference indicated by a Conference header field is 
                // unknown to the media server.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 453:
                // Not Enough Bandwidth (RTSP)
                // The request was refused because there was insufficient 
                // bandwidth. This may, for example, be the result of a resource 
                // reservation failure.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 454:
                // Session Not Found (RTSP)
                // The RTSP session identifier in the Session header is missing, 
                // invalid, or has timed out.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 455:
                // Method Not Valid in This State (RTSP)
                // The client or server cannot process this request in its 
                // current state. The response SHOULD contain an Allow header to 
                // make error recovery easier.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 456:
                // Header Field Not Valid for Resource (RTSP)
                // The server could not act on a required request header. For 
                // example, if PLAY contains the Range header field but the 
                // stream does not allow seeking.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 457:
                // Invalid Range (RTSP)
                // The Range value given is out of bounds, e.g., beyond the end 
                // of the presentation.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 458:
                // Parameter Is Read-Only (RTSP)
                // The parameter to be set by SET_PARAMETER can be read but not 
                // modified.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 459:
                // Aggregate Operation Not Allowed (RTSP)
                // The requested method may not be applied on the URL in question 
                // since it is an aggregate (presentation) URL. The method may 
                // be applied on a stream URL.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 460:
                // Only Aggregate Operation Allowed (RTSP)
                // The requested method may not be applied on the URL in question 
                // since it is not an aggregate (presentation) URL. The method 
                // may be applied on the presentation URL.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 461:
                // Unsupported Transport (RTSP)
                // The Transport field did not contain a supported transport 
                // specification.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 462:
                // Destination Unreachable (RTSP)
                // The data transmission channel could not be established 
                // because the client address could not be reached. This error 
                // will most likely be the result of a client attempt to place 
                // an invalid Destination parameter in the Transport field.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 500:
                // Internal Server Error
                // A generic error message, given when no more specific message 
                // is suitable.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 501:
                // Not Implemented
                // The server either does not recognize the request method, or 
                // it lacks the ability to fulfill the request.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 502:
                // Bad Gateway
                // The server was acting as a gateway or proxy and received an 
                // invalid response from the upstream server.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 503:
                // Service Unavailable
                // The server is currently unavailable (because it is overloaded 
                // or down for maintenance). Generally, this is a temporary state.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 504:
                // Gateway Timeout
                // The server was acting as a gateway or proxy and did not 
                // receive a timely response from the upstream server.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 505:
                // HTTP Version Not Supported
                // The server does not support the HTTP protocol version used in 
                // the request.
                //errorPage->content(errorHeader + extOption->errorString.toUtf8() + errorFooter);
                //return true;
                break;
            case 506:
                // Variant Also Negotiates (RFC 2295)
                // Transparent content negotiation for the request results in a 
                // circular reference.
                //
                break;
            case 507:
                // Insufficient Storage (WebDAV; RFC 4918)
                // The server is unable to store the representation needed to 
                // complete the request.
                //
                break;
            case 508:
                // Loop Detected (WebDAV; RFC 5842)
                // The server detected an infinite loop while processing the 
                // request (sent in lieu of 208).
                //
                break;
            case 509:
                // Bandwidth Limit Exceeded (Apache bw/limited extension)
                // This status code, while used by many servers, is not 
                // specified in any RFCs.
                //
                break;
            case 510:
                // Not Extended (RFC 2774)
                // Further extensions to the request are required for the server 
                // to fulfill it.
                //
                break;
            case 511:
                // Network Authentication Required (RFC 6585)
                // The client needs to authenticate to gain network access. 
                // Intended for use by intercepting proxies used to control 
                // access to the network (e.g. "captive portals" used to require 
                // agreement to Terms of Service before granting full Internet 
                // access via a Wi-Fi hotspot).
                //
                break;
            case 550:
                // Permission denied
                //
                break;
            case 551:
                // Option not supported (RTSP)
                // An option given in the Require or the Proxy-Require fields 
                // was not supported. The Unsupported header should be returned 
                // stating the option for which there is no support.
                //
                break;
            case 598:
                // Network read timeout error (Unknown)
                // This status code is not specified in any RFCs, but is used by 
                // Microsoft HTTP proxies to signal a network read timeout 
                //behind the proxy to a client in front of the proxy.
                //
                break;
            case 599:
                // Network connect timeout error (Unknown)
                // This status code is not specified in any RFCs, but is used 
                // by Microsoft HTTP proxies to signal a network connect timeout 
                // behind the proxy to a client in front of the proxy.
                //
                break;
            default:
                break;
            }
            
            break;
        case WebKit:
            // The error is an internal WebKit error.
            //
            break;
        default:            
            //
            break;
        }
        
        errorPage->content = QString(errorHeader + extOption->errorString + errorFooter).toLatin1();
        return true;
    }
    
    return false;
}
