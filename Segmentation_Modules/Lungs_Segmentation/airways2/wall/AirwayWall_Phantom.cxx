#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <sys/types.h>
#include <sys/timeb.h>

//#include "lmcurve.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileReader.h"
#include "itkImageSliceIteratorWithIndex.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageFileWriter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkAirwaySectioningImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
//#include "itkShapeLabelObject.h"
//#include "itkLabelMap.h"
//#include "itkBinaryImageToShapeLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkChangeInformationImageFilter.h"

// TODO check for results with other interpolators
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include <sys/stat.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>   // includes all needed Boost.Filesystem declarations
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/IntersectionMatrix.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/WKTWriter.h>
#include <geos/util/GeometricShapeFactory.h>
#include <geos/util/GEOSException.h>
#include <geos/util/IllegalArgumentException.h>
#include <geos/opLinemerge.h>
#include <geos/opPolygonize.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLString.hpp>

#include "tests/xerces_strings.hpp"
#include <memory>   // std::auto_ptr
#include <cstdlib>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <odb/mysql/database.hxx>

#include "tests/ImageEntity.hxx"
#include "tests/ImageEntity-odb.hxx"
#include "tests/Measure.hxx"
#include "tests/Measure-odb.hxx"

namespace xercesc_3_1 {
#define X(str) XStr(str).unicodeForm()
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};
typedef struct
{
	int segnum;
	string nas;
	double fwhmWT;
	double filteredWT;
	double WT;
	double WAPerc;
	double LR;
	double LRCalc;
	double Peri; // perimeter
	double LA; // lumen area
	int measureCount;
	double mPWA;
	double pcWT; // wallthickness (by phase congruency)
	double pcLR; // lumen radius (by phase congruency)
	double pcWAPerc;

} outputSegment ;

typedef struct
{
	string studyName;
	string seriesName;
	double fwhmWT;
	double filteredWT;
	double WT;
	double WAPerc;
	double LR;
	int BC;
	double TL;
	double Peri;
	double LA;
	double NASCount;
	std::vector<outputSegment> allSegs;

} wholeOutput ;

wholeOutput myoutput;


int xmlwriteDoc(DOMDocument *doc) {
	XMLPlatformUtils::Initialize();
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
	int errorCode;
	if (impl != NULL)
	{
		try
		{
		    DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		    DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
		    theOutputDesc->setByteStream(new StdOutFormatTarget());

		    if (doc==NULL){
			DOMDocument* doc = impl->createDocument(
					0,                    // root element namespace URI.
					X("company"),         // root element name
					0);                   // document type object (DTD).

			DOMElement* rootElem = doc->getDocumentElement();

			DOMElement*  prodElem = doc->createElement(X("product"));
			rootElem->appendChild(prodElem);

			DOMText*    prodDataVal = doc->createTextNode(X("Xerces-C"));
			prodElem->appendChild(prodDataVal);

			DOMElement*  catElem = doc->createElement(X("category"));
			rootElem->appendChild(catElem);

			catElem->setAttribute(X("idea"), X("great"));

			DOMText*    catDataVal = doc->createTextNode(X("XML Parsing Tools"));
			catElem->appendChild(catDataVal);

			DOMElement*  devByElem = doc->createElement(X("developedBy"));
			rootElem->appendChild(devByElem);

			DOMText*    devByDataVal = doc->createTextNode(X("Apache Software Foundation"));
			devByElem->appendChild(devByDataVal);

			theSerializer->write(doc, theOutputDesc);
			doc->release();
		} else
			theSerializer->write(doc, theOutputDesc);
			//
			// Now count the number of elements in the above DOM tree.
			//

//			const XMLSize_t elementCount = doc->getElementsByTagName(X("*"))->getLength();
//			std::cout << "The tree just created contains: " << elementCount
//					<< " elements." << std::endl;

			theOutputDesc->release();
			theSerializer->release();

		}
		catch (const OutOfMemoryException&)
		{
			std::cerr << "OutOfMemoryException" << std::endl;
			errorCode = 5;
		}
		catch (const DOMException& e)
		{
			std::cerr << "DOMException code is:  " << e.code << std::endl;
			errorCode = 2;
		}
		catch (...)
		{
			std::cerr << "An error occurred creating the document" << std::endl;
			errorCode = 3;
		}
	}  // (inpl != NULL)
	else
	{
		std::cout << "error in xmlwrite: "<< errorCode << endl;
	}
	XMLPlatformUtils::Terminate();
	return 0;
}

void xmlwrite(wholeOutput o) {

	XMLPlatformUtils::Initialize();
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));

	if (impl != NULL)
	{

		DOMDocument* doc = impl->createDocument(
				0,                    // root element namespace URI.
				X("Output"),         // root element name
				0);                   // document type object (DTD).
		DOMElement* rootElem = doc->getDocumentElement();
		rootElem->setAttribute(X("experimentID"), X("1"));
		rootElem->setAttribute(X("workflowname"), X("Air"));
		DOMElement*  repositoryElem = doc->createElement(X("RepositoryName"));
		rootElem->appendChild(repositoryElem);
		DOMText*    prodDataVal = doc->createTextNode(X("test"));
		repositoryElem->appendChild(prodDataVal);
		DOMElement*  repositoryElem2 = doc->createElement(X("StudyName"));
		rootElem->appendChild(repositoryElem2);
		DOMText*    prodDataVal2 = doc->createTextNode(X(myoutput.studyName.c_str()));
		repositoryElem2->appendChild(prodDataVal2);
		DOMElement*  repositoryElem3 = doc->createElement(X("SeriesName"));
		rootElem->appendChild(repositoryElem3);
		DOMText*    prodDataVal3 = doc->createTextNode(X(myoutput.seriesName.c_str()));
		repositoryElem3->appendChild(prodDataVal3);
		DOMElement*  catElem = doc->createElement(X("Global"));
		DOMElement*  mElem = doc->createElement(X("Measure"));
		mElem->setAttribute(X("Name"), X("WT"));
		mElem->setAttribute(X("Value"), X(boost::lexical_cast<string>( myoutput.WT ).c_str()));
		catElem->appendChild(mElem);

		DOMElement*  mElem2 = doc->createElement(X("Measure"));
		mElem2->setAttribute(X("Name"), X("LR"));
		mElem2->setAttribute(X("Value"), X(boost::lexical_cast<string>( myoutput.LR).c_str()));
		catElem->appendChild(mElem2);


//		DOMElement*  mElem3 = doc->createElement(X("Measure"));
//		mElem3->setAttribute(X("Name"), X("BC"));
//		mElem3->setAttribute(X("Value"), X(boost::lexical_cast<string>( myoutput.LR).c_str()));
//		catElem->appendChild(mElem3);
//
//		DOMElement*  mElem4 = doc->createElement(X("Measure"));
//		mElem4->setAttribute(X("Name"), X("TL"));
//		mElem4->setAttribute(X("Value"), X(boost::lexical_cast<string>( myoutput.LR).c_str()));
//		catElem->appendChild(mElem4);

		rootElem->appendChild(catElem);
		for (int x=0; x<myoutput.allSegs.size(); x++) {
			outputSegment os = myoutput.allSegs.at(x);
			DOMElement*  segElem = doc->createElement(X("Segment"));
			segElem->setAttribute(X("SegmentNum"), X(boost::lexical_cast<string>( os.segnum).c_str()));
			segElem->setAttribute(X("MeasureCount"), X(boost::lexical_cast<string>( os.measureCount).c_str()));
			rootElem->appendChild(segElem);

			DOMElement*  mElem3 = doc->createElement(X("Measure"));
			mElem3->setAttribute(X("Name"), X("FilteredWT"));
			mElem3->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.filteredWT).c_str()));
			segElem->appendChild(mElem3);

			DOMElement*  mElem4 = doc->createElement(X("Measure"));
			mElem4->setAttribute(X("Name"), X("FWHMWT"));
			mElem4->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.fwhmWT).c_str()));
			segElem->appendChild(mElem4);

			DOMElement*  mElem5  = doc->createElement(X("Measure"));
			mElem5->setAttribute(X("Name"), X("WT"));
			mElem5->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.WT).c_str()));
			segElem->appendChild(mElem5);

			DOMElement*  mElem6 = doc->createElement(X("Measure"));
			mElem6->setAttribute(X("Name"), X("LR"));
			mElem6->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.LR).c_str()));
			segElem->appendChild(mElem6);

			DOMElement*  mElem7 = doc->createElement(X("Measure"));
			mElem7->setAttribute(X("Name"), X("CalcLR"));
			mElem7->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.LRCalc).c_str()));
			segElem->appendChild(mElem7);

			DOMElement*  mElem9 = doc->createElement(X("Measure"));
			mElem9->setAttribute(X("Name"), X("mPWA"));
			mElem9->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.mPWA).c_str()));
			segElem->appendChild(mElem9);

			DOMElement*  mElem10 = doc->createElement(X("Measure"));
			mElem10->setAttribute(X("Name"), X("WAPerc"));
			mElem10->setAttribute(X("Value"), X(boost::lexical_cast<string>( os.WAPerc).c_str()));
			segElem->appendChild(mElem10);
		}
		xmlwriteDoc(doc);
	}
}


}

using namespace std;
using namespace boost;
using namespace geos;
using namespace geos::geom;
using namespace geos::operation::polygonize;
using namespace geos::operation::linemerge;


#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#define GEOMETRIC_SHAPES 1
#define RELATIONAL_OPERATORS 1
#define COMBINATIONS 1
#define UNARY_OPERATIONS 1
#define LINEMERGE 1
#define POLYGONIZE 1

//int* getXOrder(vector<Coordinate> c);

GeometryFactory *global_factory;
typedef signed short PixelType;
typedef vector<string> LINE;

bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() > ending.length()) {
    	return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
    	return false;
    }
}

double f( double t, const double *p )
{
    return p[0] + p[1]*(t-p[2])*(t-p[2]);
}

double getMin( vector<PixelType> profile ) {
	double curMin = profile.at(0);
	for (unsigned int i=1; i<profile.size(); i++)
		if (profile.at(i) < curMin) curMin = profile.at(i);
	return curMin;
}
typedef struct
{
	double angle;
	double edge10L;
	double edge10R;
	double alpha;
	double beta;
	double edge10LVal;
	double edge10RVal;
	double minLeftVal;
	double minLeftIdx;
	double maxPeakIdx;
	double idx; // according to segmentation
} rayprofile;


double integralSumOffset(rayprofile r, vector<PixelType> profile) {
	double sum = 0.0;
	vector<PixelType> temp;
	vector<double> x;

	// get offset -- problem is, r.minLeftVal isn't always enough to shift profile up
	double offset = getMin(profile);
//	double offset = r.minLeftVal;

	x.push_back(r.edge10L);
	temp.push_back(r.edge10LVal);
	for (unsigned int i=int(ceil(r.edge10L)); i<int(ceil(r.edge10R)); i++) {
		x.push_back(i);
		temp.push_back(profile.at(i));
	}
	x.push_back(r.edge10R);
	temp.push_back(r.edge10RVal);
    for (unsigned int i = 1; i < temp.size(); i++) { // start + profile + end
    	// x is 1 to profile.size(), y is (profile - r.minLeftVal)
        sum += abs ( (x.at(i) - x.at(i-1)) * ((temp.at(i)-offset) + (temp.at(i-1)-offset)) );
    }
    return sum * 0.5;

}
/*
vector<Coordinate> calcExtPoints(multimap<double, Coordinate> ext ,bool interp ) {

	// <angle, Coordinate(edgeL, edgeR)>
	// x = r cos theta, y = r sin theta

	vector<Coordinate> extPointVect;
	extPointVect.clear();
	vector<Coordinate> firstHalf, secondHalf;
	bool halfway = false;
	int halfwayIndex = 0;

	for (multimap<double, Coordinate >::const_iterator it (ext.begin()), end(ext.end());
	          		  it != end;
	          		  ++it)
	{

//		ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
		double theta = (*it).first;
		Coordinate c = (*it).second;
		double r = c.y;
		setprecision(3);
//		ccout << "pushing out: " <<  r * cos(theta)* 100 / 100 << "," << r * sin(theta)* 100 / 100 << endl;
		extPointVect.push_back(Coordinate(r * cos(theta) * 100 / 100, -(r * sin(theta)* 100 / 100)));
		if (interp && theta>=M_PI) {
			if (!halfway) {
				halfwayIndex = extPointVect.size() - 1;
				halfway = true; // mark halfway as true
			}
		}
	}

	return extPointVect;

}

vector<Coordinate> calcIntPoints(multimap<double, Coordinate> ext, bool interp) {
	vector<Coordinate> intPointVect;
	intPointVect.clear();
	vector<Coordinate> firstHalf, secondHalf;
	bool halfway = false;
	int halfwayIndex = 0;
	for (multimap<double, Coordinate >::const_iterator it (ext.begin()), end(ext.end());
		          		  it != end;
		          		  ++it)
	{

		double theta = (*it).first;
		Coordinate c = (*it).second;
		double r = c.x;
//		ccout << "pushing out: " <<  (r * cos(theta)* 10) / 10 << "," << (r * sin(theta)* 10) / 10 << endl;
		intPointVect.push_back(Coordinate((r * cos(theta)* 10) / 10, -(r * sin(theta)* 10) / 10));
		if (interp && theta>=M_PI) {
			if (!halfway) {
				halfwayIndex = intPointVect.size() - 1;
				halfway = true; // mark halfway as true
			}
		}
	}

	return intPointVect;
}

bool MyDataSortPredicate(const rayprofile& d1, const rayprofile& d2)
{
  return abs(d1.edge10L - d1.edge10R) < abs(d2.edge10L - d2.edge10R);
}
*/
class LineVector
{
public:
    float x_, y_;

    LineVector(float f = 0.0f)
        : x_(f), y_(f) {}

    LineVector(float x, float y)
        : x_(x), y_(y) {}
};


class LineSegment
{
public:
    LineVector begin_;
    LineVector end_;

    LineSegment(const LineVector& begin, const LineVector& end)
        : begin_(begin), end_(end) {}

    enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERSECTING, INTERSECTING };

    IntersectResult Intersect(const LineSegment& other_line, LineVector& intersection)
    {
        float denom = ((other_line.end_.y_ - other_line.begin_.y_)*(end_.x_ - begin_.x_)) -
                      ((other_line.end_.x_ - other_line.begin_.x_)*(end_.y_ - begin_.y_));

        float nume_a = ((other_line.end_.x_ - other_line.begin_.x_)*(begin_.y_ - other_line.begin_.y_)) -
                       ((other_line.end_.y_ - other_line.begin_.y_)*(begin_.x_ - other_line.begin_.x_));

        float nume_b = ((end_.x_ - begin_.x_)*(begin_.y_ - other_line.begin_.y_)) -
                       ((end_.y_ - begin_.y_)*(begin_.x_ - other_line.begin_.x_));

        if(denom == 0.0f)
        {
            if(nume_a == 0.0f && nume_b == 0.0f)
            {
                return COINCIDENT;
            }
            return PARALLEL;
        }

        float ua = nume_a / denom;
        float ub = nume_b / denom;
        if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
        {
            // Get the intersection point.
            intersection.x_ = begin_.x_ + ua*(end_.x_ - begin_.x_);
            intersection.y_ = begin_.y_ + ua*(end_.y_ - begin_.y_);

            return INTERSECTING;
        }

        return NOT_INTERSECTING;
    }
};

