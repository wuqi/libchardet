/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Mozilla's universal charset detector C/C++ Wrapping API
 *      Writer(s) :
 *          Detect class by John Gardiner Myers <jgmyers@proofpoint.com>
 *          C wrapping API by JoungKyun.Kim <http://oops.org>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <prmem.h>
#include <nscore.h>
#include <nsUniversalDetector.h>
#include <vector>
#include <string>
#include <algorithm>

class Detector: public nsUniversalDetector {
	public:
		Detector ()
		: nsUniversalDetector (NS_FILTER_ALL) {}
		virtual ~Detector () {}
		const char *getCharsetName () { return mDetectedCharset; }
		float getConfidence () { return mDetectedConfidence; }
		short getIsBOM () { return mDetectedIsBOM; }
		virtual void Reset () { this->nsUniversalDetector::Reset (); }
	protected:
		virtual void Report (const char* aCharset) { mDetectedCharset = aCharset; }
};

typedef struct Detect_t {
	Detector *detect;
} Detect;

#include <chardet.h>

CHARDET_API bool isUTF8(const char * input)
{
	CodeType encodingType;
	float confidence;
	bool withBom;
	if (detectCode(input, encodingType, confidence, withBom) == CHARDET_SUCCESS \
		&& encodingType == Code_UTF8 )
	{
		return true;
	}
	return false;
}

CHARDET_API short detectCode(const char * input, CodeType & encodingType, float & confidence, bool & bom)
{
	DetectObj *obj;
	if ((obj = detect_obj_init()) == NULL) {
		fprintf(stderr, "Memory Allocation failed\n");
		return CHARDET_MEM_ALLOCATED_FAIL;
	}

	short  aa = detect_r(input, strlen(input), &obj);
	switch (detect_r(input, strlen(input), &obj))
	{
	case CHARDET_OUT_OF_MEMORY:
		fprintf(stderr, "On handle processing, occured out of memory\n");
		detect_obj_free(&obj);
		return CHARDET_OUT_OF_MEMORY;
	case CHARDET_NULL_OBJECT:
		fprintf(stderr,
			"2st argument of chardet() is must memory allocation "
			"with detect_obj_init API\n");
		return CHARDET_NULL_OBJECT;
	case CHARDET_NO_RESULT:
		fprintf(stderr, "can't match any code\n");
		return CHARDET_NULL_OBJECT;
	}
	if (!obj) {
		fprintf(stderr, "can't match any code\n");
		return CHARDET_NULL_OBJECT;
	}
	std::string str[] = { "UTF-8","windows-1252","GB18030","Big5","EUC-JP","EUC-KR","Shift_JIS","EUC-TW" };
	std::vector<std::string> codeList(str, str + 8);
	std::vector <std::string>::iterator iElement = std::find(codeList.begin(), codeList.end(), obj->encoding);
	if (iElement != codeList.end())
	{
		int nPosition = std::distance(codeList.begin(), iElement);
		encodingType = (CodeType)(nPosition + 1);
	}
	confidence = obj->confidence;
	//if (confidence < 0.6) { encodingType = Code_UnKnown; }
	bom = obj->bom;

	detect_obj_free(&obj);
	return CHARDET_SUCCESS;
}

CHARDET_API char * detect_version (void) {
	return (char *) LIBCHARDET_VERSION;
}

CHARDET_API char * detect_uversion (void) {
	return (char *) LIBCHARDET_UVERSION;
}

CHARDET_API DetectObj * detect_obj_init (void) {
	DetectObj * obj;

	if ( (obj = (DetectObj *) PR_Malloc (sizeof (DetectObj))) == NULL )
		return NULL;

	obj->encoding = NULL;
	obj->confidence = 0.0;
	obj->bom = 0;

	return obj;
}

CHARDET_API void detect_obj_free (DetectObj ** obj) {
	if ( *obj != NULL ) {
		PR_FREEIF ((*obj)->encoding);
		PR_FREEIF (*obj);
	}
}

CHARDET_API Detect * detect_init (void) {
	Detect *det = NULL;

	det = (Detect *) PR_Malloc (sizeof (Detect));

	if ( det == NULL )
		return NULL;

	det->detect	= new Detector;
	return det;
}

CHARDET_API void detect_reset (Detect **det) {
	(*det)->detect->Reset ();
}

CHARDET_API void detect_dataend (Detect **det) {
	(*det)->detect->DataEnd ();
}

CHARDET_API short detect_handledata (Detect ** det, const char * buf, DetectObj ** obj) {
	return detect_handledata_r (det, buf, strlen (buf), obj);
}

CHARDET_API short detect_handledata_r (Detect ** det, const char * buf, size_t buflen, DetectObj ** obj) {
	const char * ret;

	if ( (*det)->detect->HandleData (buf, buflen) == NS_ERROR_OUT_OF_MEMORY )
		return CHARDET_OUT_OF_MEMORY;
	(*det)->detect->DataEnd ();

	ret = (*det)->detect->getCharsetName ();

	if ( ! ret )
		return CHARDET_NO_RESULT;
	else if ( *obj == NULL )
		return CHARDET_NULL_OBJECT;

	(*obj)->encoding = (char *) _strdup (ret);
	(*obj)->confidence = (*det)->detect->getConfidence ();
	(*obj)->bom = (*det)->detect->getIsBOM ();

	return CHARDET_SUCCESS;
}

CHARDET_API void detect_destroy (Detect **det) {
	delete (*det)->detect;
	PR_FREEIF (*det);
}

CHARDET_API short detect (const char *buf, DetectObj ** obj) {
	return detect_r (buf, strlen (buf), obj);
}

CHARDET_API short detect_r (const char *buf, size_t buflen, DetectObj ** obj) {
	Detector * det;
	const char * ret;

	det = new Detector;
	det->Reset ();
	if ( det->HandleData (buf, buflen) == NS_ERROR_OUT_OF_MEMORY ) {
		delete det;
		return CHARDET_OUT_OF_MEMORY;
	}
	det->DataEnd ();

	ret = det->getCharsetName ();
	delete det;

	if ( ! ret )
		return CHARDET_NO_RESULT;
	else if ( *obj == NULL )
		return CHARDET_NULL_OBJECT;

	(*obj)->encoding = (char *) _strdup (ret);
	(*obj)->confidence = det->getConfidence ();
	(*obj)->bom = det->getIsBOM ();

	return CHARDET_SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
