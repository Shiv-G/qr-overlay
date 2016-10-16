#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <iterator>
#include <Awesomium/WebCore.h>

#include "web.h"
using namespace std;
using namespace cv;
using namespace zbar;


void overlayImage(const cv::Mat &background, const cv::Mat &foreground, 
  cv::Mat &output, cv::Point2i location)
{
  background.copyTo(output);


  // start at the row indicated by location, or at row 0 if location.y is negative.
  for(int y = std::max(location.y , 0); y < background.rows; ++y)
  {
    int fY = y - location.y; // because of the translation

    // we are done of we have processed all rows of the foreground image.
    if(fY >= foreground.rows)
      break;

    // start at the column indicated by location, 

    // or at column 0 if location.x is negative.
    for(int x = std::max(location.x, 0); x < background.cols; ++x)
    {
      int fX = x - location.x; // because of the translation.

      // we are done with this row if the column is outside of the foreground image.
      if(fX >= foreground.cols)
        break;

      // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
      double opacity =
        ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

        / 255.;


      // and now combine the background and foreground pixel, using the opacity, 

      // but only if opacity > 0.
      for(int c = 0; opacity > 1 && c < output.channels(); ++c)
      {
        unsigned char foregroundPx =
          foreground.data[fY * foreground.step + fX * foreground.channels() + c];
        unsigned char backgroundPx =
          background.data[y * background.step + x * background.channels() + c];
        output.data[y*output.step + output.channels()*x + c] =
          //backgroundPx * (1.-opacity) + foregroundPx * opacity;
          foregroundPx;
      }
    }
  }
}



int main(int argc, char **argv) {

    //double alpha, beta;
    Mat Foreground, result;

    int cam_idx = 0;

    // std::cout<<" Simple Linear Blender "<<std::endl;
    // std::cout<<"-----------------------"<<std::endl;
    // std::cout<<"* Enter alpha [0-1]: ";
    // std::cin>>alpha;

    // if(alpha<0.0 || alpha>1.0) {
    //     std::cout<<"Input invalid. Enter alpha [0-1]";
    //     std::cin>>alpha;
    // }
    // else{}

    //beta = 1.0-alpha;
    //char webAddr[] = "http://www.google.com";

    if (argc == 2) {
        cam_idx = atoi(argv[1]);
    }

    VideoCapture cap(cam_idx);
    if (!cap.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);



    namedWindow("ar qr scanner", CV_WINDOW_AUTOSIZE);
    
    // Create a zbar reader
    ImageScanner scanner;
    
    // Configure the reader
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    std::map<std::string,std::pair<webPage*,Mat*> > webPageMap;
    std::map<std::string,std::pair<webPage*,Mat*> >::iterator webPageMapIt;
    std::string webAddress, webExt;
    for (;;) {
        // Capture an OpenCV frame
        cv::Mat frame, frame_grayscale;
        cap >> frame;

        // Convert to grayscale
        cvtColor(frame, frame_grayscale, CV_BGR2GRAY);

        // Obtain image data
        int width = frame_grayscale.cols;
        int height = frame_grayscale.rows;
        uchar *raw = (uchar *)(frame_grayscale.data);

        // Wrap image data
        Image image(width, height, "Y800", raw, width * height);

        // Scan the image for barcodes
        //int n = scanner.scan(image);
        scanner.scan(image);

        imshow("ar qr scanner", frame);
        // Extract results
        int counter = 0;
        for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
            time_t now;
            tm *current;
            now = time(0);
            current = localtime(&now);

            // do something useful with results
            cout    << "[" << current->tm_hour << ":" << current->tm_min << ":" << setw(2) << setfill('0') << current->tm_sec << "] " << counter << " "
                    << "decoded " << symbol->get_type_name()
                    << " symbol \"" << symbol->get_data() << '"' << endl;
            webAddress = symbol->get_data();
            cout << webAddress;
            if(!(webPageMap.count(webAddress))){
                webPageMap[webAddress]=std::make_pair(new webPage(webAddress), new Mat);
            }
    webPageMap.find(webAddress)->second.first->surface->SaveToJPEG(Awesomium::WSLit(webPageMap.find(webAddress)->second.first->webExt_));
    * webPageMap.find(webAddress)->second.second = imread(webPageMap.find(webAddress)->second.first->webExt_, -1);
            //cout << "Location: (" << symbol->get_location_x(0) << "," << symbol->get_location_y(0) << ")" << endl;
            //cout << "Size: " << symbol->get_location_size() << endl;
            
            // Draw location of the symbols found
            if (symbol->get_location_size() == 4) {
                //rectangle(frame, Rect(symbol->get_location_x(i), symbol->get_location_y(i), 10, 10), Scalar(0, 255, 0));
                line(frame, Point(symbol->get_location_x(0), symbol->get_location_y(0)), Point(symbol->get_location_x(1), symbol->get_location_y(1)), Scalar(0, 255, 0), 2, 8, 0);
                line(frame, Point(symbol->get_location_x(1), symbol->get_location_y(1)), Point(symbol->get_location_x(2), symbol->get_location_y(2)), Scalar(0, 255, 0), 2, 8, 0);
                line(frame, Point(symbol->get_location_x(2), symbol->get_location_y(2)), Point(symbol->get_location_x(3), symbol->get_location_y(3)), Scalar(0, 255, 0), 2, 8, 0);
                line(frame, Point(symbol->get_location_x(3), symbol->get_location_y(3)), Point(symbol->get_location_x(0), symbol->get_location_y(0)), Scalar(0, 255, 0), 2, 8, 0);
            overlayImage(frame,* webPageMap.find(webAddress)->second.second, result, cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)));
            imshow("ar qr scanner", result);
            }
            
            // Get points
            /*for (Symbol::PointIterator point = symbol.point_begin(); point != symbol.point_end(); ++point) {
                cout << point << endl;
            } */
            counter++;
        }


        //namedWindow("Overlay", 1);
        //addWeighted(frame, alpha, Harambe, beta, 0.0, dst);
        //overlayImage(frame, Foreground, result, cv::Point(0, 0));

        // Show captured frame, now with overlays!
        //imshow("ar qr scanner", frame);
        //imshow("Linear Blend", Harambe);
        //imshow("Result", result);
                                                                                                                                                          
        // clean up
        image.set_data(NULL, 0);
        
        waitKey(30);
    }

    // Destroy our WebCore instance
    Awesomium::WebCore::Shutdown();
    return 0;
}