float DoLineSegmentIntersection(const LineVector& p0, const LineVector& p1, const LineVector& p2, const LineVector& p3)
{
    LineSegment linesegment0(p0, p1);
    LineSegment linesegment1(p2, p3);

    LineVector intersection;

//    std::cout << "Line Segment 0: (" << p0.x_ << ", " << p0.y_ << ") to (" << p1.x_ << ", " << p1.y_ << ")\n"
//              << "Line Segment 1: (" << p2.x_ << ", " << p2.y_ << ") to (" << p3.x_ << ", " << p3.y_ << ")\n";

    switch(linesegment0.Intersect(linesegment1, intersection))
    {
    case LineSegment::PARALLEL:
//        std::cout << "The lines are parallel\n\n";
        break;
    case LineSegment::COINCIDENT:
//        std::cout << "The lines are coincident\n\n";
        break;
    case LineSegment::NOT_INTERSECTING:
//        std::cout << "The lines do not intersect\n\n";
        break;
    case LineSegment::INTERSECTING:
//    	std::cout << "The lines intersect at (" << intersection.x_ << ", " << intersection.y_ << ")\n\n";
    	return intersection.x_;
//        std::cout << "The lines intersect at (" << intersection.x_ << ", " << intersection.y_ << ")\n\n";
        break;
    }
    return -1.0;
}

class sortCompare
    {
    public:
         bool operator()(const double s, const double t) const
         {
              return (s<t);
         }
    };

namespace odb {

int lookup(string measureName) {
	try
	{
		// Create a few persistent person objects.
		{
			auto_ptr<database> db (new mysql::database ("odb_test", "", "odb_test", "localhost"));

			typedef odb::query<Measure> query;
			typedef odb::result<Measure> result;

			int measure_id;
			{
				transaction t (db->begin ());

				result r (db->query<Measure> (query::name == measureName));

				for (result::iterator i (r.begin ()); i != r.end (); ++i)
				{
					//						cout << "'WT' id: " << i->name () <<  endl;
//					cout << "Name: "<< i->name() <<", id: " << i->id () <<  endl;
					measure_id = i->id();
					//						cout << "'WT' id: " << i->first () <<  endl;
				}
				return measure_id;

			}


		}
	}
	catch (const odb::exception& e)
	{
		cerr << e.what () << endl;
		return -1;
	}
}

void persist(string studyName, string seriesName, int measureid, int localid, double value, double lambda, int expID, int generation, int sliceidx) {

	ImageEntity measured (seriesName, studyName, measureid, localid, value, lambda, expID, generation, sliceidx);

	auto_ptr<database> db (new mysql::database ("odb_test", "", "odb_test", "localhost"));
	transaction t2 (db->begin ());

	// Make objects persistent and save their ids for later use.
	//
	db->persist (measured);

	t2.commit ();

}

void persistWholeOutput(xercesc_3_1::wholeOutput o, double lambda, int expID) {

	// save globals
	int mid = lookup("WT");
	persist(o.studyName, o.seriesName, mid, -1,  o.WT, lambda, expID, -1, -1);
	mid = lookup("LR");
	persist(o.studyName, o.seriesName, mid, -1,  o.LR, lambda, expID, -1, -1);
	mid = lookup("FilteredWT");
	persist(o.studyName, o.seriesName, mid, -1,  o.filteredWT, lambda, expID, -1, -1);
	mid = lookup("WAPerc");
	persist(o.studyName, o.seriesName, mid, -1,  o.WAPerc, lambda, expID, -1, -1);
	mid = lookup("LA");
	persist(o.studyName, o.seriesName, mid, -1,  o.LA, lambda, expID, -1, -1);

//	int mid = lookup("WT");
//	persist(o.studyName, o.seriesName, mid, -1, boost::lexical_cast<std::string>( o.WT ), lambda, expID);
//	mid = lookup("LR");
//	persist(o.studyName, o.seriesName, mid, -1, boost::lexical_cast<std::string>( o.LR ), lambda, expID);
//	mid = lookup("FilteredWT");
//	persist(o.studyName, o.seriesName, mid, -1, boost::lexical_cast<std::string>( o.filteredWT ), lambda, expID);

	// save local segments
	for (int x=0; x<o.allSegs.size(); x++) {
		xercesc_3_1::outputSegment localSeg = o.allSegs.at(x);
		mid = lookup("WT");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.WT , lambda, expID, -1, -1);
		mid = lookup("LR");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.LR , lambda, expID, -1, -1);
		mid = lookup("FilteredWT");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.filteredWT , lambda, expID, -1, -1);
		mid = lookup("FWHMWT");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.fwhmWT , lambda, expID, -1, -1);
		mid = lookup("CalcLR");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.LRCalc , lambda, expID, -1, -1);
		mid = lookup("mPWA");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.mPWA , lambda, expID, -1, -1);
		mid = lookup("LA");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.LA , lambda, expID, -1, -1);
		mid = lookup("WAPerc");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.WAPerc , lambda, expID, -1, -1);
		mid = lookup("PhaseCongWT");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.pcWT , lambda, expID, -1, -1);
		mid = lookup("PhaseCongLR");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.pcLR , lambda, expID, -1, -1);
		mid = lookup("PhaseCongWAPerc");
		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  localSeg.pcWAPerc , lambda, expID, -1, -1);
//		mid = lookup("WT");
//		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  boost::lexical_cast<std::string>( localSeg.WT ), lambda, expID);
//		mid = lookup("LR");
//		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  boost::lexical_cast<std::string>( localSeg.LR ), lambda, expID);
//		mid = lookup("FilteredWT");
//		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  boost::lexical_cast<std::string>( localSeg.filteredWT ), lambda, expID);
//		mid = lookup("FWHMWT");
//		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  boost::lexical_cast<std::string>( localSeg.fwhmWT ), lambda, expID);
//		mid = lookup("CalcLR");
//		persist(o.studyName, o.seriesName, mid, localSeg.segnum,  boost::lexical_cast<std::string>( localSeg.LRCalc ), lambda, expID);
	}
}

}

double calculateTotalError(xercesc_3_1::wholeOutput o, string truevalues, bool perc) {

	// read true values
	ifstream trueStream;
	trueStream.open(truevalues.c_str());
	string line, tmp;
	typedef tokenizer< escaped_list_separator<char> > Tokenizer;
	map<int, double> trueMap;

//	cout << "opening truevalues at: "<< truevalues << "\n";
	if (trueStream){
		int loc=1, mid=-1, sid;
	    while (getline(trueStream,line))
	    {

	    	Tokenizer tok(line);
	    	// assumes there's only 1 MeasureID to be compared

	    	double val = 0.0;

	    	loc = 1;
	    	for (Tokenizer::iterator it(tok.begin()), end(tok.end());
	    	     it != end; ++it)
	    	{
	    	    if (loc==1 && mid==-1) // measureid
	    	    {
	    	    	tmp =  ((*it));
	    	    	if (atoi(tmp.c_str())==0) {
	    	    		break;
	    	    	}
	    	    	mid = atoi(tmp.c_str());
	    	    }
	    	    else if (loc==2) { // segment id
	    	    	tmp =  ((*it));
	    	    	if (atoi(tmp.c_str())==0) {
	    	    		break;
	    	    	}
	    	    	sid = atoi(tmp.c_str());
	    	    }
	    	    else if (loc==3) { // value
	    	    	tmp =  ((*it));
	    	    	if (atof(tmp.c_str())==0) {
	    	    		break;
	    	    	}
	    	    	val = atof(tmp.c_str());
//	    	    	cout << "insert: "<< sid << ", " << val << "\n";
	    	    	trueMap.insert(pair<int,double>(sid , val));
	    	    }
	    	    loc++;
	    	}
	    }
		// to compare the csv given (measureid, localid, value) with the wholeOutput
	    double error = 0.0;
	    double errperc = 0.0;
	    int vcount = 0;
		for (int x=0; x<o.allSegs.size(); x++) {
			xercesc_3_1::outputSegment localSeg = o.allSegs.at(x);

			double tv = trueMap.find(localSeg.segnum)->second;

			double measured;
			if (mid==odb::lookup("FWHMWT"))
				measured = abs(tv - localSeg.WT);
			else if (mid==odb::lookup("FilteredWT"))
				measured = abs(tv - localSeg.filteredWT);
			else if (mid==odb::lookup("WT"))
				measured = abs(tv - localSeg.WT);
			else if (mid==odb::lookup("LR"))
				measured = abs(tv - localSeg.LR);
			else if (mid==odb::lookup("CalcLR"))
				measured = abs(tv - localSeg.LRCalc);
			else if (mid==odb::lookup("WAPerc"))
				measured = abs(tv - localSeg.WAPerc);
			else if (mid==odb::lookup("LA"))
				measured = abs(tv - localSeg.LA);
			else if (mid==odb::lookup("PhaseCongWT"))
				measured = abs(tv - localSeg.pcWT);
			else if (mid==odb::lookup("PhaseCongLR"))
				measured = abs(tv - localSeg.pcLR);
			if (measured != measured) continue; // detect nan
			error += measured;
			errperc += measured/tv;
			vcount++;
//			if (mid==odb::lookup("FilteredWT"))
//				cout << "Finding : "<< localSeg.segnum << ", getting filteredWT: "<< localSeg.filteredWT <<"\n";
//			else
//				cout << "Finding : "<< localSeg.segnum << ", getting: "<< tv << ", error: "<< measured <<"\n";
		}
		if (perc)
			return errperc*100 / vcount;
		else
			return error / vcount;
	}
	return 0.0;
}

double calculateTotalErrorPerc(xercesc_3_1::wholeOutput o, string truevalues) {
	return calculateTotalError(o, truevalues, true);
}

vector<double> getDerivative(vector<PixelType> profile) {
	vector<double> res;

	for (int j=1; j<profile.size()-1; j++) {
		res.push_back(profile.at(j) - profile.at(j-1));
	}
	res.push_back(res.at(res.size()-1));
	return res;
}

vector<float> calculatePC_WallThickness(vector<double> derivatives1, vector<double> derivatives2, vector<float> intersection, bool interp) {

	vector<float> res;
	// get intersections which are not -1, pick those with greatest slope
	float x_l = 0.0, x_r = 0.0;
	float max_grad_l = 0.0, max_grad_r = 0.0;
//	std::cout << "\n[calcPc "<< interp << "] vector length: "<< intersection.size() << "\n";
	for (unsigned int i=0; i<intersection.size(); i++) {
// FIXME - scale isn't correct - need to check derivative over larger interval
		if (intersection.at(i) == -1.0) continue;
//		std::cout << i << ", " << intersection.at(i) << ", d1 " << derivatives1.at(i) << "d2 "<< derivatives2.at(i) << "\n";
		float deriv_comp1 = 0.0, deriv_comp2 = 0.0;
		int neighb_count = 0;
		int lb = 20, ub = 0;
		for (int x=-10; x<10; x++) {
			if (i+x >= 0 && ((i+x) <intersection.size())) {
//				if (interp) {
//					std::cout << "Adding up derivs (1st curve): " << derivatives1.at(i+x) << ", (2nd curve): " << derivatives2.at(i+x) << "\n";
//				}
				deriv_comp1 += derivatives1.at(i+x);
				deriv_comp2 += derivatives2.at(i+x);
				neighb_count++;
			}
		}
//		deriv_comp1 /= neighb_count;
//		deriv_comp2 /= neighb_count;
//		if (interp)
//			std::cout << "(INTERP) loc, deriv1 "<< deriv_comp1 << ", deriv2 "<< deriv_comp2 << ", max_grad_l: "<< max_grad_l <<", max_grad_r: "<< max_grad_r <<"\n";
//		else
//			std::cout << "(DISC) loc "<< intersection.at(i) <<", deriv1 "<< derivatives1.at(i)<< ", deriv2 "<< derivatives2.at(i) << ", max_grad_l: "<< max_grad_l <<", max_grad_r: "<< max_grad_r <<"\n";
		if ((interp && (deriv_comp1 > 0) && (deriv_comp2 > 0) && abs((long) deriv_comp1) > max_grad_l) ||
				(!interp && derivatives1.at(i) > 0 && derivatives2.at(i) > 0 && abs((long) derivatives1.at(i)) > max_grad_l)) {// both must be of the same sign
			x_l = intersection.at(i);
			if (!interp)
				max_grad_l = abs ((long) derivatives1.at(i) );
			else
				max_grad_l = abs ((long) deriv_comp1 );
//			std::cout << intersection.at(i) << ", Rising slope: " << derivatives1.at(i) << ", Comp: "<< deriv_comp1 << "\n";
		} else if ((interp && deriv_comp1 < 0 && deriv_comp2 < 0 && abs((long) deriv_comp1) > max_grad_r) ||
				(!interp && derivatives1.at(i) < 0 && derivatives2.at(i) < 0 && abs((long) derivatives1.at(i)) > max_grad_r )) {// both must be negative
			x_r = intersection.at(i);
			if (!interp)
				max_grad_r = abs ((long) derivatives1.at(i) );
			else
				max_grad_r = abs ((long) deriv_comp1 );
//			std::cout << intersection.at(i) << ", Downward slope: " << derivatives1.at(i) << ", Comp: "<< deriv_comp1 <<"\n";
		}
	}
//	std::cout << "max_grad_l " << max_grad_l << ", max_grad_r: "<< max_grad_r << ", x_l: "<< x_l <<", x_r: "<< x_r <<"\n";
	res.push_back(x_l); // lumen
	res.push_back(x_r - x_l); // thickness
	return res;
}

static void usage()
{
	std::cout << "\n"
			"\nUsage: "
			"\nAirwayWall_Phantom base_dir segment_tree.csv binaryImage originalImage altImageForPC_fullPath result_base_dir num_of_rays lambda saveToPersistence xmlMode zspacing experimentID suppressOutput trueValues.csv useMedianRays individualDataCapture\n"
			<< std::endl;
}

