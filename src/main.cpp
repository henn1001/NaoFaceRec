/**
 *============================================================================
 * Name        :    NaoFaceRecognition
 * Author      :    Hendrik Schlee / Daniel Schäfer
 * Description :    This programm uses a remote module to retrieve the Nao
 *                  video stream. OpenCV and Facevacs is then used to identify
 *                  faces.
 *============================================================================
 */

#include <iostream>
#include <string>
#include <list>
#include <sstream>
#include <stdlib.h>
#include <time.h>

/** Header includes.*/
#include <faceVACS.h>

/** Aldebaran includes.*/
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alproxies/altexttospeechproxy.h>
#include <alerror/alerror.h>

/** Opencv includes.*/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"

/** ****************************************Global variable declarations.*************************************************/
using namespace AL;
using namespace std;


/** ****************************************MAIN FUNCTION.****************************************************************/
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Give me the Nao IP Adress'" << endl;
        return 1;
    }

    /** ****************************************General initialisation.*******************************************************/
    const string robotIp(argv[1]);

    int configure = 0; // variable for the terminal interface

    string cfg_file="../../etc/frsdk.cfg";
    string img_file= "../../images/buffer.png" ;
    string cropped_file= "../../images/cropped_face.png" ;
    string fir_file= "../../images/fir" ;

    string phraseToSay= "Found you";

    /** NaoQI related*/
    // Create proxy modules on the robot.
    ALVideoDeviceProxy camProxy(robotIp, 9559);
    ALTextToSpeechProxy tts(robotIp, 9559);
    // Subscribe a client image requiring 640x480 and BGR colorspace.*/
    const string clientName = camProxy.subscribe("NAO_Vid", kVGA , kBGRColorSpace, 10);

    /** openCV related*/
    // Create an cv::Mat header to wrap into an opencv image.*/
    cv::Mat imgHeader = cv::Mat(cv::Size(640, 480), CV_8UC3);
    cv::Mat imgDebug;

    //Read a Image File
    imgDebug =cv::imread("../../images/alice.jpg",1);

    /** FaceVACS related*/
    // Create class of faceVacs named faceVacs
    faceVACS faceVacs;
    // Create list named face_list containing a struct declared in faceVACS.h
    list<found_faces_struct> face_list;
    list<found_faces_struct>::iterator faceIter;
    // Create list named ident_list containing a struct declared in faceVACS.h
    list<identified_faces_struct> ident_list;
    list<identified_faces_struct>::iterator identIter;

    /** Nao bootup */
    //tts.say(phraseToSay);

    /** ****************************************MAIN LOOP.********************************************************************/
    do
    {
        while ((std::cout << "What do you want to do? ") << endl << "1:   Face location loop" <<
               endl << "2:   Face recognition loop" << endl << "3:   Teach in Person" << endl <<
               "4:   Combination of 1 & 2" << endl << "0:   End programm" << endl
               && (!(std::cin >> configure) || configure < 0 || configure > 4))
        {
            std::cout << "That's not a number between 0 and 4; ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        /** ********************************FACE LOCATION LOOP - CONFIGURE ==1 *********************************************/
        if (configure==1)
        {
            /** Exit when pressing ESC.*/
            while ((char) cv::waitKey(30) != 27)
            {
                //debug
                double time1=0.0, tstart;tstart = clock();
                //debug
                /** Retrieve an image from the camera.
              * The image is returned in the form of a container object, with the
              * following fields:
              * 0 = width
              * 1 = height
              * 2 = number of layers
              * 3 = colors space index (see alvisiondefinitions.h)
              * 4 = time stamp (seconds)
              * 5 = time stamp (micro seconds)
              * 6 = image buffer (size of width * height * number of layers)
              */
                ALValue img = camProxy.getImageRemote(clientName);

                /** Access the image buffer (6th field) and assign it to the opencv image
              * container. */
                imgHeader.data = (uchar*) img[6].GetBinary();
                //debug
                //imgHeader = cv::imread("../../images/alice.jpg",1);

                /** Tells to ALVideoDevice that it can give back the image buffer to the
              * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
                camProxy.releaseImage(clientName);


                /** Image has been recieved from NAO and is saved in "imgHEader"
               * begin image processing / face recognition. */

                /** Write image files to disk.*/
                imwrite("../../images/buffer.png", imgHeader);

                // call to face_find. Found faces are listed in face_list
                faceVacs.face_find(cfg_file,img_file,face_list);

                // List iteration to recieve the x.y Position of the found faces
                // Draw rectangles around the found faces with openCV
                faceIter= face_list.begin();
                while (faceIter != face_list.end())
                {
                    cv::rectangle(imgHeader,
                                  cv::Point2f( (*faceIter).pos[0]  - (*faceIter).width, (*faceIter).pos[1] - (*faceIter).width ),
                                  cv::Point2f( (*faceIter).pos[0]  + (*faceIter).width, (*faceIter).pos[1] + 1.5 * (*faceIter).width ),
                                  cv::Scalar( 110, 220, 0 ),
                                  1.5,
                                  8);
                    cv::putText(imgHeader,"Person",
                                cv::Point2f( (*faceIter).pos[0] - (*faceIter).width, (*faceIter).pos[1] - (*faceIter).width - 10),
                                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar( 110, 220, 0 ), 2, 8, false);
                    faceIter++;
                }

                //std::cout << endl << "number of found faces: " << face_list.size () << endl;


                /** Display the openCV images on screen.*/
                //cv::startWindowThread();
                cv::namedWindow( "NaoCam",cv::WINDOW_NORMAL);
                cv::imshow("NaoCam", imgHeader);


                /** Clear the found_face_list buffer or else they will stack..*/
                face_list.clear();
                //debug
                time1 += clock() - tstart; time1 = time1/CLOCKS_PER_SEC; cout << "  time = " << time1 << " sec." << endl;
                //debug
            }
        }

        /** ********************************FACE RECOGNITION - CONFIGURE ==2 ***********************/
        else if (configure == 2)
        {
            /** Exit when pressing ESC.*/
            while ((char) cv::waitKey(30) != 27)
            {
                //debug
                double time1=0.0, tstart;tstart = clock();
                //debug
                ALValue img = camProxy.getImageRemote(clientName);

                /** Access the image buffer (6th field) and assign it to the opencv image
              * container. */
                imgHeader.data = (uchar*) img[6].GetBinary();
                //debug
                imgHeader = cv::imread("../../images/face4.png",1);

                /** Tells to ALVideoDevice that it can give back the image buffer to the
              * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
                camProxy.releaseImage(clientName);

                /** Write image files to disk.*/
                imwrite("../../images/buffer.png", imgHeader);

                faceVacs.compare_to_fir(cfg_file, img_file, fir_file,ident_list);

                identIter= ident_list.begin();


                while (identIter != ident_list.end())
                {
                    cv::rectangle(imgHeader,
                                  cv::Point2f( (*identIter).pos[0]  - (*identIter).width, (*identIter).pos[1] - (*identIter).width ),
                                  cv::Point2f( (*identIter).pos[0]  + (*identIter).width, (*identIter).pos[1] + 1.5 * (*identIter).width ),
                                  cv::Scalar( 110, 220, 0 ),
                                  1.5,
                                  8);
                    if ((*identIter).score_f > 0.5)
                    {
                        char text[255];
                        sprintf(text, "  Score: %f", (*identIter).score_f);
                        cv::putText(imgHeader,(*identIter).name + text,
                                    cv::Point2f( (*identIter).pos[0] - (*identIter).width, (*identIter).pos[1] - (*identIter).width - 10),
                                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar( 110, 220, 0 ), 2, 8, false);
                    }


                    identIter++;
                }

                /** Display the openCV images on screen.*/
                //cv::startWindowThread();
                cv::namedWindow( "NaoCam",cv::WINDOW_NORMAL);
                cv::imshow("NaoCam", imgHeader);

                ident_list.clear();


                //debug
                time1 += clock() - tstart; time1 = time1/CLOCKS_PER_SEC; cout << "  time = " << time1 << " sec." << endl;
                //debug
            }
        }

        /** ********************************Save Pictures of a Person - CONFIGURE ==3 ***********************/
        else if (configure == 3)
        {
            stringstream ss;
            string name_a_person;
            int i = 1;

            string type = ".png";

            cout << "Name of the Person? Without spaces.. use underline '_' " << endl;
            cin >> name_a_person;
            cout << "Creating images with the name: " << name_a_person << endl
                 << "Press Exit to snap a image / 10 images required" << endl;

            string folderCreateCommand = "mkdir ../../images/" + name_a_person;
            system(folderCreateCommand.c_str());  // erstellt einen ordner

            saved_user person;  // neues personen struct
            person.name = name_a_person;

            /** */
            while (i < 11)
            {
                /** Exit when pressing ESC.*/
                while ((char) cv::waitKey(30) != 27)
                {
                    //debug
                    double time1=0.0, tstart;tstart = clock();
                    //debug
                    /** Retrieve an image from the camera. */
                    ALValue img = camProxy.getImageRemote(clientName);

                    /** Access the image buffer (6th field) and assign it to the opencv image
                    * container. */
                    imgHeader.data = (uchar*) img[6].GetBinary();
                    //debug
                    imgHeader = cv::imread("../../images/alice.jpg",1);

                    /** Tells to ALVideoDevice that it can give back the image buffer to the
                    * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
                    camProxy.releaseImage(clientName);


                    /** Image has been recieved from NAO and is saved in "imgHEader"
                     * begin image processing / face recognition. */

                    /** Write image files to disk.*/
                    imwrite("../../images/buffer.png", imgHeader);

                    // call to face_find. Found faces are listed in face_list
                    faceVacs.face_find(cfg_file,img_file,face_list);

                    // List iteration to recieve the x.y Position of the found faces
                    // Draw rectangles around the found faces with openCV
                    faceIter= face_list.begin();
                    while (faceIter != face_list.end())
                    {
                        cv::rectangle(imgHeader,
                                      cv::Point2f( (*faceIter).pos[0]  - (*faceIter).width, (*faceIter).pos[1] - (*faceIter).width ),
                                      cv::Point2f( (*faceIter).pos[0]  + (*faceIter).width, (*faceIter).pos[1] + 1.5 * (*faceIter).width ),
                                      cv::Scalar( 110, 220, 0 ),
                                      1.5,
                                      8);
                        cv::putText(imgHeader,"Person",
                                    cv::Point2f( (*faceIter).pos[0] - (*faceIter).width, (*faceIter).pos[1] - (*faceIter).width - 10),
                                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar( 110, 220, 0 ), 2, 8, false);

                        faceIter++;
                    }

                    //std::cout << endl << "number of found faces: " << face_list.size () << endl;


                    /** Display the openCV images on screen.*/
                    //cv::startWindowThread();
                    cv::namedWindow( "NaoCam",cv::WINDOW_NORMAL);
                    cv::imshow("NaoCam", imgHeader);


                    /** Clear the found_face_list buffer or else they will stack..*/
                    face_list.clear();
                    //debug
                    time1 += clock() - tstart; time1 = time1/CLOCKS_PER_SEC; cout << "  time = " << time1 << " sec." << endl;
                    //debug
                }

                ss<<"../../images/"<<name_a_person<<"/"<<name_a_person<<(i)<<type;
                string fullPath = ss.str();
                ss.str("");
                cout << fullPath << endl ;  //setzt den pfad für imwrite
                /** Write image files to disk.*/

                imgHeader =cv::imread(img_file,1);
                imwrite(fullPath, imgHeader); //change to imgDebug for testing

                person.imgFileLocation.push_back(fullPath);

                i++;
            }

            faceVacs.create_fir(cfg_file,person.imgFileLocation);
        }

        /** ********************************FACE LOCATION LOOP - CONFIGURE ==4 *********************************************/
        if (configure==4)
        {
            /** Exit when pressing ESC.*/
            while ((char) cv::waitKey(30) != 27)
            {
                //debug
                double time1=0.0, tstart;tstart = clock();
                //debug
                /** Retrieve an image from the camera. */
                ALValue img = camProxy.getImageRemote(clientName);

                /** Access the image buffer (6th field) and assign it to the opencv image
              * container. */
                imgHeader.data = (uchar*) img[6].GetBinary();
                //debug
                imgHeader = cv::imread("../../images/test.jpg",1);

                /** Tells to ALVideoDevice that it can give back the image buffer to the
              * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
                camProxy.releaseImage(clientName);


                /** Image has been recieved from NAO and is saved in "imgHEader"
               * begin image processing / face recognition. */

                /** Write image files to disk.*/
                imwrite("../../images/buffer.png", imgHeader);

                // call to face_find. Found faces are listed in face_list
                faceVacs.face_find(cfg_file,img_file,face_list);

                // List iteration to recieve the x.y Position of the found faces
                // Draw rectangles around the found faces with openCV
                faceIter= face_list.begin();
                int ct=0;
                while (faceIter != face_list.end())
                {
                    cv::Mat cropedImage = imgHeader(cv::Rect( (*faceIter).pos[0]  - (*faceIter).width,
                                                              (*faceIter).pos[1] - (*faceIter).width,
                                                              2* (*faceIter).width ,
                                                              2.5* (*faceIter).width)
                                                    );
                    imwrite("../../images/cropped_face.png", cropedImage);


                    faceVacs.compare_to_fir(cfg_file, cropped_file, fir_file,ident_list);

                    cv::rectangle(imgHeader,
                                  cv::Point2f( (*faceIter).pos[0]  - (*faceIter).width, (*faceIter).pos[1] - (*faceIter).width ),
                                  cv::Point2f( (*faceIter).pos[0]  + (*faceIter).width, (*faceIter).pos[1] + 1.5 * (*faceIter).width ),
                                  cv::Scalar( 110, 220, 0 ),
                                  1.5,
                                  8);

                    if (ct==0)
                        identIter= ident_list.begin();

                    if ((*identIter).score_f > 0.5 && (*identIter).score_f < 1)
                    {
                        char text[255];
                        sprintf(text, "  Score: %f", (*identIter).score_f);
                        cv::putText(imgHeader,(*identIter).name + text,
                                    cv::Point2f( (*faceIter).pos[0] - (*faceIter).width, (*faceIter).pos[1] - (*faceIter).width - 10),
                                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar( 110, 220, 0 ), 2, 8, false);
                    }


                    //                                        char file_name[100];
                    //                                        sprintf(file_name, "../../images/face%d.png", ct + 1);
                    //                                        imwrite(file_name, cropedImage);


                    faceIter++;
                    ct++;
                }

                //std::cout << endl << "number of found faces: " << face_list.size () << endl;


                /** Display the openCV images on screen.*/
                //cv::startWindowThread();
                cv::namedWindow( "NaoCam",cv::WINDOW_NORMAL);
                cv::imshow("NaoCam", imgHeader);


                /** Clear the found_face_list buffer or else they will stack..*/
                ident_list.clear();
                face_list.clear();
                //debug
                time1 += clock() - tstart; time1 = time1/CLOCKS_PER_SEC; cout << "  time = " << time1 << " sec." << endl;
                //debug
            }
        }

        /////////////////////////////
        int x= 0;
        while ((char) cv::waitKey(30) != 27) ////////closes openCV windows... there should be an easier way
        {
            if (x==10)
                break;
            cv::destroyAllWindows();
            x++;
        }
        /////////////////////////////


    }while(configure != 0);

    /** ****************************************END MAIN LOOP.****************************************************************/
    /** Cleanup.*/
    camProxy.unsubscribe(clientName);


    //cin.get();
    return 0;
}
