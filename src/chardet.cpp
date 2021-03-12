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

#ifdef _MSC_VER
#define _strdup strdup
#endif
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

	(*obj)->encoding = (char *)strdup(ret);
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

	(*obj)->encoding = (char *)strdup(ret);
	(*obj)->confidence = det->getConfidence ();
	(*obj)->bom = det->getIsBOM ();

	return CHARDET_SUCCESS;
}
//https://github.com/lytsing/gbk-utf8
/**
 *  Copyright (C) 2008  Huang Guan
 *  Copyright (C) 2011  iBoxpay.com inc.
 *
 *  $Id$
 *
 *  Description: This file mainly includes the functions about utf8
 *
 *  History:
 *  2008-7-10 13:31:57 Created.
 *  2011-12-28 Format the code style, and add comments by Lytsing
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef _WIN32
#include <windows.h>
#else
#include <iconv.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>

#ifdef _WIN32
void utf8ToGBK(const char* src, char* dst, int len)
{
	int ret = 0;
	WCHAR* strA;
	int i = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	if (i <= 0) {
		printf("ERROR.");
		return;
	}
	strA = (WCHAR*)malloc(i * 2);
	MultiByteToWideChar(CP_UTF8, 0, src, -1, strA, i);
	i = WideCharToMultiByte(CP_ACP, 0, strA, -1, NULL, 0, NULL, NULL);
	if (len >= i) {
		ret = WideCharToMultiByte(CP_ACP, 0, strA, -1, dst, i, NULL, NULL);
		dst[i] = 0;
	}
	if (ret <= 0) {
		free(strA);
		return;
	}

	free(strA);
}

void gbkToUtf8(const char* src, char* dst, int len)
{
	int ret = 0;
	WCHAR* strA;
	int i = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
	if (i <= 0) {
		printf("ERROR.");
		return;
	}
	strA = (WCHAR*)malloc(i * 2);
	MultiByteToWideChar(CP_ACP, 0, src, -1, strA, i);
	i = WideCharToMultiByte(CP_UTF8, 0, strA, -1, NULL, 0, NULL, NULL);
	if (len >= i) {
		ret = WideCharToMultiByte(CP_UTF8, 0, strA, -1, dst, i, NULL, NULL);
		dst[i] = 0;
	}

	if (ret <= 0) {
		free(strA);
		return;
	}
	free(strA);
}
#else   //Linux
 // starkwong: In iconv implementations, inlen and outlen should be type of size_t not uint, which is different in length on Mac
void utf8ToGBK(const char* src, char* dst, int len)
{
	int ret = 0;
	size_t inlen = strlen(src) + 1;
	size_t outlen = len;

	// duanqn: The iconv function in Linux requires non-const char *
	// So we need to copy the source string
	char* inbuf = (char *)malloc(len);
	char* inbuf_hold = inbuf;   // iconv may change the address of inbuf
								// so we use another pointer to keep the address
	memcpy(inbuf, src, len);

	char* outbuf = dst;
	iconv_t cd;
	cd = iconv_open("GBK", "UTF-8");
	if (cd != (iconv_t)-1) {
		ret = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
		if (ret != 0) {
			printf("iconv failed err: %s\n", strerror(errno));
		}

		iconv_close(cd);
	}
	free(inbuf_hold);   // Don't pass in inbuf as it may have been modified
}

void gbkToUtf8(const char* src, char* dst, int len)
{
	int ret = 0;
	size_t inlen = strlen(src) + 1;
	size_t outlen = len;

	// duanqn: The iconv function in Linux requires non-const char *
	// So we need to copy the source string
	char* inbuf = (char *)malloc(len);
	char* inbuf_hold = inbuf;   // iconv may change the address of inbuf
								// so we use another pointer to keep the address
	memcpy(inbuf, src, len);

	char* outbuf2 = NULL;
	char* outbuf = dst;
	iconv_t cd;

	// starkwong: if src==dst, the string will become invalid during conversion since UTF-8 is 3 chars in Chinese but GBK is mostly 2 chars
	if (src == dst) {
		outbuf2 = (char*)malloc(len);
		memset(outbuf2, 0, len);
		outbuf = outbuf2;
	}

	cd = iconv_open("UTF-8", "GBK");
	if (cd != (iconv_t)-1) {
		ret = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
		if (ret != 0)
			printf("iconv failed err: %s\n", strerror(errno));

		if (outbuf2 != NULL) {
			strcpy(dst, outbuf2);
			free(outbuf2);
		}

		iconv_close(cd);
	}
	free(inbuf_hold);   // Don't pass in inbuf as it may have been modified
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