int main( int argc, char ** argv )
{

  // Verify the number of parameters in the command line
	if( argc < 17 )
	    {
	    usage();
	    return -1;
	    }
	struct timeb initialTime;
	struct timeb actualTime;
	struct timeb time0;
	ftime(&initialTime);
	ftime(&time0);

	ifstream inputStream;
	//   inputStream.open(argv[2]);
	   string base_dir = argv[1];


	   std::stringstream segments;
	   segments << argv[1]<< "/" << argv[2];
	   string segmentsFile = segments.str();

	   std::stringstream origImg;
	   origImg << argv[1]<< "/" << argv[4];
	   string origImgFile = origImg.str();

	   std::stringstream pcImg;
	   pcImg << argv[5];
	   string pcImgFile = pcImg.str();

	   std::stringstream binImg;
	   binImg << argv[1]<< "/" << argv[3];
	   string binImgFile = binImg.str();

	   string resultBaseDir = "";
	   resultBaseDir = argv[6];

	   unsigned int num_of_rays = atoi(argv[7]);

	   // for 60f kernel (sharper), higher lambda
	   double lambda = 1;
	   // for 40f kernel, smaller lambda
	   //  lambda = 1.043;

	   vector<double> lambda_array;

	   if (atof(argv[8])==0) { // not double!thus an array
//(0.3,0.4,0.5,0.6)
		   string l = argv[8];
		   char_separator<char> sep(",\'[] ");
		   tokenizer< char_separator<char> > tokL(l, sep);

//		   Tokenizer tokL(l);
		   for (tokenizer< char_separator<char> >::iterator itt(tokL.begin()), end(tokL.end()); itt != end; ++itt) {

			   l =  ((*itt)); // reusing l!
			   lambda_array.push_back(atof(l.c_str()));
//			   cout << l << "\n" ;
		   }
//		   lambda = 0.9;
	   } else {
		   lambda = atof(argv[8]);
	   }
//	   return 0;
	   int saveToPersistence = 0;
	   saveToPersistence = atoi(argv[9]);

	   int xmlMode = 0;
	   xmlMode = atoi(argv[10]);

	   int zspacing = 0;
	   zspacing = atoi(argv[11]);

	   int expID = 0;
	   expID = atoi(argv[12]);

	   int suppressOutput = 0;
	   suppressOutput = atoi(argv[13]);

	   string truevalues;
	   if (suppressOutput) { // oppressed only when calculating the global error
		   std::stringstream truevaluescsv;
		   truevaluescsv << argv[14];
		   truevalues = truevaluescsv.str();

	   }
	   else {

		   cout << "BASE DIR: " << base_dir << endl;
		   cout << "resultBaseDir: "<< resultBaseDir << endl;
		   if (lambda_array.size()==0)
			   cout << "lambda: " <<lambda<< endl;
		   else {
			   cout << "lambda: ";
			   for (int r=0; r<lambda_array.size(); r++) cout << lambda_array.at(r) << ",";
		   }
		   cout << "saveToPersistence: "<< saveToPersistence << endl;
		   cout << "xmlMode: "<< xmlMode << endl;
		   cout << "zspacing: "<< zspacing << endl;
		   cout << "experimentID: "<< expID << endl;
	   }

	   int useMedianRays = 0;
	   useMedianRays = atoi(argv[15]);

	   int individualDataCapture = 0;
	   individualDataCapture = atoi(argv[16]);

//   inputStream.open(argv[2]);
   // segments.CSV
   inputStream.open(segmentsFile.c_str());
  if( !inputStream ) 
    {
     cerr << "Error opening input stream" << endl;
    }

   vector<int> GS_Files;

// Typedefs
  const int Dimension = 3;


  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType_3D;       
  typedef itk::ImageFileWriter< itk::Image<PixelType, 2> > WriterType_2D;
  typedef itk::Rigid3DTransform< double > TransformType;
  typedef itk::LookAtTransformInitializer< TransformType, ImageType > TransformInitalizerType;
  typedef itk::ResampleImageFilter<ImageType, ImageType >  FilterType;
  typedef itk::ImageRegionIterator< ImageType > IteratorType;
  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
  typedef itk::MinimumMaximumImageCalculator< ImageType > MinCalcType;

  // convert the image in a collection of objects

  typedef itk::AirwaySectioningImageFilter< ImageType > AirwaySectionType;
  typedef AirwaySectionType::InputVectorType VectorType;  
  AirwaySectionType::Pointer AirwaySectioner = AirwaySectionType::New();
  AirwaySectionType::Pointer AirwaySectionerBin = AirwaySectionType::New();
  AirwaySectionType::Pointer AirwaySectionerPC = AirwaySectionType::New();
  ImageType::Pointer image = ImageType::New();

  PixelType globalMin, globalMax;

  VectorType Center, CenterBin, CenterPC;
  VectorType Direction, DirectionBin, DirectionPC;
  VectorType Up;

  // TODO find out how to determine this exactly.. for Gaussian & PSF-dependent

  float Radius=0;
  string f_out_GS = "";
  // set to true to segment with binary thresholded image
  std::stringstream f_profile, f_base;

  if ( inputStream ) 
    {

	  ReaderType::Pointer readerBin = ReaderType::New();
	  ReaderType::Pointer readerGS = ReaderType::New();
	  ReaderType::Pointer readerPC = ReaderType::New();

    readerBin->SetFileName(binImgFile.c_str());
    readerBin->Update( );
    // reads original image
    readerGS->SetFileName(origImgFile.c_str());
    readerGS->Update( );
    readerPC->SetFileName(pcImgFile.c_str());
    readerPC->Update( );

//    map_type parentChildTree;
    multimap<int, int> parentChildTree;

    ImageType::IndexType start; //Initialization indexes of index-matrix.
    start[0] =   0;  // first index on X
    start[1] =   0;  // first index on Y
    start[2] =   0;  // first index on Z

    ImageType::SizeType  size;

    size = readerGS->GetOutput()->GetLargestPossibleRegion().GetSize();

    string s;
    string studyName, seriesName;
    string histogram;

    vector< string > vec;

    multimap<int, int> validityScore;
    multimap<int, double> totalScore;

    multimap<int, double> gWT_sortIBM, gWT_IBM, gER_IBM, gIR_IBM, gIRcalc_IBM, gPWA, gWA_IBM, gLA;
//    multimap<int, double> gWA_IBM, gWA_IBM_circ, gWAmm2_IBM;
    multimap<int, double> gWT, gER, gIR;
//    multimap<int, double> gWA, gWAmm2, gPerim;

    multimap<double, Coordinate > extPoints;
    multimap<double, Coordinate > extPointsBin;
    multimap<double, Coordinate > extPointsIBM;
    multimap<double, Coordinate, sortCompare> toSortIBM;

//    map<double, Coordinate> rayProfile;
//    map<double, Coordinate> thickV;

    multimap<int, VectorType> centroidVect;
    multimap<int, VectorType> normVect1;

    string line;

	// reset GS_Files, TODO to comment later
    GS_Files.clear();
    int segIdx=0;

    // create base output directory
	std::vector<std::string> strs;
	boost::split(strs, base_dir, boost::is_any_of("/"));

	studyName =  strs.at(strs.size()-2);
	if (suppressOutput==0)
		cout << "studyName: "<< studyName << endl;

	xercesc_3_1::myoutput.studyName = studyName;

	seriesName =  strs.at(strs.size()-1);
	if (suppressOutput==0)
		cout << "seriesName: "<< seriesName << endl;
	xercesc_3_1::myoutput.seriesName = seriesName;

	if (resultBaseDir.size() > 0)
		f_base << resultBaseDir << "/" << studyName << "/";

	boost::filesystem::path p2base (f_base.str());
	boost::filesystem::create_directory(p2base);
	f_base << seriesName << "/";
	boost::filesystem::path p2basename (f_base.str());
	create_directory(p2basename);
	std::stringstream logfile;
	logfile << f_base.str() << "log.txt";
	ofstream ccout (logfile.str().c_str());

	vector<int> segmentNumbers;

    while (getline(inputStream,line))
    {

    	segIdx++;
    	typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    	Tokenizer tok(line);
    	double sx=0.0, sy=0.0, sz=0.0;
    	double mx=0.0, my=0.0, mz=0.0;

// 		first pass, store all segmentIDs vs startCentroid coords
//    	second pass, calc plane based on parent segment ID: startCentroid(current) - startCentroid()
//    	third pass, (optional) if length is too long, calc plane over smaller intervals
//    	fourth pass, resample image & store in FS

    	// 1st Pass: READING ROUTINE:
    	int loc=1;
    	int slices=0;
    	int	pointcount=0;

    	string child, parent;
    	VectorType temp;
		// keep 1st & last
    	double sxF=0.0, syF=0.0, szF=0.0;
    	double sxL=0.0, syL=0.0, szL=0.0;

    	for (Tokenizer::iterator it(tok.begin()),
    	                         end(tok.end());
    	     it != end; ++it)
    	{

			// v1: SegmentID, ParentSegmentID, Length, Radius, StartCentroid, EndCentroid, Orientation
    		// v2: SegmentID, ParentSegmentID, Length, Radius, StartCentroid, EndCentroid, Orientation, FinalOrientation
    		// v3: SegmentID, ParentSegmentID, Length, Radius, StartCentroid, MidCentroid, EndCentroid, Orientation, FinalOrientation
    	    if (loc==1) // segmentID
    	    {
    	    	child =  ((*it));
    	    	if (atoi(child.c_str())==0) {
    	    		break;
    	    	}
    	    	// new struct
//    	    	ccout << "Reading segment #" << child << " into tree" << endl;
    	    }
    	    else if (loc==2)
    	    {
    	    	parent = ((*it));

    	    	parentChildTree.insert(pair<int, int>(atoi(parent.c_str()), atoi(child.c_str())));

    	    	ccout << "Inserting segment#" << child << " w parent segment #" << parent <<" into tree" << endl;
    	    }
    	    else if (loc==3 || loc==4)
    	    {
    	    	// ignore length, radius for now
    	    }
    	    else if (loc==5) // pointcount
    	    {
    	    	string pcount = ((*it));
				pointcount = atoi(pcount.c_str());
				ccout << "point count: " << pointcount << endl;
    	    }
    	    else if (loc==6)
    	    {
    	    	// save start centroid
    	    	string startC = ((*it));

// parse diff points

    	    	//parse x,y,z

//    	    	ccout << "sx, sy, sz: ";
    	    	short idx=1;
				string tmp;
				sx=0.0, sy=0.0, sz=0.0;

				char_separator<char> sep(",\'[] ");
				tokenizer< char_separator<char> > tok2(startC, sep);

				ccout << "trying to parse "<< pointcount << " points!"<< endl;
				for (tokenizer< char_separator<char> >::iterator it2(tok2.begin()), end(tok2.end()); it2 != end; ++it2) {
					tmp = ((*it2));
					//					ccout << tmp << endl ;
					if (idx%3==1) {
						// need to strip
						sx = strtod(tmp.c_str(), NULL) ;
//						ccout << sx << ",";
					}
					else if (idx%3==2) {
						sy = strtod(tmp.c_str(), NULL);
//						ccout << sy << ",";
					}
					else if (idx%3==0) {
						sz = strtod(tmp.c_str(), NULL);
//						ccout << sz ;
					}

					if (idx%3==0){ // insert into centroidVect
						temp[0] = sx;
						temp[1] = sy;
						temp[2] = sz;
//						ccout << "combining sx, sy, sz: " << sx << "," << sy << "," << sz << endl;
						slices++;
						centroidVect.insert(pair<int,VectorType>(atoi(child.c_str()) , temp));
					}
					if (idx==3) { // keep first
						sxF = sx; syF = sy; szF = sz;
					}

					idx++;
				}
				// keep last
				sxL = sx; syL = sy; szL = sz;
				segmentNumbers.push_back(atoi(child.c_str()));
    	    }
    	    else if (loc==7)
    	    {
    	    	// save 1st normal
    	    	string midC = ((*it));
    	    	//parse
    	    	typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    	    	Tokenizer tok4(midC);

    	    	mx=0.0, my=0.0, mz=0.0;
    	    	short idx=1;
    	    	string tmp;

    	    	for (Tokenizer::iterator it3(tok4.begin()), end(tok4.end()); it3 != end; ++it3) {
    	    		tmp = ((*it3));
    	    		if (idx==1) {
    	    			// need to strip
    	    			tmp = tmp.substr(1, tmp.length()-1);
    	    			mx = strtod(tmp.c_str(), NULL);
    	    		}
    	    		if (idx==2) {
    	    			my = strtod(tmp.c_str(), NULL);
    	    		}
    	    		if (idx==3) {
    	    			tmp = tmp.substr(0, tmp.length()-1);
    	    			mz = strtod(tmp.c_str(), NULL);
    	    		}
    	    		idx++;
    	    	}
//    	    	ccout << "combining sx, sy, sz: " << mx << "," << my << "," << mz << endl;
    	    	temp[0] = mx;
    	    	temp[1] = my;
    	    	temp[2] = mz;
    	    	if (mx==0 && my==0 && mz==0) { // need to calculate
//    	    		ccout << "Dir all Zero! attempting to calculate norm...";
    	    		// make sure 1st & last is not 0
    	    		if (!(sxL == 0 && syL == 0 && szL == 0 && sxF == 0 && syF == 0 && szF == 0)) {
    	    			// last pt - first pt
    	    			temp[0] = sxL - sxF;
    	    			temp[1] = syL - syF;
    	    			temp[2] = szL - szF;
    	    		}

    	    	}
    	    	normVect1.insert( pair<int,VectorType>(atoi(child.c_str()), temp ));

    	    }
    	    else if (loc==8) // discard the 2nd normal
    	    {
    	    	GS_Files.push_back(slices); // slices now keep count how many points
    	    }
    	    loc++;
    	}

    }
    // GS_Files entries
    ccout << endl;
    ccout << "BEGIN READING ... # of segments to read: " << GS_Files.size() << endl;

    // DEBUG
//    return 0;

    // for each segment,
//    1 create directory Slices/Segment_X
//    2 if there are no slices do nothing, else create image with N number of z-slices


    // START READING FILE HERE
// reader is defined above (to read original image?)
    ImageType::Pointer inputBin;
    ImageType::Pointer input;
    ImageType::Pointer inputPC;
    inputBin = readerBin->GetOutput();
    input = readerGS->GetOutput();
    inputPC = readerPC->GetOutput();

    // Create transform
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    // Setup output parameters
    size = input->GetLargestPossibleRegion().GetSize();

    // in output image there is only a plain slice!
    size[2]=1;

    ImageType::SpacingType spacing, spacing2, spacing3;
    spacing = input->GetSpacing();
    spacing2 = inputBin->GetSpacing();
    spacing3 = inputPC->GetSpacing();
    if (suppressOutput==0) {
    	cout << "spacing: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
    	cout << "spacing (binary): " << spacing2[0] << ", " << spacing2[1] << ", " << spacing2[2] << std::endl;
    	cout << "spacing (PC): " << spacing3[0] << ", " << spacing3[1] << ", " << spacing3[2] << std::endl;
    }
//    input->SetSpacing(spacing);
//    inputBin->SetSpacing(spacing);

    // Create transform initializer
    TransformInitalizerType::PointType center;
    TransformInitalizerType::PointType inputOrigin;
    inputOrigin[0] = 0;
    inputOrigin[1] = 0;
    inputOrigin[2] = 0;
    input->SetOrigin( inputOrigin );
    inputBin->SetOrigin( inputOrigin );

    TransformInitalizerType::PointType outputOrigin;
    // do nothing about translate!
    outputOrigin[0] = 0; //size[0] * 0.5 * spacing[0];
    outputOrigin[1] = 0; //size[1] * 0.5 * spacing[1];
    outputOrigin[2] = 0;

    if (spacing[2]==1) {
    	//  	  cout << "(Optional - default is 1) Please input a z-spacing value to continue" << endl;
    	//  	  cin >> z_spacing;
    	spacing[2] = zspacing;
    	//  	  spacing[2] = z_spacing;
    	if (suppressOutput==0)
    		cout << "New value of z-spacing: "<< spacing[2];
    	input->SetSpacing(spacing);
    }

    ccout << "Effective spacing: "<< spacing << endl;

	AirwaySectioner->SetInput( input );
	AirwaySectionerBin->SetInput(inputBin );
	AirwaySectionerPC->SetInput(inputPC );

//    AirwaySectioner->SetInput(input );
//    AirwaySectionerBin->SetInput(inputBin );

    // END OF FILE READING SETUP

	std::stringstream file_summary, file_summaryP, file_sp_fwhm, file_sp_ibm;
	if (hasEnding(argv[3], "all.csv")) {
		file_summary << f_base.str() << "/Summary_ALL.txt";
		file_summaryP << f_base.str() << "/Summary_mm_ALL.txt";
		file_sp_fwhm << f_base.str() << "/FWHM_mm_periWT.txt";
		file_sp_ibm << f_base.str() << "/IBM_mm_periWT.txt";
	} else {
		file_summary << f_base.str() << "/Summary.txt";
		file_summaryP << f_base.str() << "/Summary.txt";
	}
	string auxS = file_summary.str();
	ofstream fout3 (auxS.c_str());
	ofstream fout3p (file_summaryP.str().c_str());
//	ofstream coutSfwhm (file_sp_fwhm.str().c_str());
//	ofstream coutSibm (file_sp_ibm.str().c_str());

	/* in order to use xmlparse / writer, need to initialize(), and terminate() */
//	XMLPlatformUtils::Initialize();
//	xmlparse(xmlFile);
//	xmlwriteplain();
//  XMLPlatformUtils::Terminate();


	// how to read the number of segments instead of starting always from 1? what if they're not sequential?

	double avgAngle = 0.0;
	// START READING SEGMENTS & SLICES
    for (unsigned int j=0; j<GS_Files.size(); j++) {

    	xercesc_3_1::outputSegment o;
    	o.segnum = (segmentNumbers.at(j));
    	VectorType diffAng ;

    	int count = GS_Files[j];

    	o.measureCount = 0;
    	o.LR = 0;
    	o.WT = 0;
    	o.filteredWT = 0;
    	o.fwhmWT = 0;
    	o.LRCalc = 0;
    	o.mPWA = 0;
    	o.WAPerc = 0;
    	o.LA = 0;
    	o.pcLR = 0;
    	o.pcWT = 0;
    	o.pcWAPerc = 0;

    	if (count==0) {
    		ccout << endl;
//    		ccout << "SKIPPING SEGMENT "<< (j+1) << endl;
    		ccout << "SKIPPING SEGMENT "<< segmentNumbers.at(j) << endl;
    	}
    	else {
    		ccout << endl << "SEGMENT "<< segmentNumbers.at(j) << ", expecting "<< GS_Files[j] << " slices" << endl;
    		// take only 20th to 80th percentile of slices - to avoid the bifurcation areas


//    		int beginCount, endCount;

// DEBUG FOR TESTING ONLY

    		std::stringstream f_outdir, f_outdirB, f_outdirPC;
    		f_outdirB << f_base.str() <<  "/SlicesB/";
    		f_outdir << f_base.str() << "/Slices/";
    		f_outdirPC << f_base.str() << "/SlicesPC/";
    		try {
    			// Seems like boost doesn't do nested directory. bloody stupid.

    			if ( !boost::filesystem::exists( p2base) ) ccout << "Oops---base_dir not created!" << endl;

    			boost::filesystem::path p (f_outdir.str());
    			boost::filesystem::create_directory(p);
    			f_outdir << "Segment_" << segmentNumbers.at(j) << "/";
    			boost::filesystem::path p2 (f_outdir.str());
    			boost::filesystem::create_directory(p2);
    			if ( !boost::filesystem::exists( p2) ) ccout << "Oops---dir not created!" << endl;

    			boost::filesystem::path pB (f_outdirB.str());
    			boost::filesystem::create_directory(pB);
    			f_outdirB << "Segment_" << segmentNumbers.at(j) << "/";
    			boost::filesystem::path p2B (f_outdirB.str());
    			boost::filesystem::create_directory(p2B);

    			boost::filesystem::path pPC (f_outdirPC.str());
    			boost::filesystem::create_directory(pPC);
    			f_outdirPC << "Segment_" << segmentNumbers.at(j) << "/";
    			boost::filesystem::path p2PC (f_outdirPC.str());
    			boost::filesystem::create_directory(p2PC);

    			if ( !boost::filesystem::exists( p2PC) ) ccout << "Oops---dir not created!" << endl;
    		}
    		catch (boost::filesystem::filesystem_error &e)
    		  {
    		    cerr << "Create_Directory failed: " << e.what() << endl;
    		  }

    		float area = 0.0;

    		// now, iterate over all the points identified for this segment

    		pair<multimap<int, VectorType>::const_iterator, multimap<int, VectorType>::const_iterator> it = centroidVect.equal_range(segmentNumbers.at(j));
    		int sliceidx = 1;
    		if (centroidVect.count(segmentNumbers.at(j))==0) {
    			ccout << "DEBUG" << segmentNumbers.at(j) << " has no centroids" << endl;
    		} else {
// count should be equal to centroidVect.count(j+1)
    			ccout << "count: "<< count << ", centroidVect.count(..): "<< centroidVect.count(segmentNumbers.at(j)) << endl;
    			multimap<int, VectorType>::const_iterator it2;

    			// EACH is a new slice within segment
    			for (it2=it.first; it2!=it.second; ++it2) {
    				// note that the actual slice number is lost. iterator returns valid sequential slices

    				if (sliceidx==1) {
    					diffAng = it2->second;
    				} else if (sliceidx==GS_Files[j]) {
    					double azim = sqrt ( pow((it2->second[0] - diffAng[0])*spacing[0],2) + pow((it2->second[1] - diffAng[1])*spacing[1],2) ); // sqrt(x²+y²)
    					double theta = atan2(abs(it2->second[2] - diffAng[2])*spacing[2], azim);
    					if (abs(theta) < M_PI/8) // almost horizontal
    						;
    					else if (abs(theta) > M_PI/8 && (abs(theta) < 3*M_PI/8))
    						;
    					else  // almost vertical!
    						;
    					avgAngle += abs(theta);
    					ccout << "Segment "<< segmentNumbers.at(j) << " has angle: "<< abs(theta) << "\n";
    				}
    				ccout << "in multimap iterator "<< it2->first << ", slice " << sliceidx  << "\n";
    				sliceidx++;

    				std::stringstream f_outfile;

    				f_outfile << (segmentNumbers.at(j)) << "/Slice_" << (segmentNumbers.at(j)) << "_" << sliceidx << ".dcm";
    				string f_out, f_out_bin, f_out_pc;
    				f_out_bin= f_base.str() + "/SlicesB/Segment_" + f_outfile.str();
    				f_out= f_base.str() + "/Slices/Segment_" + f_outfile.str();
    				f_out_pc= f_base.str() + "/SlicesPC/Segment_" + f_outfile.str();

    				// open output files
    				std::stringstream file_1D, file_1Dp;
    				if (hasEnding(argv[3], "all.csv")) {
    					file_1D << f_base.str() << "/Profiles/Profile_ALL_" << (segmentNumbers.at(j)) << "_" << sliceidx << ".csv";
    					file_1Dp << f_base.str() << "/Profiles/Profile_mm_ALL_" << (segmentNumbers.at(j)) << "_" << sliceidx << ".csv";
    				} else {
    					file_1D << f_base.str() << "/Profiles/Profile_" << (segmentNumbers.at(j)) << "_" << sliceidx << ".csv";
    					file_1Dp << f_base.str() << "/Profiles/Profile_mm_ALL_" << (segmentNumbers.at(j)) << "_" << sliceidx << ".csv";
    				}
    				string aux2 = file_1D.str();
    				string aux2p = file_1Dp.str();

    				ofstream fout2 (aux2.c_str());
    				ofstream fout2p (aux2p.c_str());
    				// it2->first // refers to segment#
    				// refers to coordinate
    				Center = it2->second;
    				Direction = normVect1.find(segmentNumbers.at(j))->second;

    				Up[0]= 0;
    				Up[1]= 1;
    				Up[2]= 0;

    				typedef itk::BSplineInterpolateImageFunction<ImageType, double >  InterpolatorType;

    				typedef InterpolatorType::PointType    PointType;
    				typedef InterpolatorType::ContinuousIndexType ContinuousIndexType;
    				PointType point1, point2, point3;
    				PointType dir1, dir2, dir3;
    				InterpolatorType::ContinuousIndexType index1;
    				index1[0] = Center[0];
    				index1[1] = Center[1];
    				index1[2] = Center[2];

    				Direction[0] = Direction[0] * spacing[0];
    				Direction[1] = Direction[1] * spacing[1];
    				Direction[2] = Direction[2] * spacing[2];
    				DirectionBin[0] = Direction[0] * spacing2[0];
    				DirectionBin[1] = Direction[1] * spacing2[1];
    				DirectionBin[2] = Direction[2] * spacing2[2];
    				DirectionPC[0] = Direction[0] * spacing3[0];
    				DirectionPC[1] = Direction[1] * spacing3[1];
    				DirectionPC[2] = Direction[2] * spacing3[2];

    				input->TransformContinuousIndexToPhysicalPoint(index1, point1);
    				inputBin->TransformContinuousIndexToPhysicalPoint(index1, point2);
    				inputPC->TransformContinuousIndexToPhysicalPoint(index1, point3);

//    				input->TransformContinuousIndexToPhysicalPoint(Direction, dir1);
//    				inputBin->TransformContinuousIndexToPhysicalPoint(Direction, dir2);
//    				inputPC->TransformContinuousIndexToPhysicalPoint(Direction, dir3);
//
//    				ccout << "[CHECK - should be equal] Comparing Direction with dir1 "<< Direction << " vs "<< dir1 << "\n";
//    				ccout << "[CHECK - should be equal] Comparing Direction with dir1 "<< DirectionBin << " vs "<< dir2 << "\n";

    				Center[0]= point1[0];
    				Center[1]= point1[1];
    				Center[2]= point1[2];
    				CenterBin[0] = point2[0];
    				CenterBin[1] = point2[1];
    				CenterBin[2] = point2[2];
    				CenterPC[0] = point3[0];
    				CenterPC[1] = point3[1];
    				CenterPC[2] = point3[2];

    				if (Center[0]==0 && Center[1]==0 && Center[2]==0) {
    					ccout << "Skipping segment "<< (segmentNumbers.at(j)) << ", slice " << sliceidx << " cos Center is ALL ZERO "<< endl;
    					continue;
    				}
    				if (Direction[0]==0 && Direction[1]==0 && Direction[2]==0) {
    					ccout << "Skipping segment "<< (segmentNumbers.at(j)) << ", slice "<< sliceidx++ << " cos Direction is ALL ZERO "<< endl;
    					continue;
    				}
    				// for further processing..
    				ccout << (segmentNumbers.at(j)) << "," << sliceidx << "," << Center << "," << Direction ;

    				AirwaySectionerBin->SetPlane( CenterBin, DirectionBin, Up, 0);
    				AirwaySectioner->SetPlane( Center, Direction, Up, 1);
    				AirwaySectionerPC->SetPlane( CenterPC, DirectionPC, Up, 1);

    				Radius = 25 / spacing[0];
    				AirwaySectioner->SetRadiusToExtract(Radius);
    				AirwaySectioner->Update();
    				AirwaySectionerBin->SetRadiusToExtract(Radius);
    				AirwaySectionerBin->Update();
    				AirwaySectionerPC->SetRadiusToExtract(Radius);
    				AirwaySectionerPC->Update();

    				const ImageType * inputImageGS = AirwaySectioner->GetOutput();
    				const ImageType * inputImageBin = AirwaySectionerBin->GetOutput();

    				const ImageType::SpacingType & spacing = inputImageGS->GetSpacing();
//    				const ImageType::PointType & origin  = inputImageGS->GetOrigin();
    				ImageType::SizeType size = inputImageGS->GetLargestPossibleRegion().GetSize();
//    				const ImageType::PointType & origin2  = inputImageBin->GetOrigin();
    				ImageType::SizeType size2 = inputImageBin->GetLargestPossibleRegion().GetSize();
    				const ImageType::SpacingType & spacing2 = inputImageBin->GetSpacing();
    				ccout << "AirwaySectioner output GS Spacing: "<< spacing[0] << "," << spacing[1] << "," << spacing[2] << endl;
    				ccout << "AirwaySectioner output Bin Spacing: " << spacing2[0] << "," << spacing2[1] << "," << spacing2[2]  << endl;
    				ccout << "AirwaySectioner output PC Spacing: " << spacing3[0] << "," << spacing3[1] << "," << spacing3[2]  << endl;

    				// TO MEASURE AREA
//    				float curArea = 0.0;
//    				double perimeter = 0.0;
//    					curArea = AirwaySectionerBin->GetArea();

/*
    				// TO MEASURE PERIMETER -- V2 check for perimeter. if not 10 or 20 discard
    				ConverterType::Pointer converter = ConverterType::New();
    				converter->SetInput( inputImageBin );
    				converter->SetInputForegroundValue( 1 );
    				converter->Update();
    				LabelMapType::Pointer labelMap = converter->GetOutput();

    				for( unsigned int label=1; label<=labelMap->GetNumberOfLabelObjects(); label++ )
    				{
    					const LabelObjectType * labelObject = labelMap->GetLabelObject( label );
    					//fout2 << "Area (using labelmap): " << labelObject->GetPhysicalSize() << endl;
    					//fout2 << "Perimeter (Equiv, using labelmap): " << labelObject->GetEquivalentPerimeter() << std::endl;
    					//  				     //fout2 << "Roundness: " << labelObject->GetRoundness() << endl;
    					//  				     roundness = labelObject->GetRoundness();
    					perimeter = labelObject->GetEquivalentPerimeter();
    				}*/

    				// Write output - COMMENTED
    				WriterType_3D::Pointer writer2 = WriterType_3D::New();
    				writer2->SetFileName( f_out );
    				writer2->SetInput( AirwaySectioner->GetOutput() );
    				writer2->Update();
    				//        		ccout << "GS: Saved file... "<< f_out << endl;
    				WriterType_3D::Pointer writer2Bin = WriterType_3D::New();
    				writer2Bin->SetFileName( f_out_bin );
    				writer2Bin->SetInput( AirwaySectionerBin->GetOutput() );
    				writer2Bin->Update();

    				WriterType_3D::Pointer writer2PC = WriterType_3D::New();
    				writer2PC->SetFileName( f_out_pc );
    				writer2PC->SetInput( AirwaySectionerPC->GetOutput() );
    				writer2PC->Update();

    				typedef itk::Image< PixelType, 2 > IType;
    				typedef itk::ImageFileReader< IType > ReaderType;
    				ReaderType::Pointer reader = ReaderType::New();
    				reader->SetFileName( f_out_bin );
    				reader->Update();
//    				std::cout << "output size: " << reader->GetOutput()->GetRequestedRegion().GetSize() << "\n";

    				// stupid way of filling hole
    				typedef itk::BinaryFillholeImageFilter< IType > I2LType;
    				I2LType::Pointer reconstruction = I2LType::New();
    				reconstruction->SetInput( reader->GetOutput() );
    				reconstruction->SetFullyConnected( 1 );
    				reconstruction->SetForegroundValue( 1 );
    				typedef itk::ImageFileWriter< IType > WriterType;
    				WriterType::Pointer writer = WriterType::New();
    				writer->SetInput( reconstruction->GetOutput() );
    				writer->SetFileName( f_out_bin );
    				writer->Update();

    				//        		ccout << "Bin: Saved file... "<< f_out_bin << endl;
    				long centerX=0.0, centerY=0.0;

    				int inPixelCount=0;
    				ImageType::IndexType idx;
    				ImageType::ConstPointer inputImage;

    				// find center of lumen - based on segmented image
    				try
    				{
    					inputImage = AirwaySectionerBin->GetOutput();
    				}
    				catch ( itk::ExceptionObject &err)
    				{
    					std::cout << "ExceptionObject caught !" << std::endl;
    					std::cout << err << std::endl;
    					return -1;
    				}
    				ConstIteratorType inputIt(  inputImage,  inputImage->GetRequestedRegion() );
    				PixelType value;
    				long minX=0, maxX=0, minY=0, maxY=0;
    				for ( inputIt.GoToBegin(); ! inputIt.IsAtEnd(); ++inputIt)
    				{
    					value = inputIt.Get();
    					if (value > 0) {
    						idx = inputIt.GetIndex();
    						centerX += idx[0];
    						centerY += idx[1];
    						if (inPixelCount==0) {
    							minX = idx[0];
    							minY = idx[1];
    						}
    						if (minX>idx[0]) minX = idx[0];
    						if (minY>idx[1]) minY = idx[1];
    						if (maxX<idx[0]) maxX = idx[0];
    						if (maxY<idx[1]) maxY = idx[1];
    						//            		    	ccout << "iterating, getting value: "<< value << " for " << idx[0] << ", " << idx[1] << endl;
    						inPixelCount++;
    					}
    				}
    				// end of find-center

    				double localRadius = 0.0;
    				// fixme localRadius should be calc as 20mm
    				// 26/05 - BAD IDEA - 2 walls could be detected
//    				localRadius = ceil(20 / spacing[0]);
    				localRadius = 16;

    				if (inPixelCount>0) {
    					// get mean of these points - VERIFY!
    					centerX = centerX/inPixelCount;
    					centerY = centerY/inPixelCount;

//    					localRadius = 16;

    					ccout << "Center of airway "<< (segmentNumbers.at(j)) <<" slice "<< sliceidx << " is " << centerX << ", " << centerY << endl;
    					// rotate, with this center
    					typedef itk::ImageFileWriter< ImageType >  WriterType;

    					ReaderType::Pointer reader_R = ReaderType::New();
    					WriterType::Pointer writer_R = WriterType::New();

//    					double numOfTakes = 128;
    					const double rotDeg = 360/num_of_rays;
    					const double degreesToRadians = vcl_atan(1.0) / 45.0;

    					double thickness = 0.0, totalEdgeR = 0.0, totalEdgeL = 0.0;
    					// for PC
    					double pc_thickness = 0.0, pc_radius = 0.0;
    					double validRay = 0.0;

    					vector<rayprofile> rayProfile;
    					vector<vector <PixelType> > OneDimProfiles;
    					vector<PixelType> OneDimProfile, OneDimProfileB, OneDimProfilePC;
    					// to store points identified by FWHM
    					extPoints.clear();
    					extPointsBin.clear();

//    					typedef itk::LinearInterpolateImageFunction<ImageType, double >  InterpolatorType;
    					InterpolatorType::Pointer interpolator = InterpolatorType::New();
    					InterpolatorType::Pointer interpolatorPC = InterpolatorType::New();
    					typedef itk::NearestNeighborInterpolateImageFunction<ImageType, double >  NNInterpolatorType;
    					NNInterpolatorType::Pointer interpolatorNN = NNInterpolatorType::New();

						// use minmax to compute
						MinCalcType::Pointer minCalc2 = MinCalcType::New();
						minCalc2->SetImage(inputImageGS);
						minCalc2->Compute();
						globalMin = minCalc2->GetMinimum();
						globalMax = minCalc2->GetMaximum();

    					PixelType defaultPixelValue = 100;
    					for (unsigned int take=0; take<num_of_rays; take++) {

    							double angleInDegrees = rotDeg * take * 1.0;
    							double angle = angleInDegrees * degreesToRadians;
    							double maxPeakIndex=0,  minLeftIdx=0, minRightIdx=0;

    							PixelType maxPeakVal=globalMin, minRightVal=globalMax, minLeftVal=globalMax;

    							// 04/01/2011
    							/**
    							 * Now we don't want to rotate, so get a vector of positions
    							 */
    							OneDimProfile.clear();
    							OneDimProfileB.clear();
    							OneDimProfilePC.clear();
    							ImageType::Pointer  outImage = ImageType::New();
    							ImageType::Pointer  outImageBin = ImageType::New();
    							ImageType::Pointer  outImagePC = ImageType::New();
    							start[0]=centerX;
    							start[1]=centerY;
    							size[0]=localRadius;
    							size[1]=1;

    							if (centerX + localRadius > size2[0]) {
    								ccout << "CENTER not detected well. DISCARDING.";
    								continue;
    							}

    							ImageType::RegionType region;
    							region.SetSize(size);
    							region.SetIndex(start);
    							outImage->SetRegions(region);
    							outImageBin->SetRegions(region);
    							outImagePC->SetRegions(region);
    							double spacing_final[ ImageType::ImageDimension ];
    							spacing_final[0]=1.0;
    							spacing_final[1]=1.0;
    							spacing_final[2]=1.0;
    							outImage->SetSpacing(spacing_final);
    							outImage->Allocate();
    							outImageBin->SetSpacing(spacing_final);
    							outImageBin->Allocate();
    							outImagePC->SetSpacing(spacing_final);
    							outImagePC->Allocate();

    							IteratorType it( outImage, outImage->GetRequestedRegion() );
    							IteratorType itBin( outImageBin, outImageBin->GetRequestedRegion() );
    							IteratorType itPC( outImagePC, outImagePC->GetRequestedRegion() );
    							it.GoToBegin();
    							itBin.GoToBegin();
    							itPC.GoToBegin();
    							ImageType::IndexType pixelIndex;
    							// use this as beginning
    							pixelIndex = it.GetIndex();

    							interpolator->SetInputImage(AirwaySectioner->GetOutput());
    							interpolatorPC->SetInputImage(AirwaySectionerPC->GetOutput());
    							interpolatorNN->SetInputImage(AirwaySectionerBin->GetOutput());

//    							typename InterpolatorType::ContinuousIndexType inputIndex;
    							typedef InterpolatorType::ContinuousIndexType ContinuousIndexType;
    							InterpolatorType::ContinuousIndexType continuousIndex, continuousIndex2;
    							vector<ContinuousIndexType> path;
    							double x_factor = cos(angle);
    							double y_factor = sin(angle);

    							// fixme localRadius contains the number of pixels, but we want the number of points
    							// between them.
    							// fixme to change distance betw points to spacing[0]
    							for (int i=0; i<localRadius; i++){
    								continuousIndex[0] = (double) pixelIndex[0] + i*x_factor;
    								continuousIndex[1] = (double) pixelIndex[1] + i*y_factor;
    								continuousIndex[2] = 0;
    								path.push_back(continuousIndex);
    							}
    							ccout << endl;
    							int pathindex = 0;
    							double p_max = globalMin, p_min = globalMax;
    							bool toDiscard = false;
    							while( !it.IsAtEnd() )
    							{
    								continuousIndex = path.at(pathindex);
    								PixelType value = interpolator->EvaluateAtContinuousIndex(continuousIndex);
    								PixelType valuePC = interpolatorPC->EvaluateAtContinuousIndex(continuousIndex);
    								if (value==-2000) {
    									ccout << "Found -2000. Discarding this ray!\n";
    									toDiscard = true;
    									break;
    								}
    								PixelType binvalue = interpolatorNN->EvaluateAtContinuousIndex(continuousIndex);
//    								ccout << "writing index> "<< pathindex << "(" << continuousIndex << "): "<< value << "[bin> " << binvalue << "]" << endl;
    								if (interpolator->IsInsideBuffer(continuousIndex)) {
//    									ccout << value << ",";
    									it.Set(value);
    									OneDimProfile.push_back(value);
    									OneDimProfilePC.push_back(valuePC);
    									if (maxPeakVal < value) { // iterLength condition added so that locations left of minLeftIdx are ignored
    										maxPeakVal = value;
    										maxPeakIndex = pathindex; // only in x-dir
    									}
    									if (value < p_min) p_min = value;
    									if (value > p_max) p_max = value;
    								} else {
    									it.Set(defaultPixelValue);
    								}
    								if (interpolatorNN->IsInsideBuffer(continuousIndex)) {
    									itBin.Set(binvalue);
    									OneDimProfileB.push_back(binvalue);
    								} else {
    									itBin.Set(0);
    								}
    								++it;
    								++itBin;
    								++pathindex;
    							}
    							if (toDiscard) continue;
    							ccout << "Angle: "<< angleInDegrees << endl;
//    							ccout << "B60f ";
//    							for (unsigned int i=0; i<OneDimProfile.size(); i++) {
//    								ccout << ", " << OneDimProfile.at(i);
//    							}
//    							ccout << "\nB40f ";
//    							for (unsigned int i=0; i<OneDimProfilePC.size(); i++) {
//    								ccout << ", " << OneDimProfilePC.at(i);
//    							}
//    							ccout << "\nInterp";

    							vector<double> derivatives_1, derivatives_2;
    							vector<float> intersection;
    							// find intersection between this pos and the next
    							// FIXME need to do spline interpolation - instead of comparing OneDimProfile & OneDimProfilePC,
    							// need to do work from interpolator!
    							// 1st step: LINEAR INTERPOLATION!
    							for (unsigned int i=1; i<OneDimProfilePC.size(); i++) {
    								float xint = DoLineSegmentIntersection(LineVector(i-1, OneDimProfile.at(i-1)), LineVector(i, OneDimProfile.at(i)), LineVector(i-1, OneDimProfilePC.at(i-1)), LineVector(i, OneDimProfilePC.at(i)));
    								intersection.push_back(xint);
//    								ccout << ", " << xint ;
    							}
    							vector<float> phaseCong = calculatePC_WallThickness(getDerivative(OneDimProfile), getDerivative(OneDimProfilePC), intersection, false);
    							ccout << "First estimate (PC) left: "<< phaseCong.at(0) <<", right: "<< phaseCong.at(0) + phaseCong.at(1) <<" \n";
    							double l1 = floor(phaseCong.at(0)) - 1.5;
    							double r1 = ceil(phaseCong.at(0) + phaseCong.at(1)) + 1.0;
    							// 2nd step: when range is identified, do a smoother bspline interpolation to determine intersection

    							intersection.clear();
    							double tstep = 0.05;
    							stringstream b60, b40, d1, d2;
    							for (double cidx=(l1+tstep); cidx<r1; cidx+= tstep) {
    								continuousIndex[0] = (double) pixelIndex[0] + (cidx-tstep)*x_factor;
    								continuousIndex[1] = (double) pixelIndex[1] + (cidx-tstep)*y_factor;
    								continuousIndex[2] = 0;
    								continuousIndex2[0] = (double) pixelIndex[0] + cidx*x_factor;
    								continuousIndex2[1] = (double) pixelIndex[1] + cidx*y_factor;
    								continuousIndex2[2] = 0;
    								PixelType value = interpolator->EvaluateAtContinuousIndex(continuousIndex);
    								PixelType valuePC = interpolatorPC->EvaluateAtContinuousIndex(continuousIndex);
    								PixelType value2 = interpolator->EvaluateAtContinuousIndex(continuousIndex2);
    								PixelType valuePC2 = interpolatorPC->EvaluateAtContinuousIndex(continuousIndex2);
    								b60 << ", "<< value;
    								b40 << ", "<< valuePC;
    								// DEBUG
    								derivatives_1.push_back((value2 - value));
    								derivatives_2.push_back((valuePC2 - valuePC));
    								d1 << ", " << (value2 - value);
    								d2 << ", " << (valuePC2 - valuePC);
    								float xint = DoLineSegmentIntersection(LineVector((cidx-tstep), value), LineVector(cidx, value2), LineVector((cidx-tstep), valuePC), LineVector(cidx, valuePC2));
//    								ccout << ", " << xint;
    								intersection.push_back(xint);
    							}
    							ccout << "B60f: " << b60.str() << "\n";
    							ccout << "B40f: " << b40.str() << "\n";
    							ccout << "deriv1: " << d1.str() << "\n";
    							ccout << "deriv2: " << d2.str() << "\n";
    							phaseCong = calculatePC_WallThickness(derivatives_1, derivatives_2, intersection, true);

    							float pc_LR = phaseCong.at(0);
    							float pc_WT = phaseCong.at(1);

    							ccout << "\nPC left: "<< pc_LR <<", right: "<< (pc_LR + pc_WT) <<" \n";
    							  /**
    							   * end of path definitions
    							   */
    							std::stringstream f_outfileR;

    							if (hasEnding(argv[3], "all.csv")) {
    								f_outfileR << (segmentNumbers.at(j)) << "/Slice_ALL_" << (segmentNumbers.at(j)) << "_" << sliceidx << "."<< take <<".dcm";
    							} else
    								f_outfileR << (segmentNumbers.at(j)) << "/Slice_" << (segmentNumbers.at(j)) << "_" << sliceidx << "."<< take <<".dcm";
    							string f_outR = f_base.str() + "/Slices/Segment_" + f_outfileR.str();
    							// for debug only... (only GS, not binary slices)
    							writer_R->SetFileName( f_outR );
    							writer_R->SetInput( outImage );
    							writer_R->Update();

    							PixelType value = 0;
    							int found = 0;
    							double idx = -1;

    							// idx is location of start of wall (from segmentation)
    							OneDimProfileB.begin();
    							for (int i=0; i<OneDimProfileB.size(); i++) {
    								value = OneDimProfileB.at(i);
    								ccout << value << ", ";
    								if (value < 1 && found==0) {
										found = 1;
										idx = (i-1);
    								}
    								if (found && value > 0) {
    									ccout << "Segmentation changes value... from 1 to 0 to 1... TO CHECK!" << endl;
    								}
    							}
    							ccout << endl;

    							value = 0;


    							ccout << "maxPeakVal: "<< maxPeakVal << ", ";
    							ccout << "maxPeakIndex: (aft offset) "<< maxPeakIndex << ", ";
    							double exp_range = p_max - p_min;
    							ccout << "expected range: "<< exp_range << endl;

    							// use binary-determined value for minLeftVal
    							if (idx==0) {
    								ccout << "there may be SOMETHING SERIOUS... idx==0." << endl;
    							}

    							// search for minLeftVal
    							for (int v1D=maxPeakIndex; v1D>=0; v1D--) {
    								value = OneDimProfile.at(v1D);
    								if (value < minLeftVal) { // less than: to get the 1st minima
    									minLeftVal = value;
    									minLeftIdx = v1D;
    								}
    								if (value > minLeftVal && v1D<minLeftIdx && (maxPeakVal - value > 0.8*exp_range) ) // to detect if 1st minima has been passed
    									break;
    							}

    							if (minLeftIdx > idx) {
    								ccout << "minLeftIdx > idx, to check manually!" << endl;
    							}

    							// TODO search for better maxPeakVal, and better minLeftIdx & minRightIdx (often there are >1 match)

								// search for minRightVal
    							minRightVal = maxPeakVal;
    							for (int v1D=maxPeakIndex; v1D<localRadius; v1D++) {
    								value = OneDimProfile.at(v1D);
    								if (value < minRightVal) { // less than: to get the 1st minima
    									minRightVal = value;
    									minRightIdx = v1D;
    								}
    								if (value > minRightVal && v1D>minRightIdx && (maxPeakVal - value > 0.8*exp_range) ) // to detect if 1st minima has been passed
    									break;
    							}
    							ccout << "idx: "<< idx << ", ";
    							if (idx==-1) {
    								idx = minLeftIdx;
    								ccout << " (but reset to "<< idx << ") ,";
    							}
    							ccout << "minLeftVal: "<< minLeftVal << ", ";
    							ccout << "minLeftIdx: "<< minLeftIdx << ", ";
    							ccout << "minRightVal: "<< minRightVal << ", ";
    							ccout << "minRightIdx: "<< minRightIdx << endl;

    							double diff1 = abs(maxPeakVal - minLeftVal) ;
    							double diff2 = abs(maxPeakVal - minRightVal);

    							// use alpha & beta to calculate
//    							double beta = diff1;
//    							double alpha = abs(minRightVal - minLeftVal);
    							// since we use a different offset value to shift the curve, beta & alpha must change
    							double beta = maxPeakVal;
    							double alpha = minRightVal;

    							// QC #1: diff must be sufficient
    							if (diff1 < 100) {
    								//fout2 << "diff1 too small (to discard) "<< endl;
    								//fout2p << "diff1 too small (to discard) "<< endl;
    								continue;
    							}
    							if (diff2 < 20) {
    								//fout2 << "diff2 too small (to discard) " << endl;
    								//fout2p << "diff2 too small (to discard) "<< endl;
    								continue;
    							}

    							// calc 10% of max values
    							double edge10L =  minLeftVal + 0.1*(maxPeakVal - minLeftVal);
    							bool altEdgeL = false ;
    							double altLeftVal = 0.0;
//    							if (idx>0) {
//    								altLeftVal = OneDimProfile.at(idx);
//    								edge10L = altLeftVal + 0.1*(maxPeakVal - altLeftVal);
//    								altEdgeL = true;
//    							}

    							double edge10R =  minRightVal + 0.1*(maxPeakVal - minRightVal);

    							double fwhm_edge10L = 0.5*(maxPeakVal + minLeftVal);
    							if (altEdgeL) {
    								fwhm_edge10L = 0.5*(maxPeakVal + altLeftVal);
    							}
    							double fwhm_edge10R = 0.5*(maxPeakVal + minRightVal);

    							// crude method: linear interp, x = xa + ( y - ya )*(xb - xa) / (yb - ya)
    							double edgeL = 1.0*minLeftIdx + (edge10L - minLeftVal)*((1.0*(maxPeakIndex - minLeftIdx))/(1.0*(maxPeakVal - minLeftVal)));
    							if (altEdgeL) {
    								edgeL = idx + (edge10L - altLeftVal)*((1.0*(maxPeakIndex - idx))/(1.0*(maxPeakVal - altLeftVal)));
    								ccout << "Using alternative edge calc (by idx): "<< endl;
    							}
    							double fwhm_edgeL = 1.0*minLeftIdx + (fwhm_edge10L - minLeftVal)*((1.0*(maxPeakIndex - minLeftIdx))/(1.0*(maxPeakVal - minLeftVal)));

    							if (altEdgeL) {
    								fwhm_edgeL = idx + (fwhm_edge10L - altLeftVal)*((1.0*(maxPeakIndex - minLeftIdx))/(1.0*(maxPeakVal - altLeftVal)));
    							}
    							double edgeR = 1.0*maxPeakIndex + (edge10R - maxPeakVal)*((1.0*(minRightIdx - maxPeakIndex))/(1.0*(minRightVal - maxPeakVal)));
    							double fwhm_edgeR = 1.0*maxPeakIndex + (fwhm_edge10R - maxPeakVal)*((1.0*(minRightIdx - maxPeakIndex))/(1.0*(minRightVal - maxPeakVal)));


//    							// FILTERING STEP - discard profiles whose edgeR is further than 2*minLeftIdx
//    							if (edgeR > 2*edgeL && idx>0 && minLeftIdx>0) {
//    								continue;
//    							}

    							ccout << "10%MaxLeft: "<<edge10L;
    							ccout << ", 10%MaxRight: "<<edge10R;
    							ccout << ", (linear interp) edgeL: "<< edgeL << ", edgeR: "<< edgeR ;
    							ccout << ", halfMaxLeft: "<<fwhm_edge10L;
    							ccout << ", halfMaxRight: "<<fwhm_edge10R;
    							ccout << ", (linear interp) edgeL: "<< fwhm_edgeL << ", edgeR: "<< fwhm_edgeR << endl;

    							if (!isnan(edgeR) && !isnan(edgeL) && (diff1 >= 100 && diff2 >= 20)){ // add the 2 contrast conditions for weinheimer impl
    								validRay++;
    								if (validRay < (num_of_rays/2) && (take==num_of_rays/2 - 1)) { // if halfway mark crossed but not enough validRays accumulated
    									//fout2 << " NOT ENOUGH VALID RAYS -- SLICE DISCARDED!";
    									//fout2p << " NOT ENOUGH VALID RAYS -- SLICE DISCARDED!";
    									ccout << " NOT ENOUGH VALID RAYS -- SLICE DISCARDED!\n";
    									continue;
    								}

    								// IBM calculation
    								double curthick = 1.0*(edgeR - edgeL);
    								rayprofile r ;
    								r.angle = angleInDegrees;
    								r.edge10L = edgeL;
    								r.edge10R = edgeR;
    								r.alpha = alpha;
    								r.beta = beta;
    								r.minLeftVal = minLeftVal;
    								r.edge10LVal = edge10L; // HU value at 10% left
    								r.edge10RVal = edge10R; // Hu value at 10% right
    								r.minLeftIdx = minLeftIdx;
    								r.maxPeakIdx = maxPeakIndex;
    								r.idx = idx;
//    								ccout << r.angle << ", ";
//    								ccout << r.edge10L << ", ";
//    								ccout << r.edge10R << ", ";
//    								ccout << r.alpha << ", ";
//    								ccout << r.beta << ", ";
//    								ccout << r.minLeftVal << ", ";
//    								ccout << r.edge10LVal << ", ";
//    								ccout << r.edge10RVal << ",";
//    								ccout << r.minLeftIdx << endl;

    								rayProfile.push_back(r);
    								OneDimProfiles.push_back(OneDimProfile);

    								// FWHM calculation
    								double curthick50 = fwhm_edgeR - fwhm_edgeL;
    								thickness += curthick50;
    								totalEdgeR += edgeR;
    								totalEdgeL += edgeL;

    								pc_thickness += pc_WT;
    								pc_radius += pc_LR;

    								ccout << "THICKNESS (EST 10%): "<< curthick << endl; // in pixels
    								ccout << "THICKNESS (FWHM): "<< curthick50 << endl; // in pixels
    								ccout << "THICKNESS (PhaseCong): "<< pc_WT  << endl; // in pixels

    								// save the points <angle, <L,R>>
    								extPoints.insert(pair<double, Coordinate>(angle,Coordinate(minLeftIdx, minLeftIdx+curthick50) ));
    							}

    							// END of ROTATE
    						} // for (int take=0; take<numOfTakes; take++)
    						ccout << "Profile written to: " << aux2 << endl;

    						if (validRay >= (num_of_rays/2)) { // use N/2 criteria -> if no of validRays is less than half, discard slice
    							// get up to 25th percentile, map is 'sorted' - make sure length of rayprofile == validRay

    							double wThick = 0.0, wlumenR = 0.0, wInnerR = 0.0, wExtR = 0.0, wPWA = 0.0;

    							int validrays2 = 0;
    							extPointsIBM.clear();
    							toSortIBM.clear();
    							for (int i=0; i<rayProfile.size(); i++) { // do this only for up to 25th percentile
    								// 20/12/2010: to shift profile up - so that integral is never negative

    								double offset = getMin(OneDimProfiles.at(i));

    								double integral = integralSumOffset(rayProfile.at(i), OneDimProfiles.at(i));
    								double alpha = rayProfile.at(i).alpha;
    								double beta = rayProfile.at(i).beta;

    								wPWA += beta;
    								alpha -= offset;
    								beta -= offset;
    								ccout << "alpha, beta, (with offset): "<< alpha << "," << beta << ", "<< offset << endl;

    								double curT = (lambda * integral - (0.5*alpha*abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R) )) / (beta - 0.5*alpha);

    								// filter to see if thickness on both sides are almost equal: (maxPeakIdx - lr) - (maxPeakIdx - (lr+curT))
    								double innerThickness, outerThickness;
    								// FIXME
    								innerThickness = abs(rayProfile.at(i).maxPeakIdx - (rayProfile.at(i).idx+1));
    								outerThickness = abs(rayProfile.at(i).maxPeakIdx - (rayProfile.at(i).idx+1+curT));
    								if (abs (outerThickness - innerThickness ) > 2) // more than 2pixels
    								{
    									ccout << "Going to discard this rayProfile, thickness inequal (LR, maxPeak, ER): "<< (rayProfile.at(i).idx+1) << ", " << rayProfile.at(i).maxPeakIdx << ", " << (rayProfile.at(i).idx+1+curT) <<" (inn vs out): "<< innerThickness << ", "<< outerThickness << "\n";
    									continue;
    								}
    								// To calculate lumen radius & external wall, a + b*beta + c*alpha is integral,
    								// a = c, a + b + c = edge1-edge2, a = ((edge1-edge2) - b) /2
    								// therefore,
    								if (curT < 0 || (abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R) - curT < 0)) {
    									ccout << "Going to discard this rayProfile, lambda: "<< lambda <<", integral: "<< integral <<" curT: "<< curT << ", 10%: "<< abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R) << endl;
    									continue;
    								}
    								else validrays2++;

    								wThick += curT;

    								// FIXME FIXME FIXME what should be the value of lumenR depends on the questions:
    								/*
    								 * 1. does minLeftIdx indicate lumen boundary better than edge10L + a?
    								 * 2. how is minLeftIdx calculated (rather, how is air.dcm generated)?
    								 */

//    								double lumenR = rayProfile.at(i).minLeftIdx; // lumen radius is what segmentation gives us
    								double lumenR = rayProfile.at(i).idx + 1; // lumen radius is what segmentation gives us
    								wlumenR += lumenR;

//    								double ir ;
    								wInnerR += lumenR; // inner radius is lumen radius
    								double er = lumenR + curT;
    								wExtR += er;

    								extPointsIBM.insert(pair<double, Coordinate> (rayProfile.at(i).angle, Coordinate(lumenR, er) ));
    								toSortIBM.insert(pair<double, Coordinate> (abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R), Coordinate(curT, lumenR)));
    								// IMPT!!!!!!!! Last run (December 3 '10)  uses the following line:
    								// ccout << "Angle in deg: " << rayProfile.at(i).angle <<", Beta: "<< beta << ", Alpha: "<< alpha << ", Integral Area: " << integral << ", thickness(pix): "<< curT << ", thickness(mm): "<< curT*spacing[0] << ", ("<< rayProfile.at(i).edge10L << ", "<< rayProfile.at(i).edge10R <<") edge2-edge1: "<< abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R) << ", lumen radius: "<< (rayProfile.at(i).edge10L + lumenR) << endl;
    								ccout << "Angle in deg: " << rayProfile.at(i).angle <<", Beta: "<< beta << ", Alpha: "<< alpha << ", Integral Area: " << integral << ", thickness(pix): "<< curT << ", thickness(mm): "<< curT*spacing[0] << ", ("<< rayProfile.at(i).edge10L << ", "<< rayProfile.at(i).edge10R <<") edge2-edge1: "<< abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R) << ", lumen radius: "<< lumenR << endl;
    								// pixel only (angle, WT, LR)
//    								fout2 << rayProfile.at(i).angle << ", "<< abs(rayProfile.at(i).edge10L - rayProfile.at(i).edge10R) << endl;
    								fout2 << rayProfile.at(i).angle << ": ";
    								for (int x=0; x<OneDimProfiles.at(i).size(); x++)
    									fout2 << OneDimProfiles.at(i).at(x) << ", ";
    								fout2 << endl;

    								fout2p << rayProfile.at(i).angle << ", "<< curT << ", "<< lumenR <<  endl;
    							}
                                // output sorted vector toSortIBM
                                ccout << "25th percentile - Sorted IBM vector (edge10L - edge10R)" << endl;

                                int limit_start = 0;
                                int limit_end = (int) ceil(toSortIBM.size() * 0.25);

                                if (useMedianRays) {
                                	limit_start = (int) ceil(toSortIBM.size() * 0.25);
                                	limit_end = (int) ceil(toSortIBM.size() * 0.75);
                                }
                                int filtCount = 0;
                                double filtThickness = 0.0, wInnerR_calc = 0.0;
                                for (multimap<double, Coordinate>::const_iterator it (toSortIBM.begin()), end(toSortIBM.end()); it != end; ++it) {
//                                                              tempscore += (*it).second;

                                	if (filtCount < limit_start) {
                                		filtCount++;
                                		continue;
                                	}
                                	Coordinate c = (*it).second;
                                	ccout << "  [" << (*it).first << ", " << ((*it).second).x << "]" << endl;
                                	filtThickness += c.x; // c.x = curT, c.y = lumenR

                                	double calcLR = c.y + ((*it).first - c.x) / 2; // it.first=(edge10L-edge10R)
                                	//        								cout << "adding: "<< c.y << "to" << ((*it).first - c.x) / 2 << "\n";
                                	wInnerR_calc += calcLR;
                                	filtCount++;

                                	if (filtCount==limit_end) break;
                                }
//                                ccout << "filtCount: " << filtCount << " - " << limit_start << ": ";
                                filtCount = filtCount - limit_start;
//                                ccout << filtCount;

//                                cout << "\nSegment " << (segmentNumbers.at(j)) << " Slice " << sliceidx << endl;
    							fout3 << "\nSegment " << (segmentNumbers.at(j)) << " Slice " << sliceidx << endl;
    							fout3 << "From "<< validRay<< " Valid Slices: ";
//    							fout3 << "Area (fr segm - mm2): " << curArea << ", Perimeter: " << perimeter ;
    							fout3 << ", (FWHM) Avg Thickness: "<< (thickness / validRay);
    							fout3 << ", (FWHM) Avg Ext Radius: "<< (totalEdgeR / validRay);
    							// validrays2 -- count of valid rays != rayProfile.size()
    							fout3 << ", (IBM) Avg LumenRadius: "<< (wInnerR / validrays2);
    							fout3 << ", (IBM) Avg Seg+LumenRadius: "<< (wInnerR_calc / filtCount);
    							fout3 << ", (IBM) Avg Thickness ("<< validrays2 << " valid rays): "<< (wThick / validrays2);
    							fout3 << ", (Filtered IBM) Avg Thickness: "<< (filtThickness / filtCount);
    							// right index is a measure of external radius
    							fout3 << ", (IBM) Avg Ext Radius: "<< (wExtR / validrays2) ;
    							fout3 << ", (IBM) Avg Peak Value: "<< (wPWA / validrays2) ;
    							fout3 << ", (PC) Avg Thickness: "<< (pc_thickness / validRay) ;
    							fout3 << ", (PC) Avg LumenRadius: "<< (pc_radius / validRay) << endl;

    							fout3p << "Segment " << (segmentNumbers.at(j)) << " Slice " << sliceidx << endl;
    							fout3p << "From "<< validRay<< " Valid Slices: ";
//    							fout3p << "Area (fr segm- mm2): " << curArea << ", Perimeter: " << perimeter*spacing[0] ;
    							fout3p << ", (FWHM) Avg Thickness: "<< (thickness*spacing[0] / validRay);
    							fout3p << ", (FWHM) Avg Ext Radius: "<< (totalEdgeR*spacing[0] / validRay) ;
    							fout3p << ", (IBM) Avg LumenRadius: "<< (wInnerR*spacing[0] / validrays2);
    							fout3p << ", (IBM) Avg Seg+LumenRadius: "<< (wInnerR_calc*spacing[0] / filtCount);
    							fout3p << ", (IBM) Avg Thickness: ("<< validrays2 << " valid rays): "<< (wThick*spacing[0] / validrays2);
    							fout3p << ", (Filtered IBM) Avg Thickness: "<< (filtThickness*spacing[0] / filtCount);
    							// right index is a measure of external radius
    							fout3p << ", (IBM) Avg Ext Radius: "<< (wExtR*spacing[0] / validrays2) ;
    							fout3p << ", (IBM) Avg Peak Value: "<< (wPWA / validrays2) ;
    							fout3p << ", (PC) Avg Thickness: "<< (pc_thickness*spacing[0] / validRay);
    							fout3p << ", (PC) Avg LumenRadius: "<< (pc_radius*spacing[0] / validRay) << endl;
//    							double calcThickness = lambda*integral(OneDimProfile, );

//    							validityScore.insert(pair<int, int>(j+1, validRay));

    							// IMPT REMINDER
    							// GLOBAL SCORE  is stord only in MM!!!!!!!!!!!
    							gWT.insert(pair<int, double>(segmentNumbers.at(j), thickness*spacing[0]/validRay));
    							gIR.insert(pair<int, double>(segmentNumbers.at(j), (totalEdgeL*spacing[0]/validRay)));
    							gER.insert(pair<int, double>(segmentNumbers.at(j), (totalEdgeR*spacing[0]/validRay)));

    							// insert this slice's IBM ray-averaged avg thickness
    							gWT_IBM.insert(pair<int, double>(segmentNumbers.at(j), (wThick*spacing[0]/validrays2)));
    							gIR_IBM.insert(pair<int, double>(segmentNumbers.at(j), (wInnerR*spacing[0]/validrays2)));
    							gER_IBM.insert(pair<int, double>(segmentNumbers.at(j), (wExtR*spacing[0]/validrays2)));
    							gWT_sortIBM.insert(pair<int, double>(segmentNumbers.at(j), (filtThickness*spacing[0]/filtCount)));
    							gIRcalc_IBM.insert(pair<int, double>(segmentNumbers.at(j), (wInnerR_calc*spacing[0]/filtCount)));
    							gPWA.insert(pair<int, double> (segmentNumbers.at(j), (wPWA/validrays2)));

    							// calculate Lumen Area, Wall Area, and WA percentage - based on circle assumption
    							double wallArea = M_PI * vnl_math_sqr((wInnerR_calc*spacing[0] / filtCount) + (filtThickness*spacing[0] / filtCount));
    							double lumenArea = M_PI * vnl_math_sqr(wInnerR_calc*spacing[0] / filtCount);
    							double wallAreaPercentage = 100 * (wallArea - lumenArea) / wallArea;

    							double pc_wa =  M_PI * vnl_math_sqr((pc_radius*spacing[0] / validRay) + (pc_thickness*spacing[0] / validRay));
    							double pc_la = M_PI * vnl_math_sqr(pc_radius*spacing[0] / validRay);
    							double pc_waPerc = 100 * (pc_wa - pc_la) / pc_wa;

    							gWA_IBM.insert(pair<int, double> (segmentNumbers.at(j), wallAreaPercentage));
    							gLA.insert(pair<int, double> (segmentNumbers.at(j), lumenArea));

/*// comment out points generating code
    							// here analyze the points saved
    							vector<Coordinate> extPointsVector = calcExtPoints(extPoints, false);
    							vector<Coordinate> intPointsVector = calcIntPoints(extPoints, false);
//    							vector<Coordinate> intPointsVectorBin = calcIntPoints(extPointsBin, false);

//    							vector<Coordinate> extPointsVectorIBM = calcExtPoints(extPointsIBM, true);
//    							vector<Coordinate> intPointsVectorIBM = calcIntPoints(extPointsIBM, true);

    							vector<Coordinate> extPointsVectorIBM = calcExtPoints(extPointsIBM, false);
    							vector<Coordinate> intPointsVectorIBM = calcIntPoints(extPointsIBM, false);


    							// do it only for 1 slice
    							std::stringstream points, points2;
    							points << f_base.str() << "/points_diagnostic_" << (segmentNumbers.at(j)) << "_" << sliceidx;
    							ofstream foutBin (points.str().c_str());

    							points << "_edgeR_FWHM";
    							ofstream foutEL (points.str().c_str());

    							points2 << f_base.str() << "/points_diagnostic_" << (segmentNumbers.at(j)) << "_" << sliceidx << "_edgeR_IBM";
    							ofstream foutER (points2.str().c_str());

    							ccout << extPointsVector.size() << "," << extPointsVectorIBM.size() << "," << intPointsVector.size() << endl;
    							for (int i=0; i<extPointsVector.size(); i++) {
    								foutEL << (extPointsVector.at(i)).x << ","<< (extPointsVector.at(i)).y << endl;
    							}
    							for (int i=0; i<extPointsVectorIBM.size(); i++) {
    								foutER << (extPointsVectorIBM.at(i)).x << ","<< (extPointsVectorIBM.at(i)).y << endl;
    							}
    							for (int i=0; i<intPointsVector.size(); i++) {
    								foutBin << (intPointsVector.at(i)).x << ","<< (intPointsVector.at(i)).y << endl;
    							}
    							foutER.close();
    							foutEL.close();
    							foutBin.close();
    							*/

    							/* persist point!
    							*/
    							if (individualDataCapture) {
    								int mid = odb::lookup("WT");
    								odb::persist(studyName, seriesName, mid, o.segnum, wThick*spacing[0]/validrays2, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("LR");
    								odb::persist(studyName, seriesName, mid, o.segnum,  wInnerR*spacing[0]/validrays2, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("FilteredWT");
    								odb::persist(studyName, seriesName, mid, o.segnum,  filtThickness*spacing[0]/filtCount, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("mPWA");
    								odb::persist(studyName, seriesName, mid, o.segnum,  wPWA/validrays2, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("CalcLR");
    								odb::persist(studyName, seriesName, mid, o.segnum,  wInnerR_calc*spacing[0]/filtCount, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("LA");
    								odb::persist(studyName, seriesName, mid, o.segnum,  lumenArea, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("WAPerc");
    								odb::persist(studyName, seriesName, mid, o.segnum,  wallAreaPercentage, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("FWHMWT");
    								odb::persist(studyName, seriesName, mid, o.segnum,  thickness*spacing[0]/validRay, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("PhaseCongLR");
    								odb::persist(studyName, seriesName, mid, o.segnum,  pc_radius*spacing[0]/validRay, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("PhaseCongWT");
    								odb::persist(studyName, seriesName, mid, o.segnum,  pc_thickness*spacing[0]/validRay, lambda, expID, -1, sliceidx);
    								mid = odb::lookup("PhaseCongWAPerc");
    								odb::persist(studyName, seriesName, mid, o.segnum,  pc_waPerc, lambda, expID, -1, sliceidx);
    							}

        						// write out this segment's valid results
        						o.LR += wInnerR*spacing[0]/validrays2;
        						o.LRCalc += wInnerR_calc*spacing[0]/filtCount;
        						o.fwhmWT += thickness*spacing[0]/validRay;
        						ccout << "Seg, slice: "<< o.segnum <<", "<< sliceidx <<"  WT: "<< wThick*spacing[0]/validrays2 << ", filteredWT: " << (filtThickness*spacing[0]/filtCount) << ", (PC) WT: "<< (pc_thickness*spacing[0]/validRay) <<", (PC) LR: "<< (pc_radius*spacing[0]/validRay) << ", CalcLR: "<< wInnerR_calc*spacing[0]/filtCount <<", LA: "<< lumenArea<<", WA%: "<< wallAreaPercentage <<"\n";
        						o.WT += wThick*spacing[0]/validrays2;// filtThickness*spacing[0]/filtCount
        						o.filteredWT += filtThickness*spacing[0]/filtCount;
        						o.mPWA += wPWA/validrays2;
//        						o.Peri += 0;
        						o.WAPerc += wallAreaPercentage;
        						o.LA += lumenArea;
        						o.pcWT += pc_thickness*spacing[0]/validRay ;
        						o.pcLR += pc_radius*spacing[0]/validRay;
        						o.pcWAPerc += pc_waPerc;
        						o.measureCount++;
/*

    							// all in pixels
    							// for FWHM
    							double totalArea = polygonArea(extPointsVector, 0);
    							double innerArea = polygonArea(intPointsVector, 1);
    							double wallArea = totalArea - innerArea;
    							double perimeter = polygonPerim(intPointsVector);
    							ccout << "FWHM Poly (TOTAL) area: " << totalArea;
    							ccout << ", (Reported BIN) Area - mm2: "<< curArea ;
    							ccout << ", Poly (Lumen) area: " << innerArea;
    							ccout << ", Poly (Lumen) Perimeter: " << perimeter;
    							ccout << ", Wall area: " << wallArea;
    							ccout << ", WA%: " << (wallArea/totalArea)*100;
    							fout3 << "FWHM Poly WA%: "<< (wallArea/totalArea)*100 << endl;
    							fout3p << "FWHM Poly WA%: "<< (wallArea/totalArea)*100 << endl;
    							ccout << endl;
    							// FWHM
    							gWA.insert(pair<int, double>(j+1, (wallArea/totalArea)*100));
    							gWAmm2.insert(pair<int, double>(j+1, wallArea*pow(spacing[0],2)));
    							gPerim.insert(pair<int, double>(j+1, perimeter*spacing[0]));

    							// to plot perimeter vs WA% (FWHM)
//    							coutSfwhm << perimeter*spacing[0] << ", "<< (wallArea/totalArea)*100 << endl;
    							// to plot perimeter vs WT
    							coutSfwhm << perimeter*spacing[0] << ", "<< (thickness*spacing[0] / validRay) << endl;

    							// for IBM -- using polygon
    							totalArea = polygonArea(extPointsVectorIBM, 0);
    							innerArea = polygonArea(intPointsVectorIBM, 1);
//    							innerAreaBin = polygonArea(intPointsVectorBinIBM, 2);
    							wallArea = totalArea - innerArea;
    							perimeter = polygonPerim(intPointsVectorIBM);
    							ccout << "IBM Poly (TOTAL) area: " << totalArea;
    							ccout << ", Poly (Lumen) area: " << innerArea;
//    							ccout << ", Poly (Lumen Bin w/o lsq) area: " << innerAreaBin;
    							ccout << ", Poly (Lumen) Perimeter: " << perimeter;
    							ccout << ", Wall area (pixel^2): " << wallArea;
    							ccout << ", WA%: " << (wallArea/totalArea)*100 ;
    							ccout << endl;

    							// to plot perimeter (mm) vs WA% (mm2)
//    							coutSibm << perimeter*spacing[0] << ", "<< (wallArea/totalArea)*100 << endl;
    							// to plot perimeter vs WT
    							coutSibm << perimeter*spacing[0] << ", "<< (wThick*spacing[0] / validrays2) << endl;

    							fout3 << "[WITH GEOS pixels]"<< endl;
    							fout3 << "IBM Poly (TOTAL) area: " << totalArea;
    							fout3 << ", (Reported BIN) Area - mm2: "<< curArea ;
    							fout3 << ", Poly (Lumen) area: " << innerArea;
    							fout3 << ", Poly (Lumen) Perimeter: " << perimeter;
//    							fout3 << ", Poly (Lumen Bin w/o lsq) area: " << innerAreaBin;
    							fout3 << ", Wall area (pixel): " << wallArea;
    							fout3 << ", WA%: " << (wallArea/totalArea)*100 ;
    							fout3 << endl;

    							fout3p << "[WITH GEOS mm2]" << endl;
    							fout3p << "IBM Poly (TOTAL) area: " << totalArea*pow(spacing[0], 2);
    							fout3p << ", (Reported BIN) Area- mm2: "<< curArea ;
    							fout3p << ", Poly (Lumen) area: " << innerArea*pow(spacing[0], 2);
    							fout3p << ", Poly (Lumen) Perimeter: " << perimeter*spacing[0];
//    							fout3p << ", Poly (Lumen Bin w/o lsq) area: " << innerAreaBin*pow(spacing[0], 2);
    							fout3p << ", WA%: " << (wallArea/totalArea)*100 ;
    							fout3p << ", Wall area (mm2): " << wallArea*pow(spacing[0],2);
    							fout3p << endl;
    							gWA_IBM.insert(pair<int, double>(j+1, (wallArea/totalArea)*100));
    							gWAmm2_IBM.insert(pair<int, double>(j+1, wallArea*pow(spacing[0],2)));

    							// Assuming circular shape!

    							totalArea = M_PI * pow((wExtR / validrays2), 2) ;
    							innerArea = M_PI * pow((wInnerR / validrays2), 2);
    							//    							innerAreaBin = polygonArea(intPointsVectorBinIBM, 2);
    							wallArea = totalArea - innerArea;
    							ccout << "[ASSUMES CIRCULAR SHAPE] "<<endl;
    							ccout << "IBM Poly (TOTAL) area: " << totalArea;
    							ccout << ", Poly (Lumen) area: " << innerArea;
    							//    							ccout << ", Poly (Lumen Bin w/o lsq) area: " << innerAreaBin;
    							ccout << ", Wall area: " << wallArea;
    							ccout << ", WA%: " << (wallArea/totalArea)*100 ;
    							ccout << endl;

    							fout3 << "[WITH CIRCLE pixels]"<< endl;
    							fout3 << "IBM Poly (TOTAL) area: " << totalArea;
    							fout3 << ", Poly (Lumen) area: " << innerArea;
//    							fout3 << ", Poly (Lumen Bin w/o lsq) area: " << innerAreaBin;
    							fout3 << ", Wall area: " << wallArea;
    							fout3 << ", WA%: " << (wallArea/totalArea)*100 ;
    							fout3 << endl;

    							fout3p << "[WITH CIRCLE mm2]" << endl;
    							fout3p << "IBM Poly (TOTAL) area: " << totalArea*pow(spacing[0], 2);
    							fout3p << ", Poly (Lumen) area: " << innerArea*pow(spacing[0], 2);
    							//    							fout3p << ", Poly (Lumen Bin w/o lsq) area: " << innerAreaBin*pow(spacing[0], 2);
    							fout3p << ", Wall area: " << wallArea*pow(spacing[0], 2);
    							fout3p << ", WA%: " << (wallArea/totalArea)*100 ;
    							fout3p << endl;

    							// REMEMBER -- global score is always in mm or mm2
    							gWA_IBM_circ.insert(pair<int, double>(j+1, (wallArea/totalArea)*100));
*/

    						} // if validrays<(1/2*numTakes)  else don't write
    						else {
    							ccout << "Validrays less than 1/2 of numTakes" << endl;
    							// empty points saved
    							extPoints.clear();
    						}

    						fout2.close();
    						fout2p.close();

    				} else { // no valid slice within segment detected
    					ccout << "CAN'T CALCULATE Center of segment bcos count is 0!"<< endl;
    					// SKIP this slice
    					continue;
    				}

    			}//end of slice iterators (per segment)
    			o.LR = o.LR / o.measureCount;
    			o.LRCalc = o.LRCalc / o.measureCount;
    			o.WT = o.WT / o.measureCount;
    			o.fwhmWT = o.fwhmWT / o.measureCount;
    			o.filteredWT = o.filteredWT / o.measureCount;
    			o.mPWA = o.mPWA / o.measureCount;
    			o.WAPerc = o.WAPerc / o.measureCount;
    			o.LA = o.LA / o.measureCount;
    			o.pcWT = o.pcWT / o.measureCount;
    			o.pcLR = o.pcLR / o.measureCount;
    			o.pcWAPerc = o.pcWAPerc / o.measureCount;
//    			cout << "pushing "<< o.segnum << ", LR: "<< o.LR << " with " << o.measureCount;
//    			cout << "pushing "<< o.segnum << ", WT: "<< o.WT << " with " << o.measureCount << endl;
    			xercesc_3_1::myoutput.allSegs.push_back(o);
    			// FIXME for the rest of the values when they're available

    		} // end of (if there is centroid detected)
    	}//end else slice count==0
    }//end-for GS_Slice

    cout << "Average angle: "<< avgAngle/GS_Files.size() << "\n";

//    map_type::left_map& view_type_L = parentChildTree.left;
//    map_type::right_map& view_type_R = parentChildTree.right;
//    typedef map_type::const_iterator const_iterator;
//
//    // test reading for parent
//    for (map_type::right_map::const_iterator it(view_type_R.begin()), end(view_type_R.end()); it != end; ++it)
//    {
//    	ccout << "Parent of #" << (*it).first << " is #" << (*it).second << endl;
//    }
//
//    for (map_type::left_map::const_iterator it(view_type_L.begin()), end(view_type_L.end()); it != end; ++it)
//    {
//    	ccout << "Child of #" << (*it).first << " is #" << (*it).second << endl;
//    }

      /*ccout << "Number of children of segment 1: " << parentChildTree.count(1) << endl;
      ccout << "CHILDREN" << endl;
      for (multimap<int, int>::const_iterator it (parentChildTree.begin()), end(parentChildTree.end());
    		  it != end;
    		  ++it)
      {
    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
*/

      for (multimap<double, Coordinate >::const_iterator it (extPoints.begin()), end(extPoints.end());
          		  it != end;
          		  ++it)
            {
          	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
            }

      fout3p << "=========GLOBAL Measures (in mm)=========" << endl;
//      fout3 << "QC Score (Valid Ray Count, Num of Valid Slices) per segment" << endl;

      // FWHM
      /*
       * # of valid Slices, global mean thickness, global mean inner radius, global WA%
       */
      fout3p << "[FWHM METHOD]";
      double tempscore = 0.0;

//      for (multimap<int, double>::const_iterator it (gPerim.begin()), end(gPerim.end()); it != end; ++it) {
//    	  tempscore += (*it).second;
//    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
//      }
//      fout3p << ", Perimeter: (from " << gPerim.size() << " takes) "<<  (tempscore/gPerim.size());
      fout3p << endl;
      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gWT.begin()), end(gWT.end()); it != end; ++it) {
    	  tempscore += (*it).second;
//    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << "WT: (from " << gWT.size() << " slices) " << (tempscore/gWT.size());
      /**
       * 	double WT;
	double WAPerc;
	double LR;
	int BC;
	double TL;
	double Peri;
	double LA;
	double NASCount;
       */

      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gIR.begin()), end(gIR.end()); it != end; ++it) {
    	  tempscore += (*it).second;
    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << ", Lumen Radius: (from " << gIR.size() << " takes) "<<  (tempscore/gIR.size());

//      tempscore = 0.0;
//      for (multimap<int, double>::const_iterator it (gWA.begin()), end(gWA.end()); it != end; ++it) {
//    	  tempscore += (*it).second;
//    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
//      }
//      fout3p << ", WA%: (from " << gWA.size() << " takes) "<<  (tempscore/gWA.size());
//
//      tempscore = 0.0;
//      for (multimap<int, double>::const_iterator it (gWAmm2.begin()), end(gWAmm2.end()); it != end; ++it) {
//    	  tempscore += (*it).second;
//    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
//      }
//      fout3p << ", WA: (from " << gWAmm2.size() << " takes) "<<  (tempscore/gWAmm2.size());
//
//      fout3p << endl;
      // IBM
      /*
       * # of valid slices, global mean thickness, global mean inner radius, global WA%
       */
      fout3p << "[IBM METHOD]"<< endl;
      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gWT_IBM.begin()), end(gWT_IBM.end()); it != end; ++it) {
    	  tempscore += (*it).second;
    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << "WT: (from " << gWT_IBM.size() << " slices) " << (tempscore/gWT_IBM.size());
      xercesc_3_1::myoutput.WT = tempscore/gWT_IBM.size();
      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gWT_sortIBM.begin()), end(gWT_sortIBM.end()); it != end; ++it) {
          tempscore += (*it).second;
          //              ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << ", Filtered WT: (from " << gWT_sortIBM.size() << " slices) " << (tempscore/gWT_sortIBM.size());
      xercesc_3_1::myoutput.filteredWT = tempscore/gWT_sortIBM.size();

      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gIR_IBM.begin()), end(gIR_IBM.end()); it != end; ++it) {
    	  tempscore += (*it).second;
    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << ", IR: (from " << gIR_IBM.size() << " takes) " << (tempscore/gIR_IBM.size());
      xercesc_3_1::myoutput.LR = tempscore/gIR_IBM.size();

//      tempscore = 0.0;
//      for (multimap<int, double>::const_iterator it (gWA_IBM.begin()), end(gWA_IBM.end()); it != end; ++it) {
//    	  tempscore += (*it).second;
//    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
//      }
//      fout3p << ", WA%: (from " << gWA_IBM.size() << " takes) " << (tempscore/gWA_IBM.size()) ;
      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gWA_IBM.begin()), end(gWA_IBM.end()); it != end; ++it) {
    	  tempscore += (*it).second;
    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << ", WA% (circ): (from " << gWA_IBM.size() << " takes) " << (tempscore/gWA_IBM.size()) ;
      xercesc_3_1::myoutput.WAPerc = tempscore/gWA_IBM.size();

      tempscore = 0.0;
      for (multimap<int, double>::const_iterator it (gLA.begin()), end(gLA.end()); it != end; ++it) {
    	  tempscore += (*it).second;
    	  //    	  ccout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
      }
      fout3p << ", LA: (from " << gLA.size() << " takes) "<<  (tempscore/gLA.size());
      xercesc_3_1::myoutput.LA = tempscore/gLA.size();

      fout3p << endl;

      if (xmlMode && suppressOutput==0) {
    	  xercesc_3_1::xmlwrite(xercesc_3_1::myoutput);
    	  cout << endl;
      }
      if (saveToPersistence) {
    	  odb::persistWholeOutput(xercesc_3_1::myoutput, lambda, expID);
      }
      if (suppressOutput) {
    	  cout << lambda << ", " << seriesName << ", " << calculateTotalError(xercesc_3_1::myoutput, truevalues, false) << ", "<< calculateTotalErrorPerc(xercesc_3_1::myoutput, truevalues) << endl;
      }
//      xercesc_3_1::xmlwrite();

//      ccout << "QC Score (Valid Ray Count, Num of Valid Slices) per segment" << endl;

//      ccout << "Leftmost Trajectory #1" << endl;
      /*
      // One trajectory, leftChild
      int rootNode = 1;

      int nextNode = rootNode;
      bool stop = false;

      multimap<int,int>::iterator pChild;
      pair<multimap<int, int>::const_iterator, multimap<int, int>::const_iterator> vScore;
      pair<multimap<int, double>::const_iterator, multimap<int, double>::const_iterator> tScore;

      double vray = 0.0, slicecount = 0.0, totalThickness = 0.0;
      int segmentCount = 0;
      while (!stop) {

    	  // TODO now traversing parentChildTree
    	  // when we have mapping between segments & branches, replace traversal with fixed trajectory
    	  pChild = parentChildTree.find(nextNode);
    	  if (pChild == parentChildTree.end()) stop = true;

    	  // Loop through range of children -- get only first
    	  vScore = validityScore.equal_range(nextNode);
    	  tScore = totalScore.equal_range(nextNode);

    	  multimap<int, int>::const_iterator it2;
    	  multimap<int, double>::const_iterator it3;
    	  for (it2 = vScore.first, it3 = tScore.first;
    			  it2 !=vScore.second;
    			  ++it2, ++it3)
    	  {
    		  int tempV = (*it2).second;
    		  double tempT = (*it3).second;
    		  //    	  ccout << "  [" << (*it2).first << ", " << (*it2).second << "]" << endl;
    		  if (tempV>0) {
    			  slicecount++;
    			  vray += tempV;
    			  totalThickness += (tempT/tempV);
    			  ccout << "slicecount, " << slicecount <<", validityScore: " << tempV << ", ";
    			  ccout << "thicknessScore: " << (tempT/tempV) << " for "<< tempV << " rays" << endl;
    		  }
    	  }
//    	  for (multimap<int, double>::const_iterator it2 = tScore.first;
//    			  it2 !=tScore.second;
//    			  ++it2)
//    	  {
//    		  double tempT = (*it2).second;
//    		  totalThickness += tempT;
//    		  //    	  ccout << "  [" << (*it2).first << ", " << (*it2).second << "]" << endl;
//    		  ccout << "tempScore: " << tempT << endl;
//    	  }
    	  ++segmentCount;
    	  rootNode = nextNode;
    	  nextNode = parentChildTree.find(nextNode)->second;
		  ccout << "1st child: " << nextNode << endl;

//    	  stop = true;
      }
      fout3 << "QC Score (Valid Ray Count, Num of Valid Slices) - TOTAL " << endl;
      ccout << "ValidityScore: " << (vray / slicecount) << "( "<< slicecount << "slices)" << endl;
      ccout << "Mean Thickness: " << (totalThickness / slicecount) << "( "<< slicecount << "slices)" << endl;

      fout3 << "ValidityScore: " << (vray / slicecount) << "( "<< slicecount << "slices)" << endl;
      fout3 << "Mean Thickness: " << (totalThickness / slicecount) << "( "<< slicecount << "slices)" << endl;
*/
//      fout3 << "QC Score (Valid Ray Count, Num of Valid Slices) - TOTAL " << endl;
//      fout3 << "ValidityScore: " << (vray / slicecount) << "( "<< slicecount << "slices)" << endl;
//      fout3 << "Mean Thickness: " << (totalThickness / slicecount) << "( "<< slicecount << "slices)" << endl;

      fout3.close();
      fout3p.close();
      ccout.close();
//      coutSfwhm.close();
//      coutSibm.close();
    }
  ftime(&actualTime);
  std::cout << "Total time: " << (float)(actualTime.time-time0.time+(actualTime.millitm-time0.millitm)/1000.0) << "s." << std::endl;

  return 0;
}



