#ifndef WEB_H
#define WEB_H

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <string>
class webPage{
public:
    webPage(std::string);
    ~webPage();
    Awesomium::BitmapSurface* surface;
    char *webExt_;
private:
    Awesomium::WebView* view;
    void update(int);
    void genChars(std::string);
    char *webAddress_;
};
extern Awesomium::WebCore* web_core;
#endif
