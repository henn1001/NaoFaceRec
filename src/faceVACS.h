#ifndef FACEVACS_H_
#define FACEVACS_H_

#include <iostream>
#include <string>
#include <list>

using namespace std;

struct found_faces_struct
{
    float pos[2];
    float width;
    float confidence;
    float rotationAngle;
};

struct saved_user
{
    string name;
    string firLocation;
    list<string> imgFileLocation;
};

struct identified_faces_struct
{
    float pos[2];
    float width;
    float score_f;
    string name;
};


class faceVACS
{
public:
    faceVACS();

    void face_find(string cfg_filepath, string image_filepath, list<found_faces_struct>& face_list);

    void create_fir(string cfg_filepath, list<string>& imgFileNames);

    void compare_to_fir(string cfg_filepath, string image_filepath, string fir_list, list<identified_faces_struct>& ident_list);

    virtual ~faceVACS();

private:



};


#endif /* FACEVACS_H_ */
