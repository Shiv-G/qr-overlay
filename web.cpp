/**
 * This is a simple "Hello World!" example of using Awesomium.
 *
 * It loads a page and saves a rendered bitmap of it to a JPEG.
 *
 * Procedure:
 * -- Create the WebCore singleton
 * -- Create a new WebView and request for it to load a URL.
 * -- Wait for the WebView to finish loading.
 * -- Retrieve the BitmapSurface from the WebView.
 * -- Save the BitmapSurface to 'result.jpg'.
 * -- Clean up.
 */

// Various included headers
#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <iostream>
#include <algorithm>
#if defined(__WIN32__) || defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <unistd.h>
#endif

#include "web.h"

// Various macro definitions
#define WIDTH   800
#define HEIGHT  600
//#define URL     "http://www.google.com"

using namespace Awesomium;


// Our main program
webPage::webPage(std::string webAddress) {
  // Create the WebCore singleton with default configuration
  genChars(webAddress);

  // Create a new WebView instance with a certain width and height, using the
  // WebCore we just created
  view = web_core->CreateWebView(WIDTH, HEIGHT);

  // Load a certain URL into our WebView instance
  WebURL url(WSLit(webAddress_));
  view->LoadURL(url);


  // Wait for our WebView to finish loading
  while (view->IsLoading())
    update(50);


  // Update once more a little longer to allow scripts and plugins
  // to finish loading on the page.
  update(300);

  // Get the WebView's rendering Surface. The default Surface is of
  // type 'BitmapSurface', we must cast it before we can use it.
  surface = (BitmapSurface*)view->surface();

}
webPage::~webPage(){
  // Destroy our WebView instance
  view->Destroy();

  // Update once more before we shutdown for good measure
  update(100);
  delete webExt_;
  delete webAddress_;


}

void webPage::update(int sleep_ms) {
  // Sleep a specified amount
#if defined(__WIN32__) || defined(_WIN32)
  Sleep(sleep_ms);
#elif defined(__APPLE__)
  usleep(sleep_ms * 1000);
#endif

  // You must call WebCore::update periodically
  // during the lifetime of your application.
  WebCore::instance()->Update();
}

void webPage::genChars(std::string webAddress){
    std::string webExt = webAddress;
    std::replace(webExt.begin(), webExt.end(), '/', '-');
    std::replace(webExt.begin(), webExt.end(), '.', '_');
    std::replace(webExt.begin(), webExt.end(), ':', ',');
    webExt = webExt + ".jpg";
    webAddress_ = new char[webAddress.size() + 1];
    std::copy(webAddress.begin(),webAddress.end(),webAddress_);
    webAddress_[webAddress.size()] = '\0';
    webExt_ = new char[webExt.size() + 1];
    std::copy(webExt.begin(),webExt.end(),webExt_);
    webExt_[webExt.size()] = '\0';
}
    Awesomium::WebCore* web_core = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
