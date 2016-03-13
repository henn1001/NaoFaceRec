#include <iostream>
#include <string>
#include <list>

#include "faceVACS.h"
#include "edialog.h"
#include "idialog.h"


/** FaceVACS includes.*/
#include <frsdk/config.h>
#include <frsdk/face.h>
#include <frsdk/image.h>
#include <frsdk/jpeg.h>
#include <frsdk/ident.h>


/** Opencv includes.*/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;


faceVACS::faceVACS()
{ 
    // TODO Auto-generated constructor stub
}



/** ****************************************FACE FIND.********************************************************************/
void faceVACS::face_find(string cfg_filepath, string image_filepath, list<found_faces_struct>& face_list)
{
    static FRsdk::Configuration cfg( cfg_filepath);
    static FRsdk::Face::Finder faceFinder(cfg);
    FRsdk::Image img( FRsdk::ImageIO::load(string( image_filepath)));

    found_faces_struct face_struct;

    static float mindist = 0.05f;
    static float maxdist = 0.4f;

    // doing face finding
    FRsdk::Face::LocationSet locations = faceFinder.find (img, mindist, maxdist);

    FRsdk::Face::LocationSet::const_iterator faceIter = locations.begin();
    while( faceIter != locations.end())
    {
        face_struct.pos[0] = (*faceIter).pos.x();
        face_struct.pos[1] = (*faceIter).pos.y();
        face_struct.width = (*faceIter).width;
        face_struct.confidence = (*faceIter).confidence;
        face_struct.rotationAngle = (*faceIter).rotationAngle;

        face_list.push_back(face_struct);

        faceIter++;
    }
}



/** ****************************************CREATE FIR.*******************************************************************/
void faceVACS::create_fir(string cfg_filepath, list<string>& imgFileNames)
{
    static FRsdk::Configuration cfg( cfg_filepath);

    if ( imgFileNames.size() == 0 )
    {
        cerr << "There are no input images specified!" << endl;
    }

    cout << "Loading the input images..." << endl;
    FRsdk::SampleSet enrollmentImages;
    list<string>::iterator it = imgFileNames.begin();
    while ( it != imgFileNames.end() )
    {
        const string& imgFileName = *it;
        cout  << "  \"" << imgFileName << "\"" << endl;
        FRsdk::Image img( FRsdk::ImageIO::load( imgFileName ) );
        enrollmentImages.push_back( FRsdk::Sample( img ) );
        ++it;
    }
    cout << "...Done.\n"  << enrollmentImages.size() << " image(s) loaded." << endl;


    if ( enrollmentImages.size() == 0 )
    {
        cerr << "There are no samples to process!" << endl;
    }

    cout << "Start processing ... " << flush;

    // create an enrollment processor
    FRsdk::Enrollment::Processor proc( cfg);

    // create the needed interaction instances
    FRsdk::Enrollment::Feedback feedback ( new EnrolCoutFeedback( "../../images/fir"));

    // do the enrollment
    proc.process( enrollmentImages.begin(),  enrollmentImages.end(), feedback);
    cout << "...Done." << endl;
}



/** ****************************************COMPARE TO FIR.***************************************************************/
void faceVACS::compare_to_fir(string cfg_filepath, string image_filepath, string fir_list,list<identified_faces_struct>& ident_list)
{

    // initialize and resource allocation
    static FRsdk::Configuration cfg( cfg_filepath);

    // load the image for identification
     static FRsdk::SampleSet identificationImages;

    identificationImages.clear();

    identificationImages.push_back( FRsdk::Sample(FRsdk::ImageIO::load( string(image_filepath) ) ) );

    // load the fir population for identification
     FRsdk::Population population( cfg);
    static FRsdk::FIRBuilder firBuilder( cfg);

    ifstream firIn( fir_list.c_str(), ios::in|ios::binary);
    population.append(firBuilder.build(firIn),fir_list);


    // request Score match list size
    FRsdk::ScoreMappings sm( cfg);
    FRsdk::Score score = sm.requestFAR( 0.001f);

    unsigned int numofmatches = 3;

    // create the needed interaction instances
     FRsdk::Identification::Feedback feedback( new IdentifyCoutFeedback());

    // create an identification processor
    FRsdk::Identification::Processor proc( cfg, population);

    // do the identification
    proc.process( identificationImages.begin(), identificationImages.end(),
                  score, feedback, numofmatches);

    // put the found face coords and matching score into the list ( did that in idialog.h )
    ident_list.clear();
    ident_list.push_back(ident_struct);
}

faceVACS::~faceVACS()
{
    // TODO Auto-generated destructor stub
}



