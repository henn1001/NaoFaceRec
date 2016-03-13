// -*- c++ -*-
// Copyright (c) 2002 Cognitec Systems GmbH
//
// $Revision: 1.13 $
//
//
//Couts wegen performance auskommentiert


/** \file
    \brief
*/

#ifndef IDIALOG_H
#define IDIALOG_H

#include <frsdk/ident.h>
#include <fstream>
#include <iostream>
#include "faceVACS.h"

identified_faces_struct ident_struct;

/** Not needed --> double declaration */
//// small helper for tracing purpose
//std::ostream&
//operator<<( std::ostream& o, const FRsdk::Position& p)
//{
//  o << "[" << p.x() << ", " << p.y() << "]";
//  return o;
//}

// the concrete feedback which prints to stdout
class IdentifyCoutFeedback : public FRsdk::Identification::FeedbackBody
{
public:
    ~IdentifyCoutFeedback() {}

    // the feedback interface
    void start()
    {
        //        std::cout << "start" << std::endl;

        ident_struct.name="";
        ident_struct.pos[0]=0;
        ident_struct.pos[1]=0;
        ident_struct.width=0;
        ident_struct.score_f=0;
    }

    void processingImage( const FRsdk::Image& img)
    {
        //    std::cout << "processing image[" << img.name() << "]" << std::endl;
    }

    void eyesFound( const FRsdk::Eyes::Location& eyeLoc)
    {
        //            std::cout << "found eyes at ["<< eyeLoc.first
        //                      << " " << eyeLoc.second << "; confidences: "
        //                      << eyeLoc.firstConfidence << " "
        //                      << eyeLoc.secondConfidence << "]" << std::endl;

        /** Calculate face middlepoint */
        ident_struct.pos[0]= ( eyeLoc.second.x() + eyeLoc.first.x() ) / 2;
        ident_struct.pos[1]= ( eyeLoc.second.y() + eyeLoc.first.y() ) / 2 ;
        ident_struct.width=eyeLoc.second.x() - eyeLoc.first.x();
    }

    void eyesNotFound()
    {
        //    std::cout << "eyes not found" << std::endl;
    }

    void sampleQuality( const float& f) {
        //    std::cout << "Sample Quality: " << f << std::endl;
    }

    void sampleQualityTooLow()
    {
        //    std::cout << "sampleQualityTooLow" << std::endl;
    }

    void matches( const FRsdk::Matches& matches)
    {
        FRsdk::Matches::const_iterator iter = matches.begin();

        while( iter != matches.end()) {
            FRsdk::Match m = *iter;
//            std::cout << "match on fir[" << m.first << "] got Score["
//                      << (float)m.second << "]" << std::endl;

            ///////////////////////////////////////////
            ident_struct.score_f=(float)m.second;
            ident_struct.name=m.first;

            iter++;
        }
    } //matches

    void end()
    {
        //      std::cout << "end" << std::endl;
    }

};

#endif
